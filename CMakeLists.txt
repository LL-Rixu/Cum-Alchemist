cmake_minimum_required(VERSION 3.31-msvc6)

project(CumAlchemist VERSION 0.0.1)

set(CMAKE_CXX_STANDARD			 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS		YES)

# Add as submodules
find_package(binary_io REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SKYRIM_SUPPORT_AE ON CACHE BOOL "" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE BOOL "" FORCE)
set(CMAKE_CXX_FLAGS "/EHsc /MP")
set(CMAKE_STATIC_LINKER_FLAGS "/LTCG")

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive- /Zc:preprocessor")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /GF /GL")
	set(CMAKE_SHARED_LINKER_FLAGS "/LTCG")
ENDIF()

add_subdirectory(extern/CommonLibSSE EXCLUDE_FROM_ALL)

add_library(${PROJECT_NAME} SHARED 	"source/entry.cpp" 
									"source/CumAlchemist.cpp" 
									"source/alchemy.cpp" 
									"source/fasthash.cpp" 
									"source/potion.cpp" 
									"source/key.cpp" 
									"source/log.cpp" 
									"source/io.cpp" 
									"source/cum.cpp"
									"source/perk.cpp"
									"source/MCM.cpp"
									"source/SkyUI.cpp"
									"source/pages.cpp"
									)

target_precompile_headers(${PROJECT_NAME} PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/PCH.h>")

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
		CommonLibSSE::CommonLibSSE
		binary_io::binary_io
		spdlog::spdlog
	)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Data/SKSE/Plugins)

install(CODE "file(ARCHIVE_CREATE \
    OUTPUT \"${CMAKE_CURRENT_SOURCE_DIR}/Cum Alchemist.7z\" \
    PATHS \"${CMAKE_CURRENT_SOURCE_DIR}/Data\" \
    FORMAT \"7zip\" \
)")