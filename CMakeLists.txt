cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (boost_examples)

find_package(Boost
    #1.73 EXACT      # Minimum or EXACT version e.g. 1.67.0
  1.65      # Minimum or EXACT version e.g. 1.67.0
  REQUIRED             # Fail with error if Boost is not found
  #COMPONENTS <libs>...] # Boost libraries by their canonical name
                         # e.g. "date_time" for "libboost_date_time"
  #[OPTIONAL_COMPONENTS <libs>...]
                         # Optional Boost libraries by their canonical name)
  )                      # e.g. "date_time" for "libboost_date_time"
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Fatal error: Boost (version >= 1.55) required.")
else()
  message(STATUS "Setting up BOOST")
  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
endif (NOT Boost_FOUND)

set(CMAKE_CXX_FLAGS "-std=c++2a")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory (interprocess)
