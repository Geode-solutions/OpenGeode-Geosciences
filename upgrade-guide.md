# Upgrade Guide

## Upgrading from OpenGeode v3.x.x to v4.0.0

### Motivations

This new major release is to formalize PyPI support for OpenGeode.

### Breaking Changes

- **CMake**: CMake minimum requirement has been upgraded to 3.14 to ease import of PyPI modules.


## Upgrading from OpenGeode-Geosciences v2.x.x to v3.0.0

### Motivations

Homogenize CMake project and macro names with repository name.

### Breaking Changes

- Replace `OpenGeode_Geosciences` by `OpenGeode-Geosciences`. For example:
`OpenGeode_Geosciences::geosciences` is replaced by `OpenGeode-Geosciences::geosciences`.


## Upgrading from OpenGeode-Geosciences v1.x.x to v2.0.0

### Motivations

Following the same organization principle than OpenGeode v2.

### Breaking Changes

- Files have been reorganized by adding two folders: mixin and representation.

