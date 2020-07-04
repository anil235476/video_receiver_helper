#include "video_recorder_util.h"
#include <video_frame_callback.h>
#include "recorder.h"
#include "video_track_receiver.h"
namespace detail {

	class video_receiver : public grt::video_frame_callback {
	private:
		
		const std::string id_;
		std::shared_ptr<grt::sender> sender_;
		grt::recorder recorder_;

	public:
		video_receiver(std::string id, std::shared_ptr<grt::sender> sender, grt::recorder recorder)
			:id_{ id }, sender_{ sender }, recorder_{ std::move(recorder) }{
			
			assert(sender_);
			assert(!id_.empty());
		}

		void on_frame(grt::yuv_frame frame) override {
			assert(false);
		/*	auto frame_info = grt::make_frame_info(
				frame.y_, frame.u_, frame.v_, frame.stride_y_,
				frame.stride_u_, frame.stride_v_, frame.w_, frame.h_);
			renderer_->render_frame(hwnd_, frame_info);
			grt::clean(frame_info);*/
		}

		void on_frame(grt::rgb_frame frame) override {
			auto frame_info = grt::make_frame_info(frame.data.data(), frame.w_, frame.h_);
			//	auto recorder_frame_info = grt::recorder_make_frame_info(frame.data.data(), frame.w_, frame.h_);
			recorder_.save_frame(frame_info);
			grt::clean(frame_info);
		}
	};


	std::unique_ptr< grt::video_frame_callback>
		get_frame_receiver( std::string id,
			std::shared_ptr<grt::sender> sender, grt::recorder&& recorder) {
		return std::make_unique< video_receiver>(id, sender, std::move(recorder));
	}
}

namespace util {

	void set_video_recorder(grt::video_track_receiver* recevier, std::shared_ptr<grt::sender> sender, grt::window_info info) {
		auto frame_receiver = detail::get_frame_receiver(info.id_, sender,
			grt::recorder{ info.name_ });
		recevier->register_callback(std::move(frame_receiver));
	}




}//namespace util