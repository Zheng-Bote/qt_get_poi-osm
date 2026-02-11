cmake_minimum_required(VERSION 3.23)

project(get_poi-osm 
    VERSION 0.1.0
    DESCRIPTION "Get POIs from OpenStreetMap"
    HOMEPAGE_URL "https://github.com/Zheng-Bote/get_poi-osm"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(GET_POI_OSM_BUILD_CLI "Build get_poi-osm CLI executable" ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network)

add_library(get_poi-osm SHARED
    src/PoiOsm.cpp
    include/PoiOsm.hpp
)

target_link_libraries(get_poi-osm
    PUBLIC
        Qt6::Core
        Qt6::Network
)

target_include_directories(get_poi-osm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(get_poi-osm PUBLIC cxx_std_23)

if(GET_POI_OSM_BUILD_CLI)
    add_executable(get_poi-osm-cli
        src/main.cpp
    )
    target_link_libraries(get_poi-osm-cli PRIVATE get_poi-osm)
endif()

include(GNUInstallDirs)

install(TARGETS get_poi-osm
    EXPORT get_poi-osmTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT get_poi-osmTargets
    NAMESPACE get_poi-osm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/get_poi-osm
    FILE "get_poi-osmTargets.cmake"
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/get_poi-osmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/get_poi-osmConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/get_poi-osm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/get_poi-osmConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/get_poi-osm
)

install(FILES
    schema/poi-osm-schema-v1.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/get_poi-osm
)
