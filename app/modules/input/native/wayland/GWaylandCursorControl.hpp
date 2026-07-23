#pragma once

#include "native/GCursorControlBackend.hpp"

#include "qwayland-pointer-constraints-unstable-v1.h"
#include "qwayland-relative-pointer-unstable-v1.h"

#include <QCursor>
#include <QPointer>

#include <atomic>
#include <cstdint>

class QWindow;

struct wl_display;
struct wl_pointer;
struct wl_registry;
struct wl_surface;

namespace gentau {
class GWaylandCursorControl final : public GCursorControlBackend,
									private QtWayland::zwp_pointer_constraints_v1,
									private QtWayland::zwp_locked_pointer_v1,
									private QtWayland::zwp_relative_pointer_manager_v1,
									private QtWayland::zwp_relative_pointer_v1
{
  public:
	GWaylandCursorControl(QWindow* window, LockStateChangedCallback lockStateChanged);
	~GWaylandCursorControl() override;

	bool lock() override;
	void unlock() override;

	bool isLockSupported() const noexcept override;

	QPointF getDeltaMovement(GCursorControl::MovementMode mode) noexcept override;

  private:
	static void registryGlobal(
		void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version
	);
	static void registryGlobalRemove(void* data, wl_registry* registry, uint32_t name);

	void initializeRelativePointer();
	void clearDeltaMovement() noexcept;
	void hideCursor();
	void restoreCursor();

	void zwp_locked_pointer_v1_locked() override;
	void zwp_locked_pointer_v1_unlocked() override;
	void zwp_relative_pointer_v1_relative_motion(
		uint32_t   utimeHi,
		uint32_t   utimeLo,
		wl_fixed_t dx,
		wl_fixed_t dy,
		wl_fixed_t dxUnaccelerated,
		wl_fixed_t dyUnaccelerated
	) override;

  private:
	QPointer<QWindow> _window;
	wl_display*       _display{ nullptr };
	wl_registry*      _registry{ nullptr };
	wl_pointer*       _pointer{ nullptr };
	wl_surface*       _surface{ nullptr };

	std::atomic<std::int64_t> _acceleratedX{ 0 };
	std::atomic<std::int64_t> _acceleratedY{ 0 };
	std::atomic<std::int64_t> _unacceleratedX{ 0 };
	std::atomic<std::int64_t> _unacceleratedY{ 0 };
	std::atomic_bool          _recordRelativeMotion{ false };

	QCursor _savedCursor;
	bool    _cursorHidden{ false };
	bool    _lockPending{ false };
};
}  // namespace gentau
