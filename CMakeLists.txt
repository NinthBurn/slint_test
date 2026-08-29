cmake_minimum_required(VERSION 3.21)
project(my_application LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Slint QUIET)
if (NOT Slint_FOUND)
  message("Slint could not be located in the CMake module search path. Downloading it from Git and building it locally")
  include(FetchContent)
  FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    # `release/1` will auto-upgrade to the latest Slint >= 1.0.0 and < 2.0.0
    # `release/1.0` will auto-upgrade to the latest Slint >= 1.0.0 and < 1.1.0
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
  )
  FetchContent_MakeAvailable(Slint)
endif (NOT Slint_FOUND)

add_executable(my_application src/main.cpp)

target_sources(my_application PRIVATE
    src/ui_bindings.cpp

    src/explorer/explorer.cpp
    src/explorer/directory_tab.cpp
    src/explorer/open_file.cpp
)

target_include_directories(my_application PRIVATE src/explorer)

target_link_libraries(my_application PRIVATE Slint::Slint)
slint_target_sources(my_application
	ui/app-window.slint
	LIBRARY_PATHS
		sleek-ui=${CMAKE_CURRENT_SOURCE_DIR}/ui/sleek-ui
)

if (WIN32)
    add_custom_command(TARGET my_application POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:my_application> $<TARGET_FILE_DIR:my_application> COMMAND_EXPAND_LISTS)
endif()
