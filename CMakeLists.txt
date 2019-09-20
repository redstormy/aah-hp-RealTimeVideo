cmake_minimum_required(VERSION 2.8)
project( aah_test )

ADD_DEFINITIONS(
    -std=c++11
    -D EXECU
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( NAME_SRC
    src/aah_test.cpp
    src/RealTimeVideo.cpp
    src/RealTimeVideo.h
)


add_executable( aah_test ${NAME_SRC})
target_link_libraries( aah_test ${OpenCV_LIBS} )
