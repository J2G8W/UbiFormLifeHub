# UbiForm Life Hub
> An application which demonstrates the possibilities of the [UbiForm platform](https://github.com/J2G8W/UbiForm).

The repository provides code for a simple application which acts general displayer of information with the [UbiForm Middleware Platform](https://github.com/J2G8W/UbiForm).
It is especially designed to work with the [UbiForm skeleton Key](https://github.com/J2G8W/UbiFormSkeletonKey).
It is built with QT and presents a simple non-interactive GUI to be used with a simple screen only setup. By its nature it is flexible and displays different information patterns.
It is a part of my PartII Computer Science project at the University of Cambridge.

## Installation
Add the UbiForm library to the path libs/UbiForm (can be done with a soft link if UbiForm already exists elsewhere on the system).
It requires the CMake build system to be installed (available for all common platforms) and can optionally use the Ninja build system for further cross-platform usability.

It also requires the presence of QT within the setup.

Linux (starting from within the repo):
```
mkdir build
cd build
cmake -G Ninja ..
ninja
```

## Usage example
[Usage Example](https://user-images.githubusercontent.com/44195342/113888545-5888be80-97ba-11eb-90e5-eb6c35ffff5b.mp4
)

## Release History

* 0.1.0
    * The first proper release
* 0.0.1
    * Work in progress

## Meta
[Julian Wreford](https://www.linkedin.com/in/julian-wreford-986b34154/) – julian \[at\] ashleywreford \[dot\] com

Distributed under the MIT license. See ``LICENSE`` for more information.
