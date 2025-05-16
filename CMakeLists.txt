cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  imgborders
  DESCRIPTION ${DESCRIPTION}
  VERSION ${VERSION}
)

set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(DESCRIPTION="${DESCRIPTION}")
add_compile_definitions(VERSION="${VERSION}")

find_package(PkgConfig REQUIRED)

include_directories(.)
file(GLOB_RECURSE SRCFILES "*.cpp")
add_library(imgborders SHARED ${SRCFILES})

pkg_check_modules(
  deps
  REQUIRED
  IMPORTED_TARGET
  hyprland
  pixman-1
  libdrm
  pangocairo
)
target_link_libraries(imgborders PkgConfig::deps)

install(TARGETS imgborders)
