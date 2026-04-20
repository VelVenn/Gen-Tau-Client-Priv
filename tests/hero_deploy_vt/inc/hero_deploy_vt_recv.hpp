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

#define CLIENT_URI ("mqtt://127.0.0.1:3333")

class VTRecv : public QObject
{
	Q_OBJECT
  public:
	Q_INVOKABLE void requestClientSwitch(const QString& newId);

  Q_SIGNALS:
	void clientSwitchRequested(const QString& newId);

  private:
	QProtobufSerializer            _serializer;
	gentau::TMqttClient::SharedPtr _client;

	gentau::TBytesVidRender::SharedPtr _bVidRend;

    std::vector<gentau::Connection> connList;

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