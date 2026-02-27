cmake_minimum_required(VERSION 3.31-msvc6)

project(CumAlchemist VERSION 0.0.1)

set(CMAKE_CXX_STANDARD			 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS		YES)

find_package(binary_io REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(CommonLibSSE REQUIRED CONFIG)

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
									"source/pages.cpp")

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

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_FLAGS "/EHsc /MP -DSKYRIM_SUPPORT_AE")

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive- /Zc:preprocessor")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /GF /GL")
	set(CMAKE_SHARED_LINKER_FLAGS "/LTCG")
ENDIF()

message(CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}")