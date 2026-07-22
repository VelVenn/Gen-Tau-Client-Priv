#pragma once

#include "runtime/GAppContext.hpp"

#include <QtQmlIntegration/qqmlintegration.h>
#include <QJSEngine>
#include <QQmlEngine>

namespace gentau {
class GAppQmlContext
{
	Q_GADGET

	QML_FOREIGN(GAppContext)
	QML_NAMED_ELEMENT(Context)
	QML_SINGLETON

  public:
	static void setInstance(GAppContext* instance)
	{
		Q_ASSERT(instance);

		_instance = instance;
	}

	static GAppContext* create(QQmlEngine*, QJSEngine* engine) {
		Q_ASSERT(_instance);
		Q_ASSERT(engine->thread() == _instance->thread());

		if(_engine) {
			Q_ASSERT(_engine == engine);
		} else {
			_engine = engine;
		}

		QJSEngine::setObjectOwnership(_instance, QJSEngine::CppOwnership);

		return _instance;
	}

  private:
	inline static GAppContext* _instance{ nullptr };
	inline static QJSEngine*   _engine{ nullptr };
};
}  // namespace gentau
