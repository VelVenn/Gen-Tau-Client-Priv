#pragma once

#include "img_trans/vid_render/TFramePool.hpp"
#include "img_trans/vid_render/TVidUtils.hpp"

#include "utils/TSignal.hpp"
#include "utils/TTypeRedef.hpp"

#include <atomic>
#include <memory>
#include <thread>

class QQuickItem;

namespace gentau {
class TReassembly;

class TReassemblyPasskey
{
	friend class TReassembly;
	TReassemblyPasskey() = default;
};

class TVidRender : public std::enable_shared_from_this<TVidRender>
{
  private:
	using FramePtr   = vid::FramePtr;
	using ElemRawPtr = vid::ElemRawPtr;
	using StateType  = vid::StateType;
	using IssueType  = vid::IssueType;
	using TimePoint  = vid::TimePoint;

  public:
	using SharedPtr = std::shared_ptr<TVidRender>;

  private:
	TFramePool framePool;

  private:
	ElemRawPtr fixedPipe;  // Should not changed the pointer after init
	ElemRawPtr fixedSrc;   // Should not changed the pointer after init
	ElemRawPtr fixedSink;  // Should not changed the pointer after init

  public:
	TSignal<TVidRender> onEOS;  // End of stream detected

	// `vid::IssueType`, `std::string` (src), `std::string` (msg), `std::string` (debug info)
	vid::BusErrSignal<TVidRender> onPipeError;

	// `vid::IssueType`, `std::string` (src), `std::string` (msg), `std::string` (debug info)
	vid::BusErrSignal<TVidRender> onPipeWarn;

	// `vid::StateType` (old state), `vid::StateType` (new state)
	vid::BusStateChangeSignal<TVidRender> onStateChanged;

  private:
	std::atomic<TimePoint> lastPushSuccess = TimePoint::min();
	std::atomic<u64>       maxBufferBytes  = 262'144;  // Default to 256 KB

	const bool useFileSrc;
	const bool enableTestMode;

  private:
	std::jthread busThread;

  public:
	// MT-SAFE
	u64 getMaxBufferBytes() const { return maxBufferBytes.load(); }

	// MT-SAFE, but should be used with caution as it may cause pushFrame to fail if set too low.
	void setMaxBufferBytes(u64 bytes) { maxBufferBytes.store(bytes); }

	// MT-SAFE
	TimePoint getLastPushSuccessTime() const { return lastPushSuccess.load(); }

  private:
	static void onDecoderPadAdded(GstElement* decoder, GstPad* new_pad, gpointer user_data);

	GstElement* choosePrefDecoder(bool& isDynamic);

  private:
	bool initPipeElements(bool useFileSrc, const char* file_path = nullptr);
	bool initBusThread();

  private:
	GstElement* pipeline() { return this->fixedPipe; }
	GstElement* src() { return this->fixedSrc; }
	GstElement* sink() { return this->fixedSink; }

  public:
	/**
	 * @brief 尝试推送一帧数据到渲染管道中。
	 * @return 在帧数据成功推送到管道返回 true，否则返回 false。
	 * @note 该方法仅适用于测试目的，仅在 Debug 构建且 `TVidRender::enableTestMode`
	 *       为 true 时才会执行推送逻辑，其他情况下调用此方法将直接返回 false。
	 * @note 该方法当且仅当存在单一调用者时才是线程安全的，请勿在多个线程中并发调用此方法。
	 */
	[[deprecated("Never use this for any general purpose")]]
	bool __TEST_ONLY_tryPushFrame_UNSAFE_WHO_USE_WHO_SB_(FramePtr frame);

	/**
	 * @brief 尝试推送一帧数据到渲染管道中。
	 * @return 在帧数据成功推送到管道返回 true，否则返回 false。
	 * @note 该方法仅能在 TReassembly 类内部被正常调用，其他地方调用此方法将导致编译
	 *       错误。该方法当且仅当存在单一调用者时才是线程安全的，请勿在多个线程中并发调
	 *       用此方法。
	 */
	bool tryPushFrame(TFramePool::FrameData&& frame, TReassemblyPasskey);

