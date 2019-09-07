#include "video_receiver_helper.h"
#include "video_receiver/video_track_receiver_impl.h"
#include "media_render_util/video_render_util.h"

namespace grt {

	std::unique_ptr< video_track_receiver>
		set_video_renderer(webrtc::MediaStreamTrackInterface* stream, std::shared_ptr<grt::sender> sender, std::string const& id) {
		auto track_receiver = get_receiver(stream);
		util::async_set_video_renderer(track_receiver.get(), sender, id);
		return track_receiver;
	}

	void 
		async_reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
		util::async_reset_video_renderer(sender, id);
	}

}//namespace grt