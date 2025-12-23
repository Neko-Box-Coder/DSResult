cmake_minimum_required(VERSION 3.10)

if(TARGET DSResult)
    return()
endif()

project(DSResult)

if(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    option(DS_BUILD_EXAMPLES "Build DSResult Examples" on)
else()
    option(DS_BUILD_EXAMPLES "Build DSResult Examples" off)
endif()

set(DS_EXPECTED_BACKEND "TL" CACHE STRING "DSResult Expected Backend (TL,LITE,STD,CUSTOM)")
set_property(CACHE DS_EXPECTED_BACKEND PROPERTY STRINGS "TL" 
                                                        "LITE" 
                                                        "STD"
                                                        "CUSTOM")

option(DS_NO_PATH "Don't show file path for error trace" off)

add_library(DSResult INTERFACE)
target_include_directories(DSResult INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Include")

if(${DS_EXPECTED_BACKEND} STREQUAL "TL")
    target_include_directories( DSResult INTERFACE 
                                "${CMAKE_CURRENT_LIST_DIR}/External/expected/include")
    target_compile_definitions(DSResult INTERFACE DS_USE_TL_EXPECTED=1)
elseif(${DS_EXPECTED_BACKEND} STREQUAL "LITE")
    target_include_directories( ExpectedLiteExample INTERFACE 
                                "${CMAKE_CURRENT_LIST_DIR}/External/expected-lite/include")
    target_compile_definitions(DSResult INTERFACE DS_USE_EXPECTED_LITE=1)
elseif(${DS_EXPECTED_BACKEND} STREQUAL "STD")
    target_compile_definitions(DSResult INTERFACE DS_USE_STD_EXPECTED=1)
elseif(${DS_EXPECTED_BACKEND} STREQUAL "CUSTOM")
    target_compile_definitions(DSResult INTERFACE DS_USE_CUSTOM_EXPECTED=1)
else()
    message(FATAL_ERROR "Invalid DS_EXPECTED_BACKEND option: ${DS_EXPECTED_BACKEND}")
endif()

if(${DS_NO_PATH})
    target_compile_definitions(DSResult INTERFACE DS_NO_PATH=1)
else()
    target_compile_definitions(DSResult INTERFACE DS_NO_PATH=0)
endif()

if(${DS_BUILD_EXAMPLES})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(DS_EXAMPLE_COMPILE_FLAGS "/utf-8" "/WX" "/Wall" "/wd4820")
    else()
        set(DS_EXAMPLE_COMPILE_FLAGS "-Wall" "-Wextra" "-Wpedantic" "-Werror")
    endif()
    
    target_compile_options(DSResult INTERFACE "${DS_EXAMPLE_COMPILE_FLAGS}")
    
    add_executable(TlExpectedExample    "${CMAKE_CURRENT_LIST_DIR}/Examples/TlExpectedExample.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/Examples/TryExamples.cpp")
    set_property(TARGET TlExpectedExample PROPERTY CXX_STANDARD 11)
    target_include_directories( TlExpectedExample PRIVATE 
                                "${CMAKE_CURRENT_LIST_DIR}/External/expected/include"
                                "${CMAKE_CURRENT_LIST_DIR}/Include")
    target_compile_definitions(TlExpectedExample PRIVATE DS_USE_TL_EXPECTED=1)
    
    
    add_executable(ExpectedLiteExample  "${CMAKE_CURRENT_LIST_DIR}/Examples/ExpectedLiteExample.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/Examples/TryExamples.cpp")
    set_property(TARGET ExpectedLiteExample PROPERTY CXX_STANDARD 11)
    target_include_directories( ExpectedLiteExample PUBLIC 
                                "${CMAKE_CURRENT_LIST_DIR}/External/expected-lite/include"
                                "${CMAKE_CURRENT_LIST_DIR}/Include")
    target_compile_definitions(ExpectedLiteExample PUBLIC DS_USE_EXPECTED_LITE=1)
    
    add_executable(StdExpectedExample   "${CMAKE_CURRENT_LIST_DIR}/Examples/StdExpectedExample.cpp"
                                        "${CMAKE_CURRENT_LIST_DIR}/Examples/TryExamples.cpp")
    set_property(TARGET StdExpectedExample PROPERTY CXX_STANDARD 23)
    target_include_directories(StdExpectedExample PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include")
    target_compile_definitions(StdExpectedExample PRIVATE DS_USE_STD_EXPECTED=1)
endif()
