cmake_minimum_required(VERSION 3.7)

#set(LIBWEBRTC_INCLUDE_PATH "D:/webrtcBuild/Sept/webrtc_checkout/src")
#set(ABSL_PATH "${LIBWEBRTC_INCLUDE_PATH}/third_party/abseil-cpp")

add_definitions(/ZI)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_library(media_render_util
	video_render_util.h
	video_render_util.cpp
	)

target_link_libraries(media_render_util PRIVATE 
	    json_parser
		video_render
		video_receiver
		rendering_server_client
		)
