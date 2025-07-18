cmake_minimum_required(VERSION 3.10)

project(DSResult)

if(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    option(DS_BUILD_EXAMPLES "Build DSResult Examples" on)
else()
    option(DS_BUILD_EXAMPLES "Build DSResult Examples" off)
endif()

add_library(DSResult INTERFACE)
target_include_directories(DSResult INTERFACE "${CMAKE_CURRENT_LIST_DIR}/Include")

if(${DS_BUILD_EXAMPLES})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(DS_EXAMPLE_COMPILE_FLAGS "/utf-8" "/WX" "/Wall" "/wd4820")
    else()
        set(DS_EXAMPLE_COMPILE_FLAGS "-Wall" "-Wextra" "-Wpedantic" "-Werror")
    endif()
    
    target_compile_options(DSResult INTERFACE "${DS_EXAMPLE_COMPILE_FLAGS}")
    
    add_executable(TlExpectedExample "${CMAKE_CURRENT_LIST_DIR}/Examples/TlExpectedExample.cpp")
    set_property(TARGET TlExpectedExample PROPERTY CXX_STANDARD 11)
    target_include_directories(TlExpectedExample SYSTEM PRIVATE "${CMAKE_CURRENT_LIST_DIR}/External/expected/include")
    target_link_libraries(TlExpectedExample DSResult)
    
    add_executable(ExpectedLiteExample "${CMAKE_CURRENT_LIST_DIR}/Examples/ExpectedLiteExample.cpp")
    set_property(TARGET ExpectedLiteExample PROPERTY CXX_STANDARD 11)
    target_include_directories(ExpectedLiteExample SYSTEM PRIVATE "${CMAKE_CURRENT_LIST_DIR}/External/expected-lite/include")
    target_link_libraries(ExpectedLiteExample DSResult)
    
    add_executable(StdExpectedExample "${CMAKE_CURRENT_LIST_DIR}/Examples/StdExpectedExample.cpp")
    set_property(TARGET StdExpectedExample PROPERTY CXX_STANDARD 23)
    target_link_libraries(StdExpectedExample DSResult)
endif()
