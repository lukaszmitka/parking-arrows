cmake_policy(SET CMP0048 NEW)
set(VERSION "0.0.1")
project(parking_arrows
VERSION ${VERSION})

cmake_minimum_required(VERSION 3.16)

add_compile_options(-std=c++17)

option(BUILD_TEST "BUILD_TEST" OFF)

find_package(PkgConfig REQUIRED)

find_path(OpenCV_DIR
    NAMES OpenCVConfig.cmake
    HINTS ${CMAKE_SOURCE_DIR}/../libraries/opencv/
    /storage/projects/alpr/libraries/opencv/
)

# Opencv Package
FIND_PACKAGE( OpenCV REQUIRED )

IF (${OpenCV_VERSION} VERSION_LESS 2.4.7)
	MESSAGE(FATAL_ERROR "OpenCV version is not compatible : ${OpenCV_VERSION}")
ENDIF()

include_directories(${OpenCV_INCLUDE_DIRS})

add_definitions( -DOPENCV_MAJOR_VERSION=${OpenCV_VERSION_MAJOR})

IF(BUILD_TEST)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
  )

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
ENDIF()

include_directories(
  ${CMAKE_SOURCE_DIR} 
  include
)

set(LIB_SOURCES
  src/Config.cpp
  src/Camera.cpp
  src/LicensePlateRecognizer.cpp
)

set(EXE_SOURCES
  src/main.cpp
)

IF(BUILD_TEST)
  set(TEST_SOURCES 
      test/test.cpp
  )
ENDIF()

set(LIBARIES
  ${OpenCV_LIBS}
  openalpr
)

add_library(camera_detector_lib STATIC ${LIB_SOURCES})

add_executable(camera_detector ${EXE_SOURCES})

if(BUILD_TEST)
  add_executable(camera_detector_test ${TEST_SOURCES})
ENDIF()

target_link_libraries(camera_detector_lib
  ${LIBARIES}
)

target_link_libraries(camera_detector
  camera_detector_lib
)

IF(BUILD_TEST)
  target_link_libraries(camera_detector_test PUBLIC 
    ${LIBARIES}
    camera_detector_lib
    gtest_main
  )

  include(GoogleTest)
  gtest_discover_tests(camera_detector_test PROPERTIES TIMEOUT 800)
ENDIF()