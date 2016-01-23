#Causes some libraries to fail to be statically linked if requiring a lower version for some reason
cmake_minimum_required(VERSION 3.3)
project(AnimeStats)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -s")
set(STATIC_ARGS "-static-libstdc++ -static-libgcc -static")
set(CMAKE_EXE_LINKER_FLAGS_STATIC_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${STATIC_ARGS}")
set(CMAKE_EXE_LINKER_FLAGS_STATIC_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${STATIC_ARGS}")

if(CMAKE_BUILD_TYPE STREQUAL "Static_Release" OR CMAKE_BUILD_TYPE STREQUAL "Static_Debug")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif(CMAKE_BUILD_TYPE STREQUAL "Static_Release" OR CMAKE_BUILD_TYPE STREQUAL "Static_Debug")

set(SOURCE_FILES 
        AdvFilterFrame.cpp
        App.cpp
        DataPanel.cpp
        GridCellDateEditor.cpp
        MainFrame.cpp
        SqlGridCommand.cpp)
set(CPPW_SQLITE3 cppw/Sqlite3/Sqlite3Connection.cpp
        cppw/Sqlite3/Sqlite3Exception.cpp
        cppw/Sqlite3/Sqlite3Result.cpp
        cppw/Sqlite3/Sqlite3Statement.cpp)
add_executable(AnimeStats ${CPPW_SQLITE3} ${SOURCE_FILES})
if(WIN STREQUAL "true" OR WIN32)
    target_sources(AnimeStats PUBLIC resource.rc)
endif(WIN STREQUAL "true" OR WIN32)

find_package(SQLite3 REQUIRED)
if(SQLITE3_FOUND)
    include_directories(${SQLITE3_INCLUDE_DIRS})
    target_link_libraries(AnimeStats ${SQLITE3_LIBRARIES})
endif(SQLITE3_FOUND)

find_package(wxWidgets REQUIRED)
if(wxWidgets_FOUND)
    include("${wxWidgets_USE_FILE}")
    target_link_libraries(AnimeStats ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)