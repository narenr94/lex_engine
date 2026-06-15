cmake_minimum_required(VERSION 3.10)
project(lexEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/lexEngineDefines.cpp
    src/lexEnginePlayerInput.cpp
    src/manager.cpp
    src/player.cpp
    src/special.cpp
)

# Header files
set(HEADERS
    include/cards.h
    include/lexEngineDefines.h
    include/lexEnginePlayerInput.h
    include/manager.h
    include/player.h
    include/spaces.h
    include/special.h
)

# Create shared library
add_library(lexEngine SHARED ${SOURCES})

# Set library properties
set_target_properties(lexEngine PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${HEADERS}"
)

# Include directories
target_include_directories(lexEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Installation paths
include(GNUInstallDirs)

# Install the library
install(TARGETS lexEngine
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/lexEngine
)

# Install header files explicitly
# install(FILES ${HEADERS}
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lexEngine
# )

# Configure pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lexEngine.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/lexEngine.pc
    @ONLY
)

# Install pkg-config file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lexEngine.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

