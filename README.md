# qt_get_poi-osm

A command‑line tool and shared library for querying OpenStreetMap POIs around a coordinate or address. \
Supports whitelisting, language‑aware POI names, JSON output, and CMake FetchContent integration.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)]()
![QT](https://img.shields.io/badge/Community-6-41CD52?logo=qt)
[![CMake](https://img.shields.io/badge/CMake-3.23+-blue.svg)]()

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Zheng-Bote/qt_get_poi-osm?logo=GitHub)](https://github.com/Zheng-Bote/qt_get_poi-osm/releases)

[Report Issue](https://github.com/Zheng-Bote/qt_get_poi-osm/issues) · [Request Feature](https://github.com/Zheng-Bote/qt_get_poi-osm/pulls)

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Description](#description)
- [Features](#features)
- [Library Shortcut / Developer Introduction](#library-shortcut--developer-introduction)
  - [What the library gives you](#what-the-library-gives-you)
  - [Why it’s easy to integrate](#why-its-easy-to-integrate)
  - [Typical usage pattern](#typical-usage-pattern)
  - [When to use this library](#when-to-use-this-library)
- [Pre‑Requisites](#pre%E2%80%91requisites)
- [Dependencies](#dependencies)
- [Build](#build)
- [Install](#install)
- [Usage (CLI)](#usage-cli)
  - [Query by coordinates](#query-by-coordinates)
  - [Query by address](#query-by-address)
  - [Whitelist examples](#whitelist-examples)
    - [All tourism POIs](#all-tourism-pois)
    - [Only viewpoints](#only-viewpoints)
    - [Viewpoints OR theme parks](#viewpoints-or-theme-parks)
    - [Restaurants only](#restaurants-only)
    - [Tourism + Restaurants](#tourism--restaurants)
- [JSON Output Schema](#json-output-schema)
- [Using the Library via CMake FetchContent](#using-the-library-via-cmake-fetchcontent)
  - [1. Add FetchContent to your project](#1-add-fetchcontent-to-your-project)
  - [2. Link against the library](#2-link-against-the-library)
- [Example Program Using the Library](#example-program-using-the-library)
- [How to use the library in multi‑threaded applications](#how-to-use-the-library-in-multi%E2%80%91threaded-applications)
  - [One client per thread (recommended)](#one-client-per-thread-recommended)
  - [Use Qt threads (QThread)](#use-qt-threads-qthread)
- [Build with CMake](#build-with-cmake)
- [License](#license)
- [Author](#author)
  - [Code Contributors](#code-contributors)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Description

A lightweight C++23 / Qt6 command‑line tool and shared library for querying OpenStreetMap POIs (Points of Interest) around a coordinate or address.
Supports whitelisting, language‑aware POI names, JSON output, and CMake FetchContent integration.

## Features

- Query POIs within a configurable radius (default: 100 km)
- Input via latitude/longitude or free‑form address (Nominatim geocoding)
- Overpass API backend
- Optional whitelist filters (e.g. tourism, tourism=viewpoint, amenity=restaurant)
- English‑preferred POI names (name:en fallback)
- JSON output following a versioned schema (schema_version = 1)
- Installable shared library + CLI tool
- FetchContent‑friendly CMake package

> [!NOTE]
> There is also a version without Qt6 of this project: [get_poi-osm](https://github.com/Zheng-Bote/get_poi-osm).

## Library Shortcut / Developer Introduction

**qt_get_poi-osm** is a lightweight C++23 / Qt6 library designed to make OpenStreetMap POI discovery effortless.

It wraps Nominatim (geocoding) and Overpass (POI queries) behind a clean, asynchronous Qt interface and returns structured, versioned JSON results.

If you need to fetch points of interest around a coordinate or address — restaurants, viewpoints, theme parks, museums, shops, or any other OSM‑tagged feature — this library gives you a simple, modern API that works out of the box.

### What the library gives you

- A single class: `PoiOsmClient`
- Asynchronous Qt signal: `finished(const PoiResult&)`
- Input:
  - latitude/longitude
  - or free‑form address (auto‑geocoded)
- Optional **whitelist filters**:
  - tourism
  - tourism=viewpoint
  - amenity=restaurant
  - any OSM key or key=value pair
- Output:
  - A stable, versioned JSON object (schema_version = 1)
  - Includes query metadata, resolved coordinates, and all matching POIs
  - English‑preferred POI names (name:en fallback)

### Why it’s easy to integrate

- Distributed as an installable shared library
- Fully FetchContent‑compatible
- Requires only Qt6 Core + Network
- No external dependencies beyond OSM APIs
- Clean header‑only interface for consumers (PoiOsm.hpp)

### Typical usage pattern

```cpp
QNetworkAccessManager nam;
PoiOsmClient client(&nam);

QObject::connect(&client, &PoiOsmClient::finished,
                 [&](const PoiResult& res) {
    qDebug() << QJsonDocument(res.json).toJson();
});

client.queryByCoordinates(48.13743, 11.57549, 100000,
                          {{"tourism", "viewpoint"}});
```

This example fetches all viewpoints within 100 km of Munich.

### When to use this library

- You’re building a CLI tool, desktop app, or service that needs OSM POIs
- You want a simple Qt‑native API instead of manually crafting Overpass queries
- You need structured JSON output for downstream processing
- You want a stable, versioned schema for long‑term compatibility

## Pre‑Requisites

- C++23 compiler
- GCC ≥ 12
- Clang ≥ 15
- MSVC ≥ 19.36
- Qt 6.5+
  - Required modules:
    - Qt6::Core
    - Qt6::Network
- CMake ≥ 3.23
- Internet access (Nominatim + Overpass API)

## Dependencies

**Runtime**

- OpenStreetMap Nominatim (geocoding)
- Overpass API (POI queries)

**Build‑time**

- Qt6 Core + Network
- CMake ≥ 3.23

## Build

```bash
cmake -B build -S .
cmake --build build -j$(nproc)
```

This builds:

- Shared library: libqt_get_poi-osm.so (Linux)
- CLI tool: qt_get_poi-osm-cli

## Install

```bash
sudo cmake --install build
```

Installs:

```bash
/usr/local/bin/qt_get_poi-osm-cli
/usr/local/lib/libqt_get_poi-osm.so
/usr/local/include/PoiOsm.hpp
/usr/local/share/qt_get_poi-osm/qt_get_poi-osm-schema-v1.json
/usr/local/lib/cmake/qt_get_poi-osm/
```

## Usage (CLI)

### Query by coordinates

```bash
qt_get_poi-osm-cli --lat 48.13743 --lon 11.57549
```

### Query by address

```bash
qt_get_poi-osm-cli --address "Marienplatz, Munich"
```

### Whitelist examples

#### All tourism POIs

```bash
qt_get_poi-osm-cli --lat 48.13743 --lon 11.57549 --whitelist tourism
```

#### Only viewpoints

```bash
qt_get_poi-osm-cli --lat 48.13743 --lon 11.57549 --whitelist tourism=viewpoint
```

#### Viewpoints OR theme parks

```bash
qt_get_poi-osm-cli \
  --lat 48.13743 --lon 11.57549 \
  --whitelist tourism=viewpoint \
  --whitelist tourism=theme_park
```

#### Restaurants only

```bash
qt_get_poi-osm-cli --lat 48.13743 --lon 11.57549 --whitelist amenity=restaurant
```

#### Tourism + Restaurants

```bash
qt_get_poi-osm-cli \
  --lat 48.13743 --lon 11.57549 \
  --whitelist tourism \
  --whitelist amenity=restaurant
```

## JSON Output Schema

The tool outputs a versioned JSON structure (schema_version = 1).
The full schema is installed under:

```bash
/usr/local/share/qt_get_poi-osm/qt_get_poi-osm-schema-v1.json
```

## Using the Library via CMake FetchContent

### 1. Add FetchContent to your project

```bash
include(FetchContent)

FetchContent_Declare(
    qt_get_poi_osm
    GIT_REPOSITORY https://github.com/Zheng-Bote/qt_get_poi-osm.git
    GIT_TAG main
)

FetchContent_MakeAvailable(qt_get_poi_osm)
```

### 2. Link against the library

```bash
add_executable(mytool main.cpp)
target_link_libraries(mytool PRIVATE qt_get_poi-osm::qt_get_poi-osm)
```

## Example Program Using the Library

```cpp
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QTextStream>

#include <PoiOsm.hpp>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QNetworkAccessManager nam;
    PoiOsmClient client(&nam);

    QObject::connect(&client, &PoiOsmClient::finished,
                     [&](const PoiResult& res) {
        QTextStream out(stdout);
        out << QJsonDocument(res.json).toJson(QJsonDocument::Indented);
        out.flush();
        QCoreApplication::quit();
    });

    QList<PoiWhitelistEntry> whitelist;
    whitelist.push_back({"tourism", "viewpoint"});

    // query by coordinates, where radius is in meters
    client.queryByCoordinates(48.13743, 11.57549, 100000, whitelist);

    return app.exec();
}
```

## How to use the library in multi‑threaded applications

### One client per thread (recommended)

```cpp
std::thread worker([] {
    QCoreApplication app(argc, argv);

    QNetworkAccessManager nam;
    PoiOsmClient client(&nam);

    QObject::connect(&client, &PoiOsmClient::finished, [&](const PoiResult& r){
        // handle result
        app.quit();
    });

    client.queryByCoordinates(...);

    app.exec();
});
```

Each thread has:

- its own event loop
- its own QNetworkAccessManager
- its own PoiOsmClient

This is fully safe.

### Use Qt threads (QThread)

```cpp
QThread* thread = new QThread;
PoiOsmClient* client = new PoiOsmClient(new QNetworkAccessManager);

client->moveToThread(thread);

QObject::connect(thread, &QThread::started, [client] {
    client->queryByCoordinates(...);
});

thread->start();
```

Qt handles the thread affinity correctly.

## Build with CMake

```bash
cmake_minimum_required(VERSION 3.21)
project(mytool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)

include(FetchContent)

FetchContent_Declare(
    qt_get_poi_osm
    GIT_REPOSITORY https://github.com/Zheng-Bote/qt_get_poi-osm.git
    GIT_TAG main
)

FetchContent_MakeAvailable(qt_get_poi_osm)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(mytool main.cpp)
target_link_libraries(mytool PRIVATE Qt6::Core Qt6::Network qt_get_poi-osm::qt_get_poi-osm)
```

---

## License

Distributed under the MIT License. See LICENSE for more information.

Copyright (c) 2026 ZHENG Robert

## Author

[![Zheng Robert - Core Development](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

### Code Contributors

![Contributors](https://img.shields.io/github/contributors/Zheng-Bote/qt_get_poi-osm?color=dark-green)

---

**Happy coding! 🚀** :vulcan_salute:
