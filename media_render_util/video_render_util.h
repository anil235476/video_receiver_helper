#ifndef _VIDEO_RENDER_UTIL_H__
#define _VIDEO_RENDER_UTIL_H__
#include <string>
#include "json_parser.h"

namespace grt {
	class sender;
	class video_track_receiver;
}
namespace util {
	void async_set_video_renderer(grt::video_track_receiver*, std::shared_ptr<grt::sender> sender, grt::window_info info);
	void async_reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id);
	void reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id);
	void send_message_to_renderer(std::shared_ptr<grt::sender> sender, std::string const& id, std::string const& msg) noexcept;
}



#endif//_VIDEO_RENDER_UTIL_H__