#Causes some libraries to fail to be statically linked if requiring a lower version for some reason
cmake_minimum_required(VERSION 3.3)
project(AnimeStats)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsuggest-override")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -s")
set(CMAKE_CXX_FLAGS_STATIC_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${STATIC_ARGS}")
set(CMAKE_CXX_FLAGS_STATIC_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${STATIC_ARGS}")
set(STATIC_ARGS "-static-libstdc++ -static-libgcc -static")
set(CMAKE_EXE_LINKER_FLAGS_STATIC_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${STATIC_ARGS}")
set(CMAKE_EXE_LINKER_FLAGS_STATIC_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${STATIC_ARGS}")

string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
if("${BUILD_TYPE_UPPER}" STREQUAL "STATIC_RELEASE" OR
    "${BUILD_TYPE_UPPER}" STREQUAL "STATIC_DEBUG")
  message("Using static libraries")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT DEFINED USE_AMALGAMATION)
  set(USE_AMALGAMATION 1)
elseif(NOT DEFINED USE_AMALGAMATION)
  set(USE_AMALGAMATION 0)
endif()

include_directories("${CMAKE_SOURCE_DIR}/external/fmt")
add_subdirectory("${CMAKE_SOURCE_DIR}/external/fmt")

set(SOURCE_FILES
  AdvFilterFrame.cpp
  AdvSortFrame.cpp
  AnalysisBox.cpp
  AnalysisPanel.cpp
  App.cpp
  AppIDs.cpp
  BasicSelect.cpp
  CreateCommands.cpp
  ColorOptionsDlg.cpp
  CustomGridCellEditors.cpp
  DataGrid.cpp
  DataPanel.cpp
  DbFilter.cpp
  EditTagDialog.cpp
  GroupStatsDlg.cpp
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

add_library(count_value STATIC
  external/my-extensions/count_value.c)
target_compile_definitions(count_value PRIVATE
  SQLITE_CORE=1)
add_library(extension-functions STATIC
  external/sqlite3-extensions/extension-functions.c)
target_compile_definitions(extension-functions PRIVATE
  SQLITE_CORE=1)

add_executable(AnimeStats ${CPPW_SQLITE3} ${SOURCE_FILES})

if(WIN32)
  target_sources(AnimeStats PUBLIC resource.rc)
endif()

find_package(SQLite3)
if(SQLITE3_FOUND AND NOT USE_AMALGAMATION)
  message("Using system SQLite")
  include_directories(${SQLITE3_INCLUDE_DIRS})
  target_link_libraries(AnimeStats ${SQLITE3_LIBRARIES})
elseif(NOT SQLITE_FOUND OR USE_AMALGAMATION)
  if (NOT SQLITE_FOUND AND NOT USE_AMALGAMATION)
    message(WARNING "\
Using SQLite Amalgamation despite settings \
because no SQLite was found on the system")
  else()
    message("Using packaged SQLite Amalgamation")
  endif()
  include_directories("${CMAKE_SOURCE_DIR}/external/sqlite-amalgamation")
  add_library(sqlite-amalgamation STATIC
      external/sqlite-amalgamation/sqlite3.c)
  target_link_libraries(AnimeStats sqlite-amalgamation dl)
endif()

find_package(wxWidgets REQUIRED core base adv xml)
if(wxWidgets_FOUND)
  include("${wxWidgets_USE_FILE}")
  target_link_libraries(AnimeStats ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

target_link_libraries(AnimeStats
  count_value
  extension-functions
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
