#include "video_render_util.h"
#include <cassert>
#include "renderer.h"
#include <thread>
#include "rendering_server_client.h"
#include "video_track_receiver.h"
#include <Windows.h>

namespace detail {

	class video_receiver : public grt::video_frame_callback {
	private:
		std::shared_ptr<grt::renderer> renderer_;
		HWND hwnd_;
		const std::string id_;
		std::shared_ptr<grt::sender> sender_;

	public:
		video_receiver(HWND hwnd, std::unique_ptr< grt::renderer>&& render, std::string id, std::shared_ptr<grt::sender> sender)
			:renderer_{ std::move(render) }, hwnd_{ hwnd }, id_{ id }, sender_{ sender }{
			auto r = SetWindowPos(hwnd_, HWND_TOPMOST,
				0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
			assert(r != 0);
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
			renderer_->render_frame(hwnd_, frame_info);
			grt::clean(frame_info);
		}

		void on_frame(grt::rgb_frame frame) override {
			auto frame_info = grt::make_frame_info(frame.data.data(), frame.w_, frame.h_);
			renderer_->render_frame(hwnd_, frame_info);
			grt::clean(frame_info);
		}
	};


	std::unique_ptr< grt::video_frame_callback>
		get_frame_receiver(HWND hwnd, std::unique_ptr< grt::renderer>&& render, std::string id, std::shared_ptr<grt::sender> sender) {
		return std::make_unique< video_receiver>(hwnd, std::move(render), id, sender);
	}
}

namespace util {


	struct result {
		const std::wstring name_;
		HWND wnd = nullptr;
		result(const wchar_t* child_wnd_name) :name_{ child_wnd_name } {

		}
	};

	BOOL CALLBACK EnumChildProc(
		_In_ HWND   hwnd,
		_In_ LPARAM lParam
	) {
		result* ptr = reinterpret_cast<result*>(lParam);
		assert(ptr);
		wchar_t buffer[100];
		const auto l = GetWindowText(hwnd, buffer, sizeof(buffer));
		if (l > 0) {
			if (ptr->name_ == std::wstring{ buffer, size_t(l) }) {
				ptr->wnd = hwnd;
				return false;
			}
		}
		return true;
	}

	HWND find_child_window(const wchar_t* className, const wchar_t* parent_wnd_name,
		const wchar_t* child_wnd_name) {

		auto parentWnd = FindWindow(className, parent_wnd_name);
		assert(parentWnd);
		result ret{ child_wnd_name };
		EnumChildWindows(parentWnd, EnumChildProc, (LPARAM)&ret);
		assert(ret.wnd);
		return ret.wnd;

		////auto parentWnd = FindWindow(className, parent_wnd_name);
		//auto parentWnd = FindWindowEx(grandparent, nullptr, className, parent_wnd_name);
		//assert(parentWnd);
		//if (parentWnd == nullptr) return nullptr;
		//auto wnd = FindWindowEx(parentWnd, nullptr, className, child_wnd_name);
		//assert(wnd);
		//return wnd;
	}

	std::wstring to_wstring(std::string v) {
		return std::wstring(v.begin(), v.end());
	}

	HWND find_child_window(std::string className, std::string parent_wnd, std::string child_name) {
		return find_child_window(to_wstring(className).c_str(),
			to_wstring(parent_wnd).c_str(), to_wstring(child_name).c_str());
	}

	bool set_video_renderer(grt::video_track_receiver* receiver, std::string class_name, 
		std::string parent_name, std::string  renderer_id, std::shared_ptr<grt::sender> sender, std::string id
	,std::string usr_name) {
		//todo: create connection with display manager and ask for creating a window.
		assert(receiver);

		auto hwnd = find_child_window(class_name, parent_name, renderer_id);
		
		assert(hwnd);//rendering application with window should be running
		if (hwnd == nullptr) return false;
		auto renderer = grt::get_renderer();
		renderer->render_name(hwnd, usr_name);
		auto frame_receiver = detail::get_frame_receiver(hwnd, std::move(renderer), id, sender);
		receiver->register_callback(std::move(frame_receiver));
		return true;
	}

	void async_set_video_renderer(grt::video_track_receiver* recevier, std::shared_ptr<grt::sender> sender, grt::window_info info) {
		const auto m = grt::make_render_wnd_req(info);
		sender->send_to_renderer(info.id_, m, [recevier, sender, info](grt::message_type type, absl::any msg, auto) {
			assert(type == grt::message_type::window_create_res);
			auto wndInfo = absl::any_cast<grt::wnd_create_res>(msg);
			assert(wndInfo.status_);
			const auto className = wndInfo.class_name_;
			const auto wndName = wndInfo.parent_wnd_name_;
			const auto wndId = wndInfo.child_wnd_id_;
		
			set_video_renderer(recevier, className, wndName, wndId, sender, info.id_, info.name_);
			sender->done(info.id_);
		});
	}

	void reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
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

	void async_reset_video_renderer(std::shared_ptr<grt::sender> sender, std::string const& id) {
		const auto m = grt::make_render_wnd_close_req(id);
		sender->send_to_renderer(id, m, [id, sender](auto type, auto msg, auto) {
			assert(type == grt::message_type::wnd_close_req_res);
			sender->done(id); 
		});
	}

#if 0
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
#endif

	void post_message_to_renderer(grt::message_type type,std::shared_ptr<grt::sender> sender ) noexcept {
		assert(sender);
		
		//todo : if required to send any argument we needs to handle here 
		// It current design we don't need it now 
		if(type == grt::message_type::show_conference_layout) {
			const auto m = grt::make_conference_view_layout();
			sender->post_to_renderer(m);
		}
		else if (type == grt::message_type::show_self_view_layout) {
			const auto m = grt::make_self_view_layout();
			sender->post_to_renderer(m);			
		}
		else if(type == grt::message_type::show_ui_layout) {
			const auto m = grt::make_ui_view_layout();
			sender->post_to_renderer(m);			
		}
		else {
			assert(false);
		}

	}

	void send_message_to_renderer(grt::message_type type, std::shared_ptr<grt::sender> sender ) noexcept {
		
		assert(sender);
		post_message_to_renderer(type, sender);

		//todo: we will add case as per the use case .
		// this function send message to render and wait for render response 
		// below is sample code that can be use for future use as reference type 
#if 0
		if (type == grt::message_type::show_conference_layout) {

			const auto m = grt::make_conference_view_layout();
			sender->send_to_renderer("confrence_view", m, [sender](auto type, auto, auto) {
				assert(type == grt::message_type::msg_received_ack);
				sender->done("confrence_view");				
			});				
		}
#endif
	}

	
}//namespace util