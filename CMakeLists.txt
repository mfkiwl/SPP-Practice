cmake_minimum_required(VERSION 3.0)
project(SPP_PRACTICE)

# set build flags. 
set(CMAKE_CXX_FLAGS "-std=c++11" )
set(CMAKE_CXX_FLAGS "-fpermissive")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")
endif()

# set output dir
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/app/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/app/lib)

# RTKLIB
add_definitions(-DENAGLO -DENACMP -DENAGAL -DNFREQ=3 -DNEXOBS=3 -DDLL)
add_subdirectory(3rd/rtklib)


# Add third-party libraries
# Eigen
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIRS})
# Yaml-cpp 
find_package(yaml-cpp REQUIRED)
# Glog
find_package(glog REQUIRED)


# source
aux_source_directory(src/utility DIR_utility)
aux_source_directory(src/gnss    DIR_gnss)
list(APPEND DIR_ALL ${DIR_utility}
                    ${DIR_gnss})

add_library(${PROJECT_NAME} SHARED ${DIR_ALL})
target_link_libraries(${PROJECT_NAME} rtklib  glog yaml-cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)



# executable
add_executable(SPP app/SPP_main.cc)
target_link_libraries(SPP ${PROJECT_NAME})
