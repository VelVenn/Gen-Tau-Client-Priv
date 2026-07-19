#include "comm/TMqttClient.hpp"

#include "mqtt/exception.h"
#include "utils/TLog.hpp"
#include "utils/TLogical.hpp"

#include "mqtt/async_client.h"
#include "utils/TSignal.hpp"

#include <memory>
#include <mutex>
#include <stdexcept>

#define T_LOG_TAG_COMM "[MQTT Client] "

using namespace mqtt;
using namespace std;
using namespace std::chrono_literals;

namespace gentau {
class TMqttClient::Callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
	TMqttClient* client;

	// Connection failure
	void on_failure(const token& tok) override
	{
		tCommLogError("Connection attempt failed");
		client->onConnectionFailed(
			tok.get_error_message().empty() ? "Unknown" : tok.get_error_message()
		);
	}

	void on_success(const token& tok) override {}

	// Connection success
	void connected(const string& cause) override
	{
		tCommLogInfo(
			"Connection success, client id: {}, server URL: {}", client->clientId, client->serverURI
		);

		client->subscribeAll();

		client->onConnected();
	}

	void connection_lost(const string& cause) override
	{
		tCommLogError("Connection lost, cause: {}", cause.empty() ? "Unknown" : cause);
		client->onConnectionLost(cause.empty() ? "Unknown" : cause);
	}

	void message_arrived(const_message_ptr msg) override
	{
		// if (msg->is_duplicate()) { return; }

		HandlerSignalPtr sigPtr;
		{
			shared_lock lock(client->topicRegMtx);
			auto        iter = client->topicRegister.find(msg->get_topic());

			if (iter == client->topicRegister.end()) {
				tCommLogDebug("Unsubscribed message recieved, topic: {}", msg->get_topic());
				return;
			}

			sigPtr = iter->second;
		}

		sigPtr->emit(msg->get_payload());
	}

	void delivery_complete(delivery_token_ptr token) override {}

  public:
	using UniPtr = unique_ptr<Callback>;

  public:
	explicit Callback(TMqttClient* client) : client(client) {}

	static UniPtr create(TMqttClient* client) { return make_unique<Callback>(client); }
};

TMqttClient::TMqttClient(const string& _clientId, const string& _serverURI) :
	clientId(_clientId),
	serverURI(_serverURI)
{
	if (anyFalse(clientId, serverURI)) {
		throw invalid_argument("Neither client ID nor server URI can be empty");
	}

	cb = Callback::create(this);

	auto createOpt = create_options_builder()
						 .client_id(clientId)
						 .server_uri(serverURI)
						 .mqtt_version(MQTTVERSION_DEFAULT)
						 .persistence(NO_PERSISTENCE)
						 .persist_qos0(false)
						 .restore_messages(false)
						 .send_while_disconnected(false)
						 .delete_oldest_messages()
						 .finalize();

	cli = make_unique<async_client>(createOpt);
	cli->set_callback(*cb);

	connOpt = make_unique<connect_options>(connect_options_builder()
											   .clean_session()
											   .automatic_reconnect(1s, 5s)
											   .connect_timeout(5s)
											   .keep_alive_interval(5s)
											   .max_inflight(25)
											   .finalize());
}

TMqttClient::~TMqttClient()
{
	tCommLogDebug("Shutting down client...");

	try {
		cli->disconnect(0)->wait_for(10ms);
	} catch (const mqtt::exception& exc) {
		tCommLogDebug(
			"Failed to disconnect gracefully, cause: {}, force to disconnect now", exc.what()
		);
	}

	cli.reset();

	cb.reset();
};

void TMqttClient::subscribeAll()
{
	unordered_set<string> failedTopics;
	{
		shared_lock lock(topicRegMtx);
		failedTopics.reserve(topicRegister.size());

		for (auto& topicReg : topicRegister) {
			try {
				cli->subscribe(topicReg.first, static_cast<i32>(QoS::AT_LEAST_ONCE));
			} catch (const mqtt::exception& exc) {
				tCommLogError(
					"Failed to subscribe to topic '{}', cause: {}", topicReg.first, exc.what()
				);
				failedTopics.emplace(topicReg.first);
			}
		}
	}

	if (!failedTopics.empty()) { onSubSyncFailed(failedTopics); }
}

void TMqttClient::publish(const std::string& topic, const std::string& payload, QoS qos)
{
	cli->publish(topic, payload, static_cast<i32>(qos), false);
}

Connection TMqttClient::registerTopic(const std::string& topic, ReceiveHandler handler)
{
	Connection conn;
	bool       isNewTopic = false;
	{
		unique_lock lock(topicRegMtx);
		auto [iter, inserted] = topicRegister.try_emplace(topic, make_shared<HandlerSignal>());

		isNewTopic = inserted;

		conn = iter->second->connect(std::move(handler));
	}

	if (isNewTopic) {
		try {
			cli->subscribe(topic, static_cast<i32>(QoS::AT_LEAST_ONCE));
		} catch (const mqtt::exception& exc) {
			tCommLogDebug(
				"Attempt to subscribe to topic '{}' after register but failed, "
				"cause: {}",
				topic,
				exc.what()
			);
			// Do nothing, usually means the client is not connected.
			// If we check cli->is_connected() then sub may cause check-then-act problem,
			// so we just try-catch and ignore the failure, the subscribe will be retried in
			// the next connection attempt.
		}
	}

	return conn;
}

void TMqttClient::connect()
{
	if (cli->is_connected()) {
		tCommLogWarn("Already connected, ignoring this");
		return;
	}

	try {
		cli->connect(*connOpt, nullptr, *cb);
	} catch (const mqtt::exception& e) {
		tCommLogError("Failed to start connection, cause: {}", e.what());
		onConnectionFailed(e.what() ? e.what() : "Unknown");
	} catch (const std::exception& e) {
		tCommLogError("Failed to start connection, cause: {}", e.what());
		onConnectionFailed(e.what() ? e.what() : "Unknown");
	} catch (...) {
		tCommLogError("Failed to start connection, cause: Unknown");
		onConnectionFailed("Unknown");
	}
}
}  // namespace gentau