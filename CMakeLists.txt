cmake_minimum_required(VERSION 3.7)

#set(BOOST_LIB "D:/boost_1_69_0/boost_1_69_0")

if(STAND_ALONE_BUILD)
set(SIGNALLER_LIB_PATH "D:/git_project/")
set(JSON_INCLUDE "D:/git_project/json_parser/include")
set(JSON_LIB_PATH "D:/git_project/json_parser/")
endif(STAND_ALONE_BUILD)


ADD_DEFINITIONS(-D_UNICODE)
add_definitions(/ZI)

#add_subdirectory(media_recorder_util)
add_subdirectory(media_render_util)
add_subdirectory(rendering_server_client)
add_subdirectory(video_receiver)
 #todo: this library should be added under a flag
 add_subdirectory(media_recorder_util)

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
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<BUILD_INTERFACE:${LIBWEBRTC_INCLUDE_PATH}>
PRIVATE
	)

target_link_libraries(video_receiver_helper 

PUBLIC 

   rendering_server_client
  
PRIVATE
    video_receiver
   media_render_util
   #media_recorder_util
   )
