<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Changelog](#changelog)
  - [[1.1.0] - 2026-02-15](#110---2026-02-15)
    - [Added](#added)
    - [Changed](#changed)
  - [[1.0.0] - 2026-02-14](#100---2026-02-14)
    - [Added](#added-1)
  - [[0.1.0] - 2026-02-11](#010---2026-02-11)
    - [Added](#added-2)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-02-15

### Added

- GitHub Actions workflow for quality and security checks (`quality_security.yml`).
- Updated project configuration to version 1.1.0.

### Changed

- Improved CI/CD pipeline stability.

## [1.0.0] - 2026-02-14

### Added

- Initial stable release of `qt_get_poi-osm`.
- Core library functionality: `PoiOsmClient` class.
- CLI tool: `qt_get_poi-osm-cli`.
- Support for querying POIs by coordinates and address.
- Whitelist filtering support.
- JSON output with schema version 1.
- CMake FetchContent integration support.
- Installation and packaging configuration.

## [0.1.0] - 2026-02-11

### Added

- Initial project structure.
- Basic implementation of POI querying logic.
- CMake build system setup.
