#include "video_receiver_helper.h"
#include "video_receiver/video_track_receiver_impl.h"
#ifdef ENABLE_RECORDER
#include "media_recorder_util/video_recorder_util.h"
#else
#include "media_render_util/video_render_util.h"
#endif
namespace grt {

	std::unique_ptr< video_track_receiver>
		set_video_renderer(webrtc::MediaStreamTrackInterface* stream, std::shared_ptr<grt::sender> sender,window_info info) {
		auto track_receiver = get_receiver(stream);
#ifdef ENABLE_RECORDER
		util::async_set_video_recorder(track_receiver.get(), sender, info);
#else
		util::async_set_video_renderer(track_receiver.get(), sender, info);
#endif
		return track_receiver;
	}

	void 
		async_reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
#ifdef ENABLE_RECORDER
		util::async_reset_video_recorder(sender, id);
#else
		util::async_reset_video_renderer(sender, id);
#endif
	}
	
	void reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
#ifdef ENABLE_RECORDER
		util::reset_video_recorder(sender, id);
#else
		util::reset_video_renderer(sender, id);
#endif
	}

}//namespace grt