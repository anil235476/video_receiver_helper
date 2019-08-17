cmake_minimum_required(VERSION 3.7)

#set(BOOST_LIB "D:/boost_1_69_0/boost_1_69_0")

set(LIBWEBRTC_INCLUDE_PATH "D:/webrtcBuild/Sept/webrtc_checkout/src")
set(ABSL_PATH "${LIBWEBRTC_INCLUDE_PATH}/third_party/abseil-cpp")


ADD_DEFINITIONS(-D_UNICODE)
add_definitions(/ZI)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_library(video_receiver_helper
   video_receiver_helper.h 
   video_receiver_helper.cpp
)

target_include_directories(video_receiver_helper PUBLIC 
	$<BUILD_INTERFACE:${ABSL_PATH}>
	$<BUILD_INTERFACE:${LIBWEBRTC_INCLUDE_PATH}>
	)

add_subdirectory(media_render_util)
add_subdirectory(rendering_server_client)
add_subdirectory(video_receiver)