#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRunnable>

#include "img_trans/TImgTrans.hpp"
#include "img_trans/vid_render/TBytesVidRender.hpp"

#include "adapter/mqtt/GMqttAdapter.hpp"

#include <atomic>
#include <memory>

namespace gentau {
class GAppContext : public QObject
{
	Q_OBJECT
  private:
	class GInitVidRenderTask;

  public:
	TImgTrans&       imgTrans() noexcept { return *_imgTrans; }
	TBytesVidRender& deployVtRender() noexcept { return *_deployVtRender; }
	GMqttAdapter&    client() noexcept { return _client; }

	quint64 clientGen() const noexcept { return _clientGen.load(); }

  private:
	void finalizeVidRenderersInit(bool success);

  public:
	void bindVidRendToWindow(QQuickWindow* window);

  private:
	TImgTrans::SharedPtr       _imgTrans{ nullptr };
	TBytesVidRender::SharedPtr _deployVtRender{ nullptr };

	GMqttAdapter _client;

	std::atomic<quint64> _clientGen{ 0 };

  public:
	[[nodiscard]] static std::unique_ptr<GAppContext> create(QObject* parent = nullptr);

	explicit GAppContext(QObject* parent = nullptr);
	~GAppContext() override;
};
}  // namespace gentau