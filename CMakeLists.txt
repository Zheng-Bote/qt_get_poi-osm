cmake_minimum_required(VERSION 3.23)

project(qt_get_poi-osm 
    VERSION 1.1.0
    DESCRIPTION "Get POIs from OpenStreetMap"
    HOMEPAGE_URL "https://github.com/Zheng-Bote/qt_get_poi-osm"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(QT_GET_POI_OSM_BUILD_CLI "Build qt_get_poi-osm CLI executable" ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network)

add_library(qt_get_poi-osm SHARED
    src/PoiOsm.cpp
    include/PoiOsm.hpp
)

target_link_libraries(qt_get_poi-osm
    PUBLIC
        Qt6::Core
        Qt6::Network
)

target_include_directories(qt_get_poi-osm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(qt_get_poi-osm PUBLIC cxx_std_23)

if(QT_GET_POI_OSM_BUILD_CLI)
    add_executable(qt_get_poi-osm-cli
        src/main.cpp
    )
    target_link_libraries(qt_get_poi-osm-cli PRIVATE qt_get_poi-osm)
endif()

include(GNUInstallDirs)

install(TARGETS qt_get_poi-osm
    EXPORT qt_get_poi-osmTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT qt_get_poi-osmTargets
    NAMESPACE qt_get_poi-osm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qt_get_poi-osm
    FILE "qt_get_poi-osmTargets.cmake"
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/qt_get_poi-osmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/qt_get_poi-osmConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qt_get_poi-osm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qt_get_poi-osmConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qt_get_poi-osm
)

install(FILES
    schema/qt_get_poi-osm-schema-v1.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/qt_get_poi-osm
)
