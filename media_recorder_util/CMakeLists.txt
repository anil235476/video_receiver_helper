cmake_minimum_required(VERSION 3.7)

#set(LIBWEBRTC_INCLUDE_PATH "D:/webrtcBuild/Sept/webrtc_checkout/src")
#set(ABSL_PATH "${LIBWEBRTC_INCLUDE_PATH}/third_party/abseil-cpp")

add_definitions(/ZI)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_library(media_recorder_util
	video_recorder_util.h
	video_recorder_util.cpp
	)

	
target_include_directories(media_recorder_util PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)


target_link_libraries(media_recorder_util PRIVATE 
	    json_parser
		video_receiver
		rendering_server_client
		video_recorder
		)

