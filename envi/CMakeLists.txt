cmake_minimum_required(VERSION 3.24)

include(FetchContent)

# proj

set(vProjName "GameOfLife")
set(vProjVnum "1.0")
set(vProjDesc "just another ConwayGameOfLife implemented by nikozdev")
set(vProjPath "${CMAKE_SOURCE_DIR}/..")

project(${vProjName} VERSION ${vProjVnum} DESCRIPTION ${vProjDesc})

# lang

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# code

set(vCodePath "${vProjPath}/code")
set(vCodeFile "${vCodePath}/${vProjName}.cpp")

# head

set(vHeadPath "${vProjPath}/code")
set(vHeadFile "${vCodePath}/${vProjName}.hpp")

# make

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "type of build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

set(vMakePath "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${vMakePath}")

##exe

set(vTexeName "t${vProjName}Exe")
set(vTexePath "${vMakePath}")
set(vTexeFile "${vTexePath}/${vTexeName}")

add_executable(${vTexeName})
target_sources(${vTexeName} PRIVATE ${vCodeFile})
target_include_directories(${vTexeName} PRIVATE ${vHeadPath})
target_precompile_headers(${vTexeName} PRIVATE ${vHeadFile})

find_package(SFML 2.6.0 COMPONENTS system graphics)
if(NOT SFML_FOUND)
  FetchContent_Declare(SFML
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG "2.6.0"
    OVERRIDE_FIND_PACKAGE)
  FetchContent_MakeAvailable(SFML)
  find_package(SFML 2.6.0 REQUIRED COMPONENTS system graphics)
endif(NOT SFML_FOUND)
target_link_libraries(${vTexeName} PRIVATE sfml-system sfml-graphics)

# test

enable_testing()
include(CTest)

function(fTest vName vText)
  add_test(${vName} COMMAND ${vTexeFile} "test" "${vName}")
  set_tests_properties(${vName} PROPERTIES PASS_REGULAR_EXPRESSION ${vText})
endfunction(fTest)

fTest("Hello" "HelloWorld")
fTest("SfmlWindow" "HelloWindow")
fTest("tGame_fMain" "tGame.fMain == vTruth")
