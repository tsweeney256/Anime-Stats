#Causes some libraries to fail to be statically linked if requiring a lower version for some reason
cmake_minimum_required(VERSION 3.3)
project(AnimeStats)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -s")
set(CMAKE_CXX_FLAGS_STATIC_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${STATIC_ARGS}")
set(CMAKE_CXX_FLAGS_STATIC_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${STATIC_ARGS}")
set(STATIC_ARGS "-static-libstdc++ -static-libgcc -static")
set(CMAKE_EXE_LINKER_FLAGS_STATIC_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${STATIC_ARGS}")
set(CMAKE_EXE_LINKER_FLAGS_STATIC_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${STATIC_ARGS}")

if(CMAKE_BUILD_TYPE STREQUAL "Static_Release" OR CMAKE_BUILD_TYPE STREQUAL "Static_Debug")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif(CMAKE_BUILD_TYPE STREQUAL "Static_Release" OR CMAKE_BUILD_TYPE STREQUAL "Static_Debug")

include_directories("${CMAKE_SOURCE_DIR}/external/fmt")

add_subdirectory("${CMAKE_SOURCE_DIR}/external/fmt")

set(SOURCE_FILES
  AdvFilterFrame.cpp
  AdvSortFrame.cpp
  AnalysisBox.cpp
  AnalysisPanel.cpp
  App.cpp
  AppIDs.cpp
  ColorOptionsDlg.cpp
  CustomGridCellEditors.cpp
  DataPanel.cpp
  DbFilter.cpp
  EditTagDialog.cpp
  Helpers.cpp
  MainFrame.cpp
  QuickFilter.cpp
  Settings.cpp
  SqlGridCommand.cpp
  SqlStrings.cpp
  SqlTitleAliasCommand.cpp
  StatsPanel.cpp
  TitleAliasDialog.cpp
  TopBar.cpp
  )
set(CPPW_SQLITE3 cppw/Sqlite3/Sqlite3Connection.cpp
  cppw/Sqlite3/Sqlite3Exception.cpp
  cppw/Sqlite3/Sqlite3Result.cpp
  cppw/Sqlite3/Sqlite3Statement.cpp)

add_library(extensions-functions STATIC
  external/sqlite3-extensions/extension-functions.c)
target_compile_definitions(extensions-functions PRIVATE
  SQLITE_CORE=1)

add_executable(AnimeStats ${CPPW_SQLITE3} ${SOURCE_FILES})

if(WIN STREQUAL "true" OR WIN32)
  target_sources(AnimeStats PUBLIC resource.rc)
endif(WIN STREQUAL "true" OR WIN32)

find_package(SQLite3 REQUIRED)
if(SQLITE3_FOUND)
  include_directories(${SQLITE3_INCLUDE_DIRS})
  target_link_libraries(AnimeStats ${SQLITE3_LIBRARIES})
endif(SQLITE3_FOUND)

find_package(wxWidgets REQUIRED core base adv xml)
if(wxWidgets_FOUND)
  include("${wxWidgets_USE_FILE}")
  target_link_libraries(AnimeStats ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

target_link_libraries(AnimeStats
  extensions-functions
  fmt)

file(COPY "${CMAKE_SOURCE_DIR}/sql"
   DESTINATION .)

set(INSTALL_PERMISSIONS_EXECUTE OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                GROUP_READ GROUP_EXECUTE
                                WORLD_READ WORLD_EXECUTE)
set(INSTALL_PERMISSIONS_NO_EXECUTE OWNER_READ OWNER_WRITE
                                   GROUP_READ
                                   WORLD_READ)
install(TARGETS AnimeStats
   DESTINATION bin
   PERMISSIONS ${INSTALL_PERMISSIONS_EXECUTE})
install(DIRECTORY sql
   DESTINATION share/AnimeStats
   FILE_PERMISSIONS ${INSTALL_PERMISSIONS_NO_EXECUTE}
   DIRECTORY_PERMISSIONS ${INSTALL_PERMISSIONS_EXECUTE})
