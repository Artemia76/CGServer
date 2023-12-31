# CGServer CMakeLists.txt 

# Minimum CMake Version compatibility
cmake_minimum_required(VERSION 3.18)

set(CMAKE_VERBOSE_MAKEFILE ON)
# Set the project name
project (cgserver)

if (POLICY CMP0127)
  cmake_policy(SET CMP0127 NEW) # cmake_dependent_option changes
endif ()

set(cmake_dir "${PROJECT_SOURCE_DIR}/cmake")
list(INSERT CMAKE_MODULE_PATH 0 "${cmake_dir}")

# Set Standard used for this project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#Build Options

option(BUILD_32 "Check this to enable to choose 32bit build" ON)

#Aw SDK Selector 
set(AW_SDK 41 CACHE STRING "Choose version of aw sdk")

set_property(CACHE AW_SDK PROPERTY STRINGS 41 93)

if (AW_SDK EQUAL 41)
    set(LIB_AW_SDK "aw_sdk")
elseif (AW_SDK EQUAL 93)
    set(LIB_AW_SDK "aw_sdk.51")
endif ()

if (UNIX)
    set(CONFIG_DIR "/etc/cgserver")
endif(UNIX)

# Create a source variable
set(SOURCES
    src/CBotCG.cpp
    src/CBot.cpp
    src/CColour.cpp
    src/CDiffusion.cpp
    src/Ciel.cpp
    src/CLog.cpp
    src/CMainApp.cpp
    src/COutils.cpp
    src/Cpasspriv.cpp
    src/CServiceApp.cpp
    src/Ctrlaw.cpp
    src/CUser.cpp
    src/DBase.cpp
    src/DBColor.cpp
    src/CIRCGateway.cpp
)

# Add executable with the above sources
add_executable(${PROJECT_NAME}
        ${SOURCES}
)

if (BUILD_32)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    MESSAGE(STATUS "Excluding 64bit library paths from search.")
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
    set(ARCH i386)
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
    set(ARCH amd64)
endif()

#Search project dependencies
set(wxWidgets_CONFIG_OPTIONS --toolkit=base)
find_package(wxWidgets REQUIRED base net)
if(NOT wxWidgets_FOUND)
    message( FATAL_ERROR "wxWidgets librairies was not found. This lib is mandatory for cgserver" )
endif()
include(${wxWidgets_USE_FILE})

#includes directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/config
)

target_link_libraries(${PROJECT_NAME}
    LINK_PUBLIC
        ${wxWidgets_LIBRARIES}
        ${LIB_AW_SDK}
)

if (UNIX)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    if (NOT EXISTS "/etc/cgserver/cgserver.ini")
            install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/cgserver.ini" DESTINATION "/etc/cgserver" )
    endif()

    if (NOT EXISTS "/etc/systemd/system/cgserver.service")
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/systemd/cgserver.service" DESTINATION "/etc/systemd/system" )
    endif()

    install(DIRECTORY DESTINATION "/var/log/cgserver")
endif (UNIX)