cmake_minimum_required(VERSION 3.10)
project(ScrabbleGame VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS})

# Source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        ${SDL2_LIBRARIES} 
        SDL2_ttf::SDL2_ttf
    )
else()
    target_link_libraries(${PROJECT_NAME} 
        ${SDL2_LIBRARIES} 
        SDL2_ttf
    )
endif()

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/dictionary.txt DESTINATION ${CMAKE_BINARY_DIR})

# Platform-specific settings
if(WIN32)
    # Copy SDL2 DLLs to output directory for Windows
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL2_DLL_DIR "${SDL2_PREFIX}/lib/x64")
    else()
        set(SDL2_DLL_DIR "${SDL2_PREFIX}/lib/x86")
    endif()
    
    if(EXISTS "${SDL2_DLL_DIR}")
        file(GLOB SDL2_DLLS "${SDL2_DLL_DIR}/*.dll")
        foreach(DLL ${SDL2_DLLS})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
        endforeach()
    endif()
endif()

# Install targets
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY assets DESTINATION .)
install(FILES dictionary.txt DESTINATION .)

# CPack settings for creating installers
set(CPACK_PACKAGE_NAME "ScrabbleGame")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scrabble Game - OOP Project")
set(CPACK_PACKAGE_VENDOR "Computer Science Student")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
