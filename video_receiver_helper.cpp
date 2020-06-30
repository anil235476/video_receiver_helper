#include "video_receiver_helper.h"
#include "video_receiver/video_track_receiver_impl.h"
#include "media_render_util/video_render_util.h"

namespace grt {

	std::unique_ptr< video_track_receiver>
		set_video_renderer(webrtc::MediaStreamTrackInterface* stream, std::shared_ptr<grt::sender> sender,window_info info) {
		auto track_receiver = get_receiver(stream);
		//narendra i think this function has to be replaced.
		//i mean in case of recorder 
		// we may like to call a function
		//util::async_set_video_recorder
		util::async_set_video_renderer(track_receiver.get(), sender, info);
		return track_receiver;
	}

	void 
		async_reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
		util::async_reset_video_renderer(sender, id);
	}
	
	void reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
		util::reset_video_renderer(sender, id);
	}

#if 0 //this function may not be required please see above comments.

	std::unique_ptr< video_track_receiver>
		set_video_recorder(webrtc::MediaStreamTrackInterface* stream, std::shared_ptr<grt::sender> sender, window_info info) {
		auto track_receiver = get_receiver(stream);
		util::async_set_video_renderer(track_receiver.get(), sender, info);
		return track_receiver;
	}
#endif
}//namespace grt