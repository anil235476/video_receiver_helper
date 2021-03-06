#ifndef _VIDEO_FRAME_CALLBACK_H__
#define _VIDEO_FRAME_CALLBACK_H__
#include <cstdint>
#include <vector>

namespace grt {
	struct yuv_frame {
		yuv_frame(const uint8_t* y, const uint8_t* u, const uint8_t* v,
			int stride_y, int stride_u, int stride_v, int w, int h)
			:y_{ y }, u_{ u }, v_{ v }, stride_y_{ stride_y }, stride_u_{ stride_u }
			, stride_v_{ stride_v }, w_{ w }, h_{ h }{}

		const uint8_t* y_;
		const uint8_t* u_;
		const uint8_t* v_;
		const int stride_y_;
		const int stride_u_;
		const int stride_v_;
		const int w_;
		const int h_;
	};

	struct rgb_frame {
		std::vector<uint8_t> data;
		const int w_{};
		const int h_{};
			rgb_frame(std::vector<uint8_t> rgb, int w, int h) 
			:data{ rgb }, w_{ w }, h_{ h }{}
	};

	class video_frame_callback {
	public:
		virtual ~video_frame_callback() {}
		virtual void on_frame(yuv_frame) = 0;
		virtual void on_frame(rgb_frame) = 0;
	};
} //namespace grt



#endif//_VIDEO_FRAME_CALLBACK_H__
