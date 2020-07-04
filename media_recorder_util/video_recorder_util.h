#ifndef _VIDEO_RECORDER_UTIL_H__
#define _VIDEO_RECORDER_UTIL_H__
#include <memory>
#include "json_parser.h"
namespace grt {
	class sender;
	class video_track_receiver;
}//namespace grt

namespace util {
	void set_video_recorder(grt::video_track_receiver*, std::shared_ptr<grt::sender> sender, grt::window_info info);
}//namespace util

#endif//_VIDEO_RECORDER_UTIL_H__