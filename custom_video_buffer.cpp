#include "custom_video_buffer.h"
#include <cassert>
#include "api/video/i420_buffer.h"
#include "rtc_base/ref_counted_object.h"

namespace grt {

		custom_video_buffer::custom_video_buffer(const char* ptr, int size, int w, int h) 
			:width_{ w }, height_{ h }, data_(ptr, &ptr[size]){
			assert(data_.size() == size);
		}

		
		rtc::scoped_refptr<webrtc::I420BufferInterface> custom_video_buffer::ToI420() {
			rtc::scoped_refptr<webrtc::I420Buffer> buffer{ nullptr };
			return buffer;
		}

		rtc::scoped_refptr< webrtc::VideoFrameBuffer>
			get_custom_video_frame(const char* ptr, int size, int w, int h) {

			return new rtc::RefCountedObject<custom_video_buffer>(ptr, size, w, h);
		}

		const std::vector<char>& 
			get_underlaying_rgb_buffer(rtc::scoped_refptr <webrtc::VideoFrameBuffer> buffer) {
			assert(buffer->type() == webrtc::VideoFrameBuffer::Type::kNative);
			const auto* ptr = dynamic_cast<custom_video_buffer*>(buffer.get());
			assert(ptr);
			return ptr->get_data();
		}


}//namespace grt