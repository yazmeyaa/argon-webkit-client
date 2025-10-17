cmake_minimum_required(VERSION 3.10)
project(argon_webkit_client C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

# GTK3 + WebKit2GTK
pkg_check_modules(GTK REQUIRED gtk+-3.0 webkit2gtk-4.1 javascriptcoregtk-4.0 gio-2.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

add_executable(argon_webkit_client src/main.c)

target_link_libraries(argon_webkit_client ${GTK_LIBRARIES})
