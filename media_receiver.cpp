#include "media_receiver.h"
#include "video_receiver_helper.h"
#include "rendering_server_client/rendering_server_client.h"
#include "video_receiver/video_track_receiver.h"

namespace grt {
	using lock = std::lock_guard<std::mutex>;

	void video_receiver::set_sender(std::shared_ptr<sender> sender){
		sender_ = sender;
	}

	video_receiver::~video_receiver() = default;

	
	void video_receiver::receive_track(std::string id, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) {
		assert(track.get() != nullptr);
		assert(track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind);
		//assert(video_track_receiver_.get() == nullptr);
		assert(sender_.get() != nullptr);
		assert(track_table_.find(id) == track_table_.end());
		auto receiver = set_video_renderer(
			static_cast<webrtc::VideoTrackInterface*>(track.release()),
			sender_, id);
		{
			lock lck{ table_lck_ };
			track_table_.emplace(id, std::move(receiver));
		}
		
								
	}

	void  video_receiver::remove_track(std::string id, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) {
		assert(track_table_.find(id) != track_table_.end());
		lock lck{ table_lck_ };
		track_table_.erase(id);
	}


	std::shared_ptr<sender> get_rendering_wnd_sender(function_callback callback) {
		auto sender_ = get_rendering_server_client();// std::make_shared<sender>();

		auto future_ = sender_->sync_connect(RENDERING_SERVER_IP, RENDERING_SERVER_PORT);
		const auto connection_status = future_.get();
		assert(connection_status);
		//util::show_rendering_window(sender_);
		sender_->register_for_session_leave_msg(callback);
		sender_->register_for_message(mic_toggle_id, callback);
		sender_->register_for_message(cam_toggle_id, callback);
		sender_->register_for_message(error_id, callback);
		sender_->register_for_message(share_toggle_id, callback);
		return sender_;
	}

}//namespace grt