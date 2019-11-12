#include "media_receiver.h"
#include "media_render_util/video_render_util.h"
#include "video_receiver_helper.h"
#include "rendering_server_client/rendering_server_client.h"
#include "video_receiver/video_track_receiver.h"

namespace grt {

	void video_receiver::start( function_callback callback) {
		assert(sender_.get() == nullptr);
		sender_ = std::make_shared<sender>();

		auto future_ = sender_->sync_connect(RENDERING_SERVER_IP, RENDERING_SERVER_PORT);
		const auto connection_status = future_.get();
		assert(connection_status);
		util::show_rendering_window(sender_);
		sender_->register_for_session_leave_msg(callback);
		sender_->register_for_message(mic_toggle_id, callback);
		sender_->register_for_message(cam_toggle_id, callback);
	}
	
	void video_receiver::stop() {
		video_track_receiver_.reset();
		util::hide_rendering_window(sender_);
		sender_.reset();//destroyp object
	}

	video_receiver::~video_receiver() = default;

	
	void video_receiver::receive_track(std::string id, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) {
		assert(track.get() != nullptr);
		assert(track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind);
		assert(video_track_receiver_.get() == nullptr);
		assert(sender_.get() != nullptr);
		video_track_receiver_ = std::move(
									set_video_renderer(
									static_cast<webrtc::VideoTrackInterface*>(track.release()),
									sender_, id)
								);
	}

	void  video_receiver::remove_track(std::string id, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) {
		assert(video_track_receiver_.get());
		video_track_receiver_.reset();
	}

	

}//namespace grt