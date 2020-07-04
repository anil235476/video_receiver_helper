#include "video_recorder_util.h"
#include <cassert>
#include "recorder.h"
#include <frame_info.h>
#include <thread>
#include "rendering_server_client.h"
#include "video_track_receiver.h"
#include <Windows.h>

namespace detail {

	class video_receiver : public grt::video_frame_callback {
	private:
		const std::string id_;
		std::shared_ptr<grt::sender> sender_;
		grt::recorder recorder_;
	public:
		video_receiver(std::string id, std::shared_ptr<grt::sender> sender, grt::recorder recorder)
			: id_{ id }, sender_{ sender }, recorder_{ std::move(recorder) }{
			assert(sender_);
			assert(!id_.empty());
		}
		~video_receiver() override {
			//todo: this is wrong design.
			//should be improved
			//const auto m = grt::make_render_wnd_close_req(id_);
		
			//std::promise<void> trigger;
			//auto future = trigger.get_future();
			//sender_->send_to_renderer(id_, m, [&trigger](auto type, auto msg, auto) mutable {
			//	trigger.set_value();
			//});

			//const auto status = future.wait_for(std::chrono::seconds(5));//wait for message to deliever.
			//assert(status == std::future_status::ready);//todo: log status.
			//sender_->done(id_);
		}

		void on_frame(grt::yuv_frame frame) override {
			auto frame_info = grt::make_frame_info(
				frame.y_, frame.u_, frame.v_, frame.stride_y_,
				frame.stride_u_, frame.stride_v_, frame.w_, frame.h_);
			//renderer_->render_frame(hwnd_, frame_info);
			grt::clean(frame_info);
		}

		void on_frame(grt::rgb_frame frame) override {
			auto frame_info = grt::make_frame_info(frame.data.data(), frame.w_, frame.h_);
			recorder_.save_frame(frame_info);
			grt::clean(frame_info);
		}
	};

	std::unique_ptr< grt::video_frame_callback>
		get_frame_receiver(std::string id,std::shared_ptr<grt::sender> sender, grt::recorder&& recorder) {
		return std::make_unique< video_receiver>(id, sender, std::move(recorder));
	}
}

namespace util {
	bool set_video_recorder(grt::video_track_receiver* receiver, std::shared_ptr<grt::sender> sender, 
		std::string id ,std::string usr_name) {
		//todo: create connection with display manager and ask for creating a window.
		assert(receiver);

		auto frame_receiver = detail::get_frame_receiver(id, sender,grt::recorder{ usr_name });
		receiver->register_callback(std::move(frame_receiver));
		
		return true;
	}

	void async_set_video_recorder(grt::video_track_receiver* recevier, std::shared_ptr<grt::sender> sender, grt::window_info info) {
		const auto m = grt::make_render_wnd_req(info);
		sender->send_to_renderer(info.id_, m, [recevier, sender, info](grt::message_type type, absl::any msg, auto) {
					
			set_video_recorder(recevier, sender, info.id_, info.name_);
			sender->done(info.id_);
		});
	}

	void reset_video_recorder(std::shared_ptr<grt::sender> sender, std::string const& id) {
		const auto m = grt::make_render_wnd_close_req(id);
		std::promise<void> trigger;
		auto future = trigger.get_future();
		sender->send_to_renderer(id, m, [id, sender, &trigger](auto type, auto msg, auto) {
			assert(type == grt::message_type::wnd_close_req_res);
			trigger.set_value();
			sender->done(id);
			
		});
		const auto status = future.wait_for(std::chrono::seconds(5));//wait for message to deliever.
		assert(status == std::future_status::ready);//todo: log status.
	}

	void async_reset_video_recorder(std::shared_ptr<grt::sender> sender, std::string const& id) {
		const auto m = grt::make_render_wnd_close_req(id);
		sender->send_to_renderer(id, m, [id, sender](auto type, auto msg, auto) {
			assert(type == grt::message_type::wnd_close_req_res);
			sender->done(id); 
		});
	}

	void show_conference_layout(std::shared_ptr<grt::sender> sender) noexcept {
		assert(sender);
		const auto m = grt::make_conference_view_layout();
		sender->send_to_renderer("confrence_view", m, [](auto, auto, auto) {});
		sender->done("confrence_view");
		std::this_thread::sleep_for(std::chrono::seconds(2));//wait for message for sent. todo: fix this dependency in sender itself.
	}
	void show_self_view_layout(std::shared_ptr<grt::sender> sender) noexcept {
		assert(sender);
		const auto m = grt::make_self_view_layout();
		sender->send_to_renderer("selfview_layout", m, [](auto, auto, auto) {});
		sender->done("selfview_layout");
		std::this_thread::sleep_for(std::chrono::seconds(2));//wait for message for sent. todo: fix this dependency
	}
	void show_ui_layout(std::shared_ptr<grt::sender> sender) noexcept {
		assert(sender);
		const auto m = grt::make_ui_view_layout();
		sender->send_to_renderer("ui_view_layout", m, [](auto, auto, auto) {});
		sender->done("ui_view_layout");
		std::this_thread::sleep_for(std::chrono::seconds(2));//wait for message for sent. todo: fix this dependency
	}

	void send_forward_message(std::shared_ptr<grt::sender> sender, std::string const& msg) {
		assert(sender);
		sender->send_to_renderer("forward", msg, [](auto, auto, auto) {});
		sender->done("forward");
	}

	void send_message(std::shared_ptr<grt::sender> sender,
		grt::message_type type, absl::optional<absl::any> msg) noexcept {
		switch (type) {
		case grt::message_type::show_self_view_layout:
			show_self_view_layout(sender);
			break;
		case grt::message_type::show_conference_layout:
			show_conference_layout(sender);
			break;
		case grt::message_type::show_ui_layout:
			show_ui_layout(sender);
			break;
		case grt::message_type::forward_message:
			assert(msg.has_value());
			send_forward_message(sender, absl::any_cast<std::string>(msg.value()));
			break;
		default:
			assert(false);
		}
	}
}//namespace util