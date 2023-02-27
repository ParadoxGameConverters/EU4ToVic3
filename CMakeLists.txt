cmake_minimum_required(VERSION 3.16)

project(EU4ToVic3)
set(CONVERTER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/EU4ToVic3/Source)
set(CONVERTER_DIR ${CMAKE_SOURCE_DIR}/EU4ToVic3)
set(FRONTER_DIR ${CMAKE_SOURCE_DIR}/Fronter/)
set(FRONTER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Fronter/Fronter/Source)
set(FRONTER_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Release-Linux)
set(CONVERTER_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Release-Linux/${PROJECT_NAME})
set(TEST_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/test/Release-Linux)
set(CMAKE_CXX_STANDARD 20)
set(UNICODE_DEFAULT OFF)

add_compile_options(-D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread -O2)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED) 
include_directories(${CURL_INCLUDE_DIR})

include_directories("/usr/include/wx-3.2-unofficial")
include_directories("/usr/lib/x86_64-linux-gnu/wx/include/gtk3-unicode-3.2-unofficial")
include_directories("commonItems")
include_directories("commonItems/external/zip/src")
include_directories("commonItems/external/googletest/googletest")
include_directories("commonItems/external/googletest/googletest/include")
include_directories("commonItems/external/googletest/googlemock")
include_directories("commonItems/external/googletest/googlemock/include")
include_directories("commonItems/external/zip/src")
include_directories("commonItems/external/json/single_include")
include_directories("Fronter/external/SQLiteCpp/include")
include_directories("Fronter/external/SQLiteCpp/sqlite3")
include_directories("Fronter/Fronter/Source")
include_directories("${CONVERTER_DIR}")
include_directories("${CONVERTER_SOURCE_DIR}")
include_directories("${CONVERTER_SOURCE_DIR}/Mappers")
include_directories("${CONVERTER_SOURCE_DIR}/EU4World")
include_directories("${CONVERTER_SOURCE_DIR}/EU4World/Loaders")
include_directories("${CONVERTER_SOURCE_DIR}/V3World")
include_directories("${CONVERTER_SOURCE_DIR}/Helpers")
include_directories("${CONVERTER_SOURCE_DIR}/Output")
include_directories("${CONVERTER_SOURCE_DIR}/Configuration")

link_directories(${CONVERTER_DIR}/Resources)

#### Converter ####

set(PCH_SOURCES "commonItems/BulkParser.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Color.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/CommonFunctions.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/CommonRegexes.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ConvenientParser.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ConverterVersion.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Date.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/GameVersion.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Log.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/OSCompatibilityLayer.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Parser.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ParserHelpers.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/StringUtils.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/targa.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ModLoader/ModFilesystem.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ModLoader/Mod.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ModLoader/ModLoader.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/ModLoader/ModParser.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Localization/LocalizationBlock.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Localization/LocalizationDatabase.h")
set(PCH_SOURCES ${PCH_SOURCES} "commonItems/Localization/LocalizationLambdas.h")

set(COMMON_SOURCES "commonItems/CommonFunctions.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/Color.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ConvenientParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ConverterVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/Date.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/GameVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/LinuxUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/Log.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ModLoader/ModLoader.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ModLoader/ModParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/Parser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ParserHelpers.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/OSCommonLayer.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/StringUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/targa.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ModLoader/ModFilesystem.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ModLoader/ModLoader.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/ModLoader/ModParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "commonItems/external/zip/src/zip.c")

add_library(CommonItems ${COMMON_SOURCES})

file(GLOB MAIN_SOURCES "${CONVERTER_SOURCE_DIR}/*.cpp")
file(GLOB CONFIGURATION_SOURCES "${CONVERTER_SOURCE_DIR}/Configuration/*.cpp")
file(GLOB EU4_WORLD_SOURCES "${CONVERTER_SOURCE_DIR}/EU4World/*.cpp")
file(GLOB EU4_SUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/EU4World/*/*.cpp")
file(GLOB EU4_SUBSUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/EU4World/*/*/*.cpp")
file(GLOB V3_WORLD_SOURCES "${CONVERTER_SOURCE_DIR}/V3World/*.cpp")
file(GLOB V3_SUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/V3World/*/*.cpp")
file(GLOB V3_SUBSUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/V3World/*/*/*.cpp")
file(GLOB V3_SUBSUBSUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/V3World/*/*/*/*.cpp")
file(GLOB OUT_WORLD_SOURCES "${CONVERTER_SOURCE_DIR}/Output/*.cpp")
file(GLOB OUT_SUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/Output/*/*.cpp")
file(GLOB OUT_SUBSUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/Output/*/*/*.cpp")
file(GLOB MAPPER_SOURCES "${CONVERTER_SOURCE_DIR}/Mappers/*/*.cpp")
file(GLOB MAPPER_SUBDIR_SOURCES "${CONVERTER_SOURCE_DIR}/Mappers/*/*/*.cpp")

add_library(EU4ToVic3lib
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
)

target_precompile_headers(EU4ToVic3lib PUBLIC ${PCH_SOURCES})
target_link_libraries(EU4ToVic3lib LINK_PUBLIC CommonItems rakaly)

add_executable(EU4ToVic3Converter
	${MAIN_SOURCES}
)
set_target_properties(EU4ToVic3Converter
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CONVERTER_OUTPUT_DIRECTORY}
)
target_precompile_headers(EU4ToVic3Converter REUSE_FROM EU4ToVic3lib)
target_link_libraries(EU4ToVic3Converter LINK_PUBLIC EU4ToVic3lib)

### FRONTER ###

file(GLOB F_MAIN_SOURCES "${FRONTER_SOURCE_DIR}/*.cpp")
file(GLOB F_CONFIGURATION_SOURCES "${FRONTER_SOURCE_DIR}/Configuration/*.cpp")
file(GLOB F_CONFIGURATION_OPTIONS_SOURCES "${FRONTER_SOURCE_DIR}/Configuration/Options/*.cpp")
file(GLOB F_FRAMES_SOURCES "${FRONTER_SOURCE_DIR}/Frames/*.cpp")
file(GLOB F_FRAMES_TABS_SOURCES "${FRONTER_SOURCE_DIR}/Frames/Tabs/*.cpp")
file(GLOB F_UTILS_SOURCES "${FRONTER_SOURCE_DIR}/Utils/*.cpp")
file(GLOB F_UTILS_LOCALIZATION_SOURCES "${FRONTER_SOURCE_DIR}/Utils/Localization/*.cpp")
file(GLOB F_CONVERTERLAUNCHER_SOURCES "${FRONTER_SOURCE_DIR}/WorkerThreads/ConverterLauncher/*.cpp")
list(REMOVE_ITEM F_CONVERTERLAUNCHER_SOURCES "${FRONTER_SOURCE_DIR}/WorkerThreads/ConverterLauncher/WinConverterLauncher.cpp")
file(GLOB F_LOGWATCHER_SOURCES "${FRONTER_SOURCE_DIR}/WorkerThreads/LogWatcher/*.cpp")
file(GLOB F_MODCOPIER_SOURCES "${FRONTER_SOURCE_DIR}/WorkerThreads/ModCopier/*.cpp")
file(GLOB F_UPDATECHECKER_SOURCES "${FRONTER_SOURCE_DIR}/UpdateChecker/*.cpp")
file(GLOB F_SQLITE_SOURCES "${FRONTER_SOURCE_DIR}/../../external/SQLiteCpp/src/*.cpp")
file(GLOB F_SQLITE3_SOURCES "${FRONTER_SOURCE_DIR}/../../external/SQLiteCpp/sqlite3/*.c")

add_executable(ConverterFrontend
	${F_MAIN_SOURCES}
	${F_CONFIGURATION_SOURCES}
	${F_CONFIGURATION_OPTIONS_SOURCES}
	${F_FRAMES_SOURCES}
	${F_FRAMES_TABS_SOURCES}
	${F_UTILS_SOURCES}
	${F_UTILS_LOCALIZATION_SOURCES}
	${F_CONVERTERLAUNCHER_SOURCES}
	${F_LOGWATCHER_SOURCES}
	${F_MODCOPIER_SOURCES}
	${F_UPDATECHECKER_SOURCES}
	${F_SQLITE_SOURCES}
	${F_SQLITE3_SOURCES}
)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
set_target_properties(ConverterFrontend
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${FRONTER_OUTPUT_DIRECTORY}
)
target_link_libraries(ConverterFrontend LINK_PUBLIC ${wxWidgets_LIBRARIES} ${CURL_LIBRARIES} CommonItems)

add_custom_command(TARGET EU4ToVic3Converter POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git rev-parse HEAD > Release-Linux/commit_id.txt)
add_custom_command(TARGET EU4ToVic3Converter POST_BUILD WORKING_DIRECTORY ${CONVERTER_DIR} COMMAND chmod u+x Copy_Files.sh)
add_custom_command(TARGET EU4ToVic3Converter POST_BUILD WORKING_DIRECTORY ${CONVERTER_DIR} COMMAND ./Copy_Files.sh)

### TESTS ###

file(GLOB TEST_SOURCES "EU4ToVic3Tests/*.cpp")
file(GLOB TEST_SUB_SOURCES "EU4ToVic3Tests/*/*.cpp")
file(GLOB TEST_SUB_SUB_SOURCES "EU4ToVic3Tests/*/*/*.cpp")
set(GTEST_SOURCES ${GTEST_SOURCES} "commonItems/external/googletest/googletest/src/gtest_main.cc")
set(GTEST_SOURCES ${GTEST_SOURCES} "commonItems/external/googletest/googletest/src/gtest-all.cc")
set(GTEST_SOURCES ${GTEST_SOURCES} "commonItems/external/googletest/googlemock/src/gmock-all.cc")

add_executable(
	EU4ToVic3Tests
	${TEST_SOURCES}
	${TEST_SUB_SOURCES}
	${TEST_SUB_SUB_SOURCES}
	${GTEST_SOURCES}
)

set_target_properties(EU4ToVic3Tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}
)

target_precompile_headers(EU4ToVic3Tests REUSE_FROM EU4ToVic3lib)
target_link_libraries(EU4ToVic3Tests LINK_PUBLIC EU4ToVic3lib)

file(COPY "EU4ToVic3Tests/TestFiles" DESTINATION "${TEST_OUTPUT_DIRECTORY}")