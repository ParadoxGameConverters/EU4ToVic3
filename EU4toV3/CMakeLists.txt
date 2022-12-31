cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE on)

project(EU4ToVic3)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Source)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_compile_options("-lstdc++fs")
add_compile_options("-O2")
add_compile_options("-pthread")
add_link_options(-no-pie)

include_directories("../commonItems")
include_directories("Source")
include_directories("Source/Mappers")
include_directories("Source/EU4World")
include_directories("Source/EU4World/Loaders")
include_directories("Source/V3World")
include_directories("Source/Helpers")
include_directories("Source/Output")
include_directories("Source/Configuration")
include_directories("${PROJECT_SOURCE_DIR}")
link_directories(${CMAKE_SOURCE_DIR}/Resources)

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CONFIGURATION_SOURCES "${PROJECT_SOURCE_DIR}/Configuration/*.cpp")
file(GLOB EU4_WORLD_SOURCES "${PROJECT_SOURCE_DIR}/EU4World/*.cpp")
file(GLOB EU4_SUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/EU4World/*/*.cpp")
file(GLOB EU4_SUBSUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/EU4World/*/*/*.cpp")
file(GLOB V3_WORLD_SOURCES "${PROJECT_SOURCE_DIR}/V3World/*.cpp")
file(GLOB V3_SUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/V3World/*/*.cpp")
file(GLOB V3_SUBSUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/V3World/*/*/*.cpp")
file(GLOB V3_SUBSUBSUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/V3World/*/*/*/*.cpp")
file(GLOB OUT_WORLD_SOURCES "${PROJECT_SOURCE_DIR}/Output/*.cpp")
file(GLOB OUT_SUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/Output/*/*.cpp")
file(GLOB OUT_SUBSUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/Output/*/*/*.cpp")
file(GLOB MAPPER_SOURCES "${PROJECT_SOURCE_DIR}/Mappers/*/*.cpp")
file(GLOB MAPPER_SUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/Mappers/*/*/*.cpp")
set(COMMON_SOURCES "../commonItems/CommonFunctions.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Color.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ConvenientParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ConverterVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Date.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/GameVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/LinuxUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Log.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModFilesystem.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModLoader.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Parser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ParserHelpers.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/StringUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/OSCommonLayer.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/targa.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/external/zip/src/zip.cpp")

add_executable(EU4ToVic3 
	${MAIN_SOURCES}
	${CONFIGURATION_SOURCES}
	${EU4_WORLD_SOURCES}
	${EU4_SUBDIR_SOURCES}
	${EU4_SUBSUBDIR_SOURCES}
	${V3_WORLD_SOURCES}
	${V3_SUBDIR_SOURCES}
	${V3_SUBSUBDIR_SOURCES}
	${V3_SUBSUBSUBDIR_SOURCES}
	${OUT_WORLD_SOURCES}
	${OUT_SUBDIR_SOURCES}
	${OUT_SUBSUBDIR_SOURCES}
	${MAPPER_SOURCES}
	${MAPPER_SUBDIR_SOURCES}
	${COMMON_SOURCES}
)

target_link_libraries(EU4ToVic3 LINK_PUBLIC stdc++fs rakaly)

add_custom_command(TARGET EU4ToVic3 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git rev-parse HEAD > Release-Linux/commit_id.txt)
add_custom_command(TARGET EU4ToVic3 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND chmod u+x Copy_Files.sh)
add_custom_command(TARGET EU4ToVic3 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ./Copy_Files.sh)