	/**
	 * @brief 尝试获取一个可用的帧数据槽位。
	 * @return std::optional<TFramePool::FrameData>
	 * @note 该方法仅能在 TReassembly 类内部被正常调用，其他地方调用此方法将导致编译
	 *       错误。该方法当且仅当存在单一调用者时才是线程安全的，请勿在多个线程中并发调
	 *       用此方法。
	 */
	auto acquireFrameSlot(TReassemblyPasskey) { return framePool.acquire(); }

  public:
	/** 
	 * @brief Let the pipeline start playing. 
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		  or the thread that created the TVidRender instance.
	 */
	bool play();

	/**
	 * @brief Pause the pipeline.
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TVidRender instance.
	 */
	bool pause();

	/**
	 * @brief Restart the pipeline.
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TVidRender instance. This method 
	 *       will release all the resources to the hardware level. May cause 
	 *       FATAL error on OS X.  
	 */
	bool restart();

	/**
	 * @brief Flush the pipeline. Will only clear the data in the pipeline. 
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TVidRender instance.
	 */
	bool flush();

	/**
	 * @brief Stop the pipeline. Will set pipeline to NULL state and release all resources. 
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TVidRender instance. This method 
	 *       will release all the resources to the hardware level. May cause 
	 *       FATAL error on OS X.  
	 */
	bool stop();

	// MT-SAFE
	vid::StateType getCurrentState();

  public:
	/**
	 * @brief Link the video output sink to a QQuickItem. This method MUST be called before
	 *        the pipeline is set to playing state.
	 * @note NOT MT-SAFE!
	 */
	void linkSinkWidget(QQuickItem* widget);

  public:
	/** 
	 * Post a test error to the pipeline, for testing purpose only.
	 * Only works in Debug builds when TVidRender::enableTestMode is true, 
	 * otherwise it will do nothing.
	 */
	void postTestError();

  public:
	explicit TVidRender(
		u64 _maxBufferBytes = 262'144, bool _enableTestMode = false
	);  // Default to 256 KB
	explicit TVidRender(
		const char* file_path, u64 _maxBufferBytes = 262'144, bool _enableTestMode = false
	);

	/** 
	 * @brief create a shared pointer to TVidRender instance. 
	 *
	 * @throws std::runtime_error if the pipeline initialization failed, or 
	 *         if file_path is provided in non-Debug builds.
	 */
	[[nodiscard("Should not ignored the created TVidRender::SharedPtr")]] static SharedPtr create(
		const char* file_path = nullptr, u64 _maxBufferBytes = 262'144
	)
	{
		if (file_path) {
			return std::make_shared<TVidRender>(file_path, _maxBufferBytes);
		} else {
			return std::make_shared<TVidRender>(_maxBufferBytes);
		}
	}

	/** 
	 * @brief create a shared pointer to TVidRender instance.
	 *
	 * @throws std::runtime_error if the pipeline initialization failed.
	 */
	[[nodiscard("Should not ignored the created TVidRender::SharedPtr")]] static SharedPtr create(
		u64 _maxBufferBytes
	)
	{
		return std::make_shared<TVidRender>(_maxBufferBytes);
	}

	/**
	 * @brief Initialize the GStreamer context. Must be called before creating any TVidRender instance.
	 * @param argc Pointer to the argc parameter from the main function.
	 * @param argv Pointer to the argv parameter from the main function.
	 */
	static void initContext(int* argc, char** argv[]);

	~TVidRender();

  public:
	TVidRender(const TVidRender&)            = delete;  // Forbid copy or move
	TVidRender& operator=(const TVidRender&) = delete;
	TVidRender(TVidRender&&)                 = delete;
	TVidRender& operator=(TVidRender&&)      = delete;
};
}  // namespace gentau