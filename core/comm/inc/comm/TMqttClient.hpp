#pragma once

#include "utils/TSignal.hpp"
#include "utils/TTypeRedef.hpp"

#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace mqtt {
class async_client;
class connect_options;
}  // namespace mqtt

namespace gentau {
class TMqttClient : std::enable_shared_from_this<TMqttClient>
{
  public:
	using SharedPtr = std::shared_ptr<TMqttClient>;
	using WeakRef   = std::weak_ptr<TMqttClient>;

	using ReceiveHandler   = std::function<void(const std::string& /*Payload*/)>;
	using HandlerSignal    = TSignal<TMqttClient, const std::string&>;
	using HandlerSignalPtr = std::shared_ptr<HandlerSignal>;
	using TopicRegister    = std::unordered_map<std::string, HandlerSignalPtr>;

  public:
	enum class QoS : i32
	{
		AT_MOST_ONCE  = 0,
		AT_LEAST_ONCE = 1,
		EXACTLY_ONCE  = 2
	};

  private:
	class Callback;

  private:
	std::unique_ptr<mqtt::connect_options> connOpt;
	std::unique_ptr<Callback>              cb;
	std::unique_ptr<mqtt::async_client>    cli;

	TopicRegister topicRegister;

	const std::string clientId;
	const std::string serverURI;

	std::shared_mutex topicRegMtx;

  public:
	TSignal<TMqttClient>                        onConnected;
	TSignal<TMqttClient, std::string /*Cause*/> onConnectionLost;    // `std::string` (cause)
	TSignal<TMqttClient, std::string /*Cause*/> onConnectionFailed;  // `std::string` (cause)

	TSignal<TMqttClient, const std::unordered_set<std::string>& /*Failed Topics*/>
		onSubSyncFailed;  // `const std::unordered_set<std::string>&` (failed topics)

  private:
	void subscribeAll();

  public:
	/**
	 * @brief Get the Client Id
	 * 
	 * @note MT-SAFE
	 */
	std::string_view getClientId() const { return clientId; }

	/**
	 * @brief Get the Server URI
	 * 
	 * @note MT-SAFE
	 */
	std::string_view getServerURI() const { return serverURI; }

  public:
	/**
	 * @brief Publish a message
	 * @param topic The topic to publish to
	 * @param payload The message payload
	 * @param qos The Quality of Service level to publish at. 
	 * @throws mqtt::exception if the publish fails
	 * @note MT-SAFE
	 */
	void publish(
		const std::string& topic, const std::string& payload, QoS qos = QoS::AT_LEAST_ONCE
	);

	/**
     * Register a topic and its corresponding handler. The client will attempt to 
	 * subscribe to the topic if it's not already subscribed.
	 * @param topic The topic to subscribe to
	 * @param handler The handler to call when a message is received on the topic
	 * @return A connection handle that can be used to disconnect the handler
	 *         from the topic. 
	 * @note MT-SAFE, but callers are responsible for the safety of the handler and the connection handle.
	 *       The handler will be called the context of the MQTT client's internal thread.
	 */
	Connection registerTopic(const std::string& topic, ReceiveHandler handler);

	/**
	 * Start the MQTT client and attempt to connect to the server.
	 * @note NOT MT-SAFE!
	 */
	void connect();

  public:
	/**
	 * @brief Create a new TMqttClient instance.
	 * @param _clientId The client ID to use for the MQTT connection
	 * @param _serverURI The URI of the MQTT server to connect to
	 * @throws std::invalid_argument or mqtt::exception if the client ID or server URI is invalid
	 */
	explicit TMqttClient(
		const std::string& _clientId, const std::string& _serverURI = "mqtt://localhost:3333"
	);

	/**
	 * @brief Create a shared pointer to a new TMqttClient instance.
	 * @param _clientId The client ID to use for the MQTT connection
	 * @param _serverURI The URI of the MQTT server to connect to
	 * @return A shared pointer to the created TMqttClient instance
	 * @throws std::invalid_argument or mqtt::exception if the client ID or server URI is invalid
	 */
	[[nodiscard("Should not ignore the created TMqttClient::SharedPtr")]] static SharedPtr create(
		const std::string& _clientId, const std::string& _serverURI = "mqtt://localhost:3333"
	)
	{
		return std::make_shared<TMqttClient>(_clientId, _serverURI);
	}

	// Create a weak reference from a shared pointer to TMqttClient
	static WeakRef weakRef(const SharedPtr& ptr) { return WeakRef(ptr); }

	~TMqttClient();

	TMqttClient(const TMqttClient&)            = delete;
	TMqttClient& operator=(const TMqttClient&) = delete;
	TMqttClient(TMqttClient&&)                 = delete;
	TMqttClient& operator=(TMqttClient&&)      = delete;
};
}  // namespace gentau
