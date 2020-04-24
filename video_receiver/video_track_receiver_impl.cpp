#include "video_track_receiver_impl.h"
#include "third_party/libyuv/include/libyuv.h"

namespace util {
	grt::yuv_frame convert_to_yuv_frame(const webrtc::VideoFrame& frame) {
		const auto width = frame.width();
		const auto height = frame.height();
		auto buffer = frame.video_frame_buffer();
		auto yuv = buffer->GetI420();
		const auto* y = yuv->DataY();
		const auto*u = yuv->DataU();
		const auto* v = yuv->DataV();
		const auto stridey = yuv->StrideY();
		const auto strideu = yuv->StrideU();
		const auto stridev = yuv->StrideV();
		return grt::yuv_frame{ y, u, v, stridey, strideu, stridev, width, height };
	}

	grt::rgb_frame convert_to_rgb_frame(const webrtc::VideoFrame& frame) {
		const auto width = frame.width(); 
		const auto height = frame.height();
		
		rtc::scoped_refptr<webrtc::I420BufferInterface> yuv = frame.video_frame_buffer()->GetI420();
		std::vector<uint8_t> data(width*height * 4, 0);
		assert(data.size() == width * height * 4);
	
		const auto r = libyuv::I420ToARGB(yuv->DataY(), yuv->StrideY(), yuv->DataU(), yuv->StrideU(),
			yuv->DataV(), yuv->StrideV(), &data[0], width *4, width, height);
	     assert(r == 0);
		
		return grt::rgb_frame{ data , width, height };
		
	}
}

namespace grt {

	video_track_receiver_impl::video_track_receiver_impl(webrtc::VideoTrackInterface* track_interface)
		:rendered_track_{ track_interface } {
		rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants{});
	}

	video_track_receiver_impl::~video_track_receiver_impl() {
		rendered_track_->RemoveSink(this);
	}

	void video_track_receiver_impl::register_callback(
		std::unique_ptr<video_frame_callback> callback) {
		callback_ = std::move(callback);
	}

	void video_track_receiver_impl::OnFrame(const webrtc::VideoFrame& frame) {
		if (callback_) {
			callback_->on_frame(util::convert_to_rgb_frame(frame));
		}
			
	}

	void video_track_receiver_impl::OnDiscardedFrame() {

	}

	std::unique_ptr< video_track_receiver>
		get_receiver(webrtc::MediaStreamTrackInterface* stream_track) {
		assert(stream_track);
		return std::make_unique<video_track_receiver_impl>(
			static_cast<webrtc::VideoTrackInterface*>(stream_track));
	}

}//namespace grt