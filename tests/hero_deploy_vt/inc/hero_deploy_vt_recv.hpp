#pragma once

#include <QObject>
#include <QProtobufSerializer>
#include <QQmlEngine>
#include <QString>

#include <vector>

#include "hdvt.qpb.h"

#include "comm/TMqttClient.hpp"
#include "img_trans/vid_render/TBytesVidRender.hpp"
#include "utils/TSignal.hpp"

#include <fstream>

#define LOCAL  "mqtt://127.0.0.1:3333"
#define REMOTE "mqtt://192.168.12.1:3333"

#define USE_LOCAL 0

#define DUMP_RECV 0

#if USE_LOCAL
#define CLIENT_URI LOCAL
#else
#define CLIENT_URI REMOTE
#endif

class VTRecv : public QObject
{
	Q_OBJECT
  public:
	Q_INVOKABLE void requestClientSwitch(const QString& newId);
	Q_INVOKABLE void requestRestartVidRend();

  Q_SIGNALS:
	void clientSwitchRequested(const QString& newId);
	void restartVidRendRequested();

  private:
	QProtobufSerializer            _serializer;
	gentau::TMqttClient::SharedPtr _client;

	gentau::TBytesVidRender::SharedPtr _bVidRend;

	std::vector<gentau::Connection> connList;

	std::ofstream recvDump;

  private:
	void initRecv();

  public:
	static void clientSwitchHandler(QQmlEngine& engine, const QString& newId);

  public:
	explicit VTRecv(
		gentau::TBytesVidRender::SharedPtr vidRender,
		gentau::TMqttClient::SharedPtr     client = gentau::TMqttClient::create("1", CLIENT_URI),
		QObject*                           parent = nullptr
	);

	~VTRecv() override;
};