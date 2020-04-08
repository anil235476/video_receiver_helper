#ifndef _CUSTOM_VIDEO_FRAME_BUFFER_H__
#define _CUSTOM_VIDEO_FRAME_BUFFER_H__
#include <vector>
#include "api/video/video_frame_buffer.h"

namespace grt {

	class custom_video_buffer : public webrtc::VideoFrameBuffer {

	private:
		int width_;
		int height_;
		std::vector<char> data_;

	public:
		custom_video_buffer(const char* ptr, int size, int w, int h);

		std::vector<char> const& get_data() const { return data_; }

		Type type() const override { return Type::kNative; }

		// The resolution of the frame in pixels. For formats where some planes are
		// subsampled, this is the highest-resolution plane.
		int width() const override { return width_; }
		int height() const override { return height_; }

		// Returns a memory-backed frame buffer in I420 format. If the pixel data is
		// in another format, a conversion will take place. All implementations must
		// provide a fallback to I420 for compatibility with e.g. the internal WebRTC
		// software encoders.
		rtc::scoped_refptr<webrtc::I420BufferInterface> ToI420();
	};

	rtc::scoped_refptr< webrtc::VideoFrameBuffer>
		get_custom_video_frame(const char* ptr, int size, int w, int h); 

	//heper functions

	const std::vector<char>& get_underlaying_rgb_buffer(rtc::scoped_refptr <webrtc::VideoFrameBuffer> buffer);

}//namespace grt

#endif//_CUSTOM_VIDEO_FRAME_BUFFER_H__