cmake_minimum_required(VERSION 3.7)

#set(BOOST_LIB "D:/boost_1_69_0/boost_1_69_0")

if(STAND_ALONE_BUILD)
set(SIGNALLER_LIB_PATH "D:/git_project/")
set(JSON_INCLUDE "D:/git_project/json_parser/include")
set(JSON_LIB_PATH "D:/git_project/json_parser/")
endif(STAND_ALONE_BUILD)

#set(LIBWEBRTC_INCLUDE_PATH "D:/webrtcBuild/Sept/webrtc_checkout/src")
#set(ABSL_PATH "${LIBWEBRTC_INCLUDE_PATH}/third_party/abseil-cpp")


ADD_DEFINITIONS(-D_UNICODE)
add_definitions(/ZI)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_library(video_receiver_helper
   video_receiver_helper.h 
   video_receiver_helper.cpp
   media_receiver.h
   media_receiver.cpp
)

target_include_directories(video_receiver_helper PUBLIC 
	$<BUILD_INTERFACE:${ABSL_PATH}>
	$<BUILD_INTERFACE:${LIBWEBRTC_INCLUDE_PATH}>
	$<BUILD_INTERFACE:${SIGNALLER_LIB_PATH}>
	$<BUILD_INTERFACE:${JSON_INCLUDE}>
	$<BUILD_INTERFACE:${JSON_LIB_PATH}>
	)

target_link_libraries(video_receiver_helper PUBLIC 
   json_parser)

add_subdirectory(media_render_util)
add_subdirectory(rendering_server_client)
add_subdirectory(video_receiver)