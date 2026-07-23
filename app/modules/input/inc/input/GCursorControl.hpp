#pragma once

#include <QObject>
#include <QPointF>

#include <atomic>
#include <memory>

class QWindow;

namespace gentau {
class GCursorControlBackend;

class GCursorControl : public QObject
{
	Q_OBJECT

  public:
	enum class MovementMode : quint8
	{
		Accelerated = 0,
		Unaccelerated
	};

	enum class LockState : quint32
	{
		Unsupported = 0,
		Unlocked,
		Pending,
		Locked
	};
	Q_ENUM(LockState)

  Q_SIGNALS:
	void lockStateChanged(LockState state);

  public:
	void lock(); // TODO: considering add retry if pending timeout occurs
	void unlock();

	QPointF captureDeltaMovement(MovementMode mode = MovementMode::Accelerated) noexcept;

  public:
	LockState lockState() const noexcept { return _lockState.load(std::memory_order_acquire); }

	bool isLocked() const noexcept { return lockState() == LockState::Locked; }

	bool isLockSupported() const noexcept;

  public:
	explicit GCursorControl(QWindow* window, QObject* parent = nullptr);
	~GCursorControl();

  private:
	void updateLockState(LockState state);

	std::unique_ptr<GCursorControlBackend> _backend;

	std::atomic<LockState> _lockState{ LockState::Unsupported };
};
}  // namespace gentau
