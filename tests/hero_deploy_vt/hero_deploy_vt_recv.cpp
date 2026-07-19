#include "hero_deploy_vt_recv.hpp"

#include "hdvt.qpb.h"
#include "img_trans/vid_render/TVidUtils.hpp"
#include "utils/TLog.hpp"

#include "fmt/ranges.h"

#include <chrono>
#include <span>

#define T_LOG_TAG "[Hero VDT VM] "

using namespace std;
using namespace gentau;

VTRecv::VTRecv(TBytesVidRender::SharedPtr vidRender, QObject* parent) :
	QObject(parent),
	_bVidRend(std::move(vidRender))
{
	initRecv();
}

VTRecv::~VTRecv()
{
	if (recvDump.is_open()) { recvDump.close(); }
}

void VTRecv::initRecv()
{
	if (!_bVidRend) {
		tLogError("Invalid Bytes vid render module");
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
	connect(
		this, &VTRecv::clientSwitchRequested, this, &VTRecv::switchClient, Qt::QueuedConnection
	);

#if (defined(USE_LOCAL) && USE_LOCAL != 1) && (defined(DUMP_RECV) && DUMP_RECV == 1)
	recvDump.open("./res/recv_dump.h265", ios::binary | ios::trunc);
#endif

	(void)bindClient(QStringLiteral("1"));
}

auto VTRecv::bindClient(const QString& clientId) -> GMqttAdapter::BindResult
{
	auto bindResult = _client.bind(clientId, QStringLiteral(CLIENT_URI));
	if (!bindResult.succeeded()) {
		tLogError(
			"Failed to bind MQTT client '{}': {}",
			clientId.toStdString(),
			bindResult.failedReason.toStdString()
		);
		return bindResult;
	}

	if (!bindResult.changed()) { return bindResult; }

	auto registerResult = _client.registerTopic(
		this,
		"CustomByteBlock",
		[this](const QByteArray& payload) {
			Gentau::Topics::CustomByteBlock msg;
			msg.deserialize(&_serializer, QByteArrayView(payload.constData(), payload.size()));

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
		}
	);

	if (!registerResult.succeeded()) {
		tLogError("Failed to register CustomByteBlock: {}", registerResult.cause.toStdString());
	}

	return bindResult;
}

void VTRecv::requestClientSwitch(const QString& newId)
{
	Q_EMIT clientSwitchRequested(newId);
}

void VTRecv::requestRestartVidRend()
{
	Q_EMIT restartVidRendRequested();
}

void VTRecv::switchClient(const QString& newId)
{
	const auto bindResult = bindClient(newId);
	if (!bindResult.changed()) { return; }

	_bVidRend->flush();
	_bVidRend->restart();  // 清空渲染管线的旧数据，等待新的配置信息
}
