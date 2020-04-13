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

	std::pair<int,int> get_16_multiple(int const width, int const height) {
		int const width_dif = width % 16;
		int const height_diff = height % 16;
		return std::make_pair(width - width_dif, height - height_diff);
	}

	grt::rgb_frame convert_to_rgb_frame(const webrtc::VideoFrame& frame) {
		const auto w_h = get_16_multiple(frame.width(), frame.height());
		const auto width = w_h.first;
		const auto height = w_h.second;
		
		rtc::scoped_refptr<webrtc::I420BufferInterface> yuv = frame.video_frame_buffer()->GetI420();
		std::vector<uint8_t> data(width*height * 4, 0);
		assert(data.size() == width * height * 4);
		const auto r3= libyuv::YUVToARGBScaleClip(yuv->DataY(),
								yuv->StrideY(),
								yuv->DataU(),
								yuv->StrideU(),
								yuv->DataV(),
								yuv->StrideV(),
								libyuv::FourCC::FOURCC_I420,
								frame.width(),
								frame.height(),
								&data[0],
								width*4,
								libyuv::FourCC::FOURCC_ARGB,
								width,
								height,
								0,
								0,
								width,
								height,
								libyuv::FilterModeEnum::kFilterNone);
		assert(r3 == 0);
		/*
		I am keeping below code for reference.
		as still there is bug for resolution,
		rendering is not right for
		original resolution 1360, 768


		destination resolution 1366, 768

		*/
		//const auto r = libyuv::I420ToARGB(yuv->DataY(), yuv->StrideY(), yuv->DataU(), yuv->StrideU(),
		//	yuv->DataV(), yuv->StrideV(), &data[0], width *4, width, height);
		//assert(r == 0);
		//const auto new_dim = get_16_multiple(width, height);
		//if (new_dim.first != width || new_dim.second != height) {

		//	const auto new_width = new_dim.first;
		//	const auto new_height = new_dim.second;

		//	std::vector<uint8_t> dest_(new_width * new_height * 4, 0); 
		//	//todo: it can be optimized also, new buffer requirement should not be there..
		//	const auto r2 = libyuv::ARGBScale(&data[0],
		//		width * 4,
		//		width,
		//		height,
		//		&dest_[0],
		//		new_width * 4,
		//		new_width,
		//		new_height,
		//		libyuv::FilterModeEnum::kFilterNone);
		//	assert(r2 == 0);
		//	return grt::rgb_frame{ dest_ , new_width, new_height };
		//}
		//
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