#ifndef MEDIA_RECEIVER_H__
#define MEDIA_RECEIVER_H__
#include <string>
#include "api/media_stream_interface.h"
#include "rendering_server_client/rendering_server_client.h"
#include "video_receiver/video_track_receiver.h"
#include <map>
#include <mutex>

namespace grt {

	class video_receiver {
	public:
		~video_receiver();
		void start( function_callback);
		void stop();
		void receive_track(std::string id, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);
		void remove_track(std::string id, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);
	private:
		std::shared_ptr<sender> sender_;
		std::map<std::string, std::unique_ptr< video_track_receiver> >track_table_;
		std::mutex table_lck_;
	};
}


#endif//MEDIA_RECEIVER_H__
