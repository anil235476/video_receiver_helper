#ifndef _VIDEO_RECEIVER_HELPER_H___
#define _VIDEO_RECEIVER_HELPER_H___
#include <memory>
#include <string>
#include "video_receiver/video_track_receiver.h"
#include "rendering_server_client/rendering_server_client.h"


namespace webrtc {
	class MediaStreamTrackInterface;
}

namespace grt {
	//class video_track_receiver;
	//class sender;

	std::unique_ptr< video_track_receiver> 
		set_video_renderer(webrtc::MediaStreamTrackInterface*, std::shared_ptr<grt::sender> sender, window_info info);

#if 0 

	std::unique_ptr< video_track_receiver>
		set_video_recorder(webrtc::MediaStreamTrackInterface*, std::shared_ptr<grt::sender> sender, window_info info);
#endif
	void reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id);
	void async_reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id);

}//namespace grt

#endif//_VIDEO_RECEIVER_HELPER_H___