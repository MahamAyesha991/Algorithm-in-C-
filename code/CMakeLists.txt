# The minimum required version of cmake
cmake_minimum_required(VERSION 3.28)

# Set required C++ standard
set(CMAKE_CXX_STANDARD 20)

# The name of the project
project(assignment4)

# Use Debug as default build
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Configure static linking
option(BUILD_SHARED_LIBS "Build a shared (ON) or static (OFF) executable" ON)
if(APPLE)
    option(USE_LIBCPP "Use libc++ instead of libstdc++" ON)
    option(USE_CXXABI "Use cxxabi for clang" ON)
else()
    option(USE_LIBCPP "Use libc++ instead of libstdc++" OFF)
    option(USE_CXXABI "Use cxxabi for clang" OFF)
endif()


if(NOT ${BUILD_SHARED_LIBS})
    #disable -rdynamic
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
    # prefer static libraries
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif(WIN32)
endif()

# Compiler specific additional flags (e.g. {GCC,CLANG,MSVC}_CXX_FLAGS_{DEBUG,RELWITHDEBINFO,RELEASE})
set(GCC_CXX_FLAGS_DEBUG " -pedantic -ftemplate-depth=1024 -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused -Wdouble-promotion -Wimplicit-fallthrough=1 -Wswitch-default -Wsuggest-override -Wduplicated-branches -Wduplicated-cond")
set(CLANG_CXX_FLAGS_DEBUG " -Weverything -Wno-c++98-compat -Wno-documentation -Wno-unknown-pragmas -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wno-c++98-compat-pedantic -Wno-disabled-macro-expansion")
set(MSVC_CXX_FLAGS_DEBUG " /W4 /FS /MP ")
set(MSVC_CXX_FLAGS_RELEASE " /MP ")

if(__COMPILER_GNU AND NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # GCC, MINGW
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_CXX_FLAGS_DEBUG}")
    if(NOT ${BUILD_SHARED_LIBS})
      set(CMAKE_EXE_LINKER_FLAGS "-static")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    if(${USE_LIBCPP})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CLANG_CXX_FLAGS_DEBUG}")
    if(NOT ${BUILD_SHARED_LIBS})
      set(CMAKE_EXE_LINKER_FLAGS "-static")
    endif()
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${MSVC_CXX_DEBUG_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_CXX_RELEASE_FLAGS}")
endif()

# add the executables
add_executable(task2 task2.cpp)
add_executable(task3 task3.cpp munkres_algorithm.cpp munkres_algorithm.hpp matrix.hpp)
add_executable(task4 task4.cpp bloom_filter.hpp)
add_executable(task5 task5.cpp matrix.hpp)


# link with libraries
if(NOT WIN32)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${USE_CXXABI})
        set(CXX_ABI c++abi)
    endif()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_link_libraries(task2 ${CXX_ABI})
        target_link_libraries(task3 ${CXX_ABI})
        target_link_libraries(task4 ${CXX_ABI})
        target_link_libraries(task5 ${CXX_ABI})
   endif()
endif()
