#Causes some libraries to fail to be statically linked if requiring a lower version for some reason
cmake_minimum_required(VERSION 3.3)
project(animestats)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "release")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsuggest-override")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
if(NOT DEFINED PROFILING OR NOT PROFILING)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g \
-fno-omit-frame-pointer \
-fno-inline-functions \
-fno-inline-functions-called-once \
-fno-optimize-sibling-calls")
endif()
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
  src/sql/BasicSelect.cpp
  src/sql/CreateCommands.cpp
  src/sql/SqlStrings.cpp
  src/AdvFilterFrame.cpp
  src/AdvSortFrame.cpp
  src/AnalysisBox.cpp
  src/AnalysisPanel.cpp
  src/App.cpp
  src/AppIDs.cpp
  src/ColorOptionsDlg.cpp
  src/CustomGridCellEditors.cpp
  src/DataGrid.cpp
  src/DataPanel.cpp
  src/DbFilter.cpp
  src/EditTagDialog.cpp
  src/GroupStatsDlg.cpp
  src/Helpers.cpp
  src/LegalDlg.cpp
  src/MainFrame.cpp
  src/QuickFilter.cpp
  src/Settings.cpp
  src/SqlGridCommand.cpp
  src/SqlTitleAliasCommand.cpp
  src/StatsPanel.cpp
  src/TitleAliasDialog.cpp
  src/TopBar.cpp
  )
set(CPPW_SQLITE3
  src/cppw/Sqlite3/Sqlite3Connection.cpp
  src/cppw/Sqlite3/Sqlite3Exception.cpp
  src/cppw/Sqlite3/Sqlite3Result.cpp
  src/cppw/Sqlite3/Sqlite3Statement.cpp)

add_library(count_value STATIC
  external/my-extensions/count_value.c)
target_compile_definitions(count_value PRIVATE
  SQLITE_CORE=1)
add_library(extension-functions STATIC
  external/sqlite3-extensions/extension-functions.c)
target_compile_definitions(extension-functions PRIVATE
  SQLITE_CORE=1)

add_executable(animestats ${CPPW_SQLITE3} ${SOURCE_FILES})

if(WIN32)
  target_sources(animestats PUBLIC src/resource.rc)
endif()

find_package(SQLite3)
if(SQLITE3_FOUND AND NOT USE_AMALGAMATION)
  message("Using system SQLite")
  include_directories(${SQLITE3_INCLUDE_DIRS})
  target_link_libraries(animestats ${SQLITE3_LIBRARIES})
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
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(animestats dl)
  endif()
  target_link_libraries(animestats sqlite-amalgamation)
endif()

find_package(wxWidgets REQUIRED core base adv xml)
if(wxWidgets_FOUND)
  include("${wxWidgets_USE_FILE}")
  target_link_libraries(animestats ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

target_link_libraries(animestats
  count_value
  extension-functions
  fmt)

set(INSTALL_PERMISSIONS_EXECUTE OWNER_READ OWNER_EXECUTE OWNER_WRITE
                                GROUP_READ GROUP_EXECUTE
                                WORLD_READ WORLD_EXECUTE)
install(TARGETS animestats
   DESTINATION bin
   PERMISSIONS ${INSTALL_PERMISSIONS_EXECUTE})
