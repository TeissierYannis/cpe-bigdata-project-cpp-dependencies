cmake_minimum_required(VERSION 3.18)
project(exifextract)

set(CMAKE_CXX_STANDARD 17)
#find_library(LIBEXIF_LIBRARY exif)
#find_package(CURL REQUIRED)

# Add the executable and link against libexif
add_executable(exifextract main.cpp include/easyexif/exif.cpp)
#add_executable(download download.cpp)
#target_link_libraries(download ${CURL_LIBRARIES})
target_link_libraries(exifextract ${LIBEXIF_LIBRARY})
