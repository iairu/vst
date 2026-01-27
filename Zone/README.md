# Zone VST Plugin

A simple VST3 plugin that generates a sawtooth wave, inspired by ZUN's Touhou soundtracks.

## Build Instructions

To build this plugin, you will need to have CMake and a C++ compiler installed.

1.  Create a build directory:
    ```
    mkdir build
    cd build
    ```

2.  Run CMake to generate the build files:
    ```
    cmake ..
    ```

3.  Build the plugin:
    ```
    cmake --build .
    ```

4.  The VST3 plugin bundle will be created in the `build/VST3` directory. You can then copy the `Zone.vst3` bundle to your VST3 plugins folder.

    - On macOS, this is typically `~/Library/Audio/Plug-Ins/VST3`.
    - On Windows, this is typically `C:\Program Files\Common Files\VST3`.

## Disclaimer

This plugin is a very basic example and is not intended to be a full-featured synthesizer. It is a starting point for creating your own VST plugins. The included VST3 SDK files are a minimal subset of the full SDK and are for demonstration purposes only.
