# -*- mode: CMAKE; -*-

cmake_minimum_required(VERSION 3.1)

project( replxx VERSION 0.0.2 LANGUAGES CXX C )
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

option(${PROJECT_NAME_UPPER}_BuildExamples "Build the examples." ON)
option(BUILD_SHARED_LIBS "Build as a shared library" OFF)

set( CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build" )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE string "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "Build mode: ${CMAKE_BUILD_TYPE}")

# INFO
set(${PROJECT_NAME_UPPER}_DISPLAY_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME_UPPER}_URL_INFO_ABOUT "https://github.com/AmokHuginnsson/replxx")
set(${PROJECT_NAME_UPPER}CONTACT "amok@codestation.org")
set(${PROJECT_NAME_UPPER}_FRIENDLY_STRING "replxx - Read Evaluate Print Loop library")

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()
# compiler options
if(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "Compiler type GNU: ${CMAKE_CXX_COMPILER}")
	set(BASE_COMPILER_OPTIONS "-Wall -Wextra -Wpedantic -D_GNU_SOURCE")
  set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${BASE_COMPILER_OPTIONS}")
  set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${BASE_COMPILER_OPTIONS} -O0 -g")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${BASE_COMPILER_OPTIONS} -Os")
  set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${BASE_COMPILER_OPTIONS} -O3 -fomit-frame-pointer")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${BASE_COMPILER_OPTIONS} -O3 -g")

elseif(CMAKE_COMPILER_IS_CLANGCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  message(STATUS "Compiler type CLANG: ${CMAKE_CXX_COMPILER}")
  set(BASE_COMPILER_OPTIONS "-Wall -Wextra -Wpedantic -D_GNU_SOURCE")
  set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${BASE_COMPILER_OPTIONS}")
  set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${BASE_COMPILER_OPTIONS} -O0 -g")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${BASE_COMPILER_OPTIONS} -Os")
  set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${BASE_COMPILER_OPTIONS} -O3 -fomit-frame-pointer")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${BASE_COMPILER_OPTIONS} -O3 -g")

elseif(MSVC)
  message(STATUS "Compiler type MSVC: ${CMAKE_CXX_COMPILER}")
  add_definitions("-D_CRT_SECURE_NO_WARNINGS=1")

  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /LTCG /ignore:4099")
  set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /SUBSYSTEM:CONSOLE /ignore:4099")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:CONSOLE /ignore:4099")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SUBSYSTEM:CONSOLE /ignore:4099")
else()
  # unknown compiler
  message(STATUS "Compiler type UNKNOWN: ${CMAKE_CXX_COMPILER}")
  set(BASE_COMPILER_OPTIONS "-Wall -Wextra -Wpedantic")
  set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${BASE_COMPILER_OPTIONS}")
  set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${BASE_COMPILER_OPTIONS} -O0 -g")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${BASE_COMPILER_OPTIONS} -Os")
  set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${BASE_COMPILER_OPTIONS} -O3 -fomit-frame-pointer")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${BASE_COMPILER_OPTIONS} -O3 -g")

endif()

set(SRC_FILES
  src/conversion.cxx
  src/ConvertUTF.cpp
  src/escape.cxx
  src/history.cxx
  src/replxx_impl.cxx
  src/io.cxx
  src/prompt.cxx
  src/replxx.cxx
  src/util.cxx
  src/wcwidth.cpp
  )

IF (WIN32)
list(APPEND SRC_FILES
  src/windows.cxx
  )
ENDIF()
# build libreplxx
add_library(
  ${PROJECT_NAME}
  ${SRC_FILES}
)

add_library(Replxx::Replxx ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
   PUBLIC ${PROJECT_SOURCE_DIR}/include
   PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()
# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
  ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL)

# Unit Tests
enable_testing()
find_package (Threads)
add_executable(${PROJECT_NAME}_test
  test/history_test.cpp
  test/replxx_cpp_interface_text.cpp)
target_link_libraries(${PROJECT_NAME}_test
  PRIVATE gtest_main
  ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}_test
  PRIVATE ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(AllTests_${PROJECT_NAME} ${PROJECT_NAME}_test)

# install
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# headers
install(FILES include/replxx.hxx include/replxx.h DESTINATION include)

if (${PROJECT_NAME_UPPER}_BuildExamples)
    # build example
    add_executable(
        example-c-api
        examples/c-api.c
    )

    add_executable(
        example-cxx-api
        examples/cxx-api.cxx
    )

    target_link_libraries(
        example-cxx-api
        PRIVATE ${PROJECT_NAME}
    )

    if ( NOT MSVC )
        set( CXX_LIB stdc++ )
    endif()

    target_link_libraries(
        example-c-api
        PRIVATE ${PROJECT_NAME} ${CXX_LIB}
    )
endif()

# packaging
include(CPack)

if (MSVC)
	set_target_properties( ${PROJECT_NAME} PROPERTIES RELEASE_OUTPUT_NAME ${PROJECT_NAME} DEBUG_OUTPUT_NAME ${PROJECT_NAME}-d )
else ()
  set(CPACK_SET_DESTDIR ON)
endif ()

set(CPACK_PACKAGE_VENDOR  "codestation.org")
set(CPACK_PACKAGE_CONTACT "amok@codestation.org")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_STRIP_FILES "ON")

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_DEBIAN_PACKAGE_SECTION "utilities")
