cmake_minimum_required(VERSION 3.5.0)

project(notes_app)

find_package(PkgConfig REQUIRED)
 pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

 link_directories(
     ${GTKMM_LIBRARY_DIRS}  )
include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

    add_executable(notes_app main.cpp Notes.cpp Window.cpp)
   target_link_libraries(notes_app ${GTKMM_LIBRARIES})