#pragma once

#include <QObject>
#include <QProtobufSerializer>
#include <QString>

#include "hdvt.qpb.h"

#include "adapter/mqtt/GMqttAdapter.hpp"
#include "img_trans/vid_render/TBytesVidRender.hpp"

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
	QProtobufSerializer  _serializer;
	gentau::GMqttAdapter _client;

	gentau::TBytesVidRender::SharedPtr _bVidRend;

	std::ofstream recvDump;

  private:
	void                             initRecv();
	gentau::GMqttAdapter::BindResult bindClient(const QString& clientId);
	void                             switchClient(const QString& newId);

  public:
	explicit VTRecv(gentau::TBytesVidRender::SharedPtr vidRender, QObject* parent = nullptr);

	~VTRecv() override;
};
