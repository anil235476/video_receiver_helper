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
	void show_conference_layout(std::shared_ptr<grt::sender> sender) noexcept;
	void show_self_view_layout(std::shared_ptr<grt::sender> sender) noexcept;
	void show_ui_layout(std::shared_ptr<grt::sender> sender) noexcept;
}



#endif//_VIDEO_RENDER_UTIL_H__