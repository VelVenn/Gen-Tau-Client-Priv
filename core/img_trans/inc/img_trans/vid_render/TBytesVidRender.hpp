#pragma once

#include "utils/TSignal.hpp"
#include "utils/TTypeRedef.hpp"

#include "img_trans/vid_render/TVidUtils.hpp"

#include <atomic>
#include <memory>
#include <span>
#include <thread>

class QQuickItem;

namespace gentau {
class TBytesVidRender : public std::enable_shared_from_this<TBytesVidRender>
{
  private:
	using ElemRawPtr = vid::ElemRawPtr;
	using StateType  = vid::StateType;
	using IssueType  = vid::IssueType;
	using TimePoint  = vid::TimePoint;

  public:
	using SharedPtr = std::shared_ptr<TBytesVidRender>;

  private:
	ElemRawPtr fixedPipe = nullptr;
	ElemRawPtr fixedSrc  = nullptr;
	ElemRawPtr fixedSink = nullptr;

	TSignal<TBytesVidRender>                   onEOS;
	vid::BusErrSignal<TBytesVidRender>         onPipeError;
	vid::BusErrSignal<TBytesVidRender>         onPipeWarn;
	vid::BusStateChangeSignal<TBytesVidRender> onStateChanged;

  private:
	std::atomic<TimePoint> lastPushSuccess = TimePoint::min();
	std::atomic<u64>       maxBufferBytes  = 65'535;  // Default to 64 KiB

  private:
	std::jthread busThread;

  public:
	// MT-SAFE
	u64 getMaxBufferBytes() const { return maxBufferBytes.load(); }

	// MT-SAFE, but should be used with caution as it may cause pushFrame to fail if set too low.
	void setMaxBufferBytes(u64 bytes) { maxBufferBytes.store(bytes); }

	// MT-SAFE
	vid::TimePoint getLastPushSuccessTime() const { return lastPushSuccess.load(); }

  private:
	static void onDecoderPadAdded(GstElement* decoder, GstPad* new_pad, gpointer user_data);

	GstElement* choosePrefDecoder(bool& isDynamic);

  private:
	bool initPipeElements();
	bool initBusThread();

  private:
	GstElement* pipeline() { return this->fixedPipe; }
	GstElement* src() { return this->fixedSrc; }
	GstElement* sink() { return this->fixedSink; }

  public:
	bool tryPushFrame(std::span<const u8> frameData);

  public:
	/** 
	 * @brief Let the pipeline start playing. 
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		  or the thread that created the TBytesVidRender instance.
	 */
	bool play();

	/**
	 * @brief Pause the pipeline.
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TBytesVidRender instance.
	 */
	bool pause();

	/**
	 * @brief Restart the pipeline.
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TBytesVidRender instance. This method 
	 *       will release all the resources to the hardware level. May cause 
	 *       FATAL error on OS X.  
	 */
	bool restart();

	/**
	 * @brief Flush the pipeline. Will only clear the data in the pipeline. 
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TBytesVidRender instance.
	 */
	bool flush();

	/**
	 * @brief Stop the pipeline. Will set pipeline to NULL state and release all resources. 
	 * 
	 * @note NOT MT-SAFE! Strongly recommend to call this from the main thread 
	 * 		 or the thread that created the TBytesVidRender instance. This method 
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
	explicit TBytesVidRender(u64 _maxBufferBytes = 65'535);  // Default to 64 KiB

	/** 
	 * @brief create a shared pointer to TBytesVidRender instance.
	 *
	 * @throws std::runtime_error if the pipeline initialization failed.
	 */
	[[nodiscard("Should not ignored the created TBytesVidRender::SharedPtr")]] static SharedPtr
	create(u64 _maxBufferBytes = 65'535)
	{
		return std::make_shared<TBytesVidRender>(_maxBufferBytes);
	}

	/**
	 * @brief Initialize the GStreamer context. Must be called before creating any TBytesVidRender instance.
	 * @param argc Pointer to the argc parameter from the main function.
	 * @param argv Pointer to the argv parameter from the main function.
	 */
	static void initContext(int* argc, char** argv[]);

	~TBytesVidRender();

  public:
	TBytesVidRender(const TBytesVidRender&)            = delete;  // Forbid copy or move
	TBytesVidRender& operator=(const TBytesVidRender&) = delete;
	TBytesVidRender(TBytesVidRender&&)                 = delete;
	TBytesVidRender& operator=(TBytesVidRender&&)      = delete;
};
}  // namespace gentau