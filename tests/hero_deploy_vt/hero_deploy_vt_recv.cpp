#include "hero_deploy_vt_recv.hpp"
#include <qcontainerfwd.h>
#include <qnamespace.h>

#include "comm/TMqttClient.hpp"
#include "hdvt.qpb.h"
#include "img_trans/vid_render/TVidUtils.hpp"
#include "utils/TLog.hpp"

#include <QQmlContext>

#include "fmt/ranges.h"

#include <chrono>
#include <span>

#define T_LOG_TAG "[Hero VDT VM] "

using namespace std;
using namespace gentau;

#define testRegTopicLog(name)                                                                      \
	{                                                                                              \
		auto conn = _client->registerTopic((name), [this](const string& payload) {                 \
			tLogDebug("Recieved topic: {}", (name));                                               \
		});                                                                                        \
		connList.push_back(conn);                                                                  \
	}

VTRecv::VTRecv(
	TBytesVidRender::SharedPtr vidRender, TMqttClient::SharedPtr client, QObject* parent
) :
	_bVidRend(std::move(vidRender)),
	_client(std::move(client))
{
	initRecv();
}

VTRecv::~VTRecv()
{
	if (recvDump.is_open()) { recvDump.close(); }
}

void VTRecv::initRecv()
{
	if (!_bVidRend || !_client) {
		tLogError("Invalid Bytes vid render module or mqtt client module");
		return;
	}

	connect(
		this,
		&VTRecv::restartVidRendRequested,
		this,
		[this] {
			if (_bVidRend) {
				_bVidRend->flush();
				_bVidRend->restart();
			}
		},
		Qt::QueuedConnection
	);

#if (defined(USE_LOCAL) && USE_LOCAL != 1) && (defined(DUMP_RECV) && DUMP_RECV == 1)
	recvDump.open("./res/recv_dump.h265", ios::binary | ios::trunc);
#endif
	_client->connect();
	auto conn = _client->registerTopic("CustomByteBlock", [this](const string& payload) {
		Gentau::Topics::CustomByteBlock msg;
		msg.deserialize(&_serializer, QByteArrayView(payload.data(), payload.size()));

		if (_serializer.lastError() != QAbstractProtobufSerializer::Error::None) {
			tLogWarn(
				"Failed to deserialize CustomByteBlock: {}",
				_serializer.lastErrorString().toStdString()
			);

			return;
		}

		if (!msg.hasData()) {
			tLogWarn("Received CustomByteBlock message without block data");
			return;
		}

		auto vidPkt = msg.data();

		span<const u8> frame(
			reinterpret_cast<const u8*>(vidPkt.data()), static_cast<size_t>(vidPkt.size())
		);

#if (defined(USE_LOCAL) && USE_LOCAL != 1) && (defined(DUMP_RECV) && DUMP_RECV == 1)
		if (recvDump.is_open()) {
			recvDump.write(reinterpret_cast<const char*>(frame.data()), frame.size());
		}
#endif
		// auto dumpSpan = span<const u8>(frame.begin(), frame.end());
		// tLogInfo("Deploy VT Dump: {}", fmt::format("{:x}", fmt::join(dumpSpan, " ")));

		_bVidRend->tryPushFrame(frame);
	});

	connList.push_back(conn);
}

void VTRecv::requestClientSwitch(const QString& newId)
{
	if (newId == QAnyStringView(_client->getClientId())) {
		tLogWarn("New id same as old: '{}', ignored", _client->getClientId());
		return;
	}

	Q_EMIT clientSwitchRequested(newId);
}

void VTRecv::requestRestartVidRend()
{
	Q_EMIT restartVidRendRequested();
}

void VTRecv::clientSwitchHandler(QQmlEngine& engine, const QString& newId)
{
	auto* oldRecv =
		qobject_cast<VTRecv*>(engine.rootContext()->contextProperty("vtRecv").value<QObject*>());

	if (!oldRecv) { return; }

	if (!oldRecv->connList.empty()) {
		for (auto& conn : oldRecv->connList) { conn.disconnect(); }
	}
	oldRecv->_client.reset();

	oldRecv->_bVidRend->flush();
	oldRecv->_bVidRend->restart();  // 清空渲染管线的旧数据，等待新的配置信息

	auto* newRecv = new VTRecv(
		oldRecv->_bVidRend, TMqttClient::create(newId.toStdString(), CLIENT_URI), &engine
	);

	engine.rootContext()->setContextProperty("vtRecv", newRecv);

	QObject::connect(
		newRecv,
		&VTRecv::clientSwitchRequested,
		&engine,
		[&engine](const QString& id) { VTRecv::clientSwitchHandler(engine, id); },
		Qt::QueuedConnection
	);

	oldRecv->deleteLater();
}