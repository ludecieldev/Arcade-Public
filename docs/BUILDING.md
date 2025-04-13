# Building and Running the Arcade Project

This guide provides detailed instructions for building and running the Arcade project, including prerequisites, build options, and troubleshooting tips.

## Prerequisites

Before building the Arcade project, ensure you have the following dependencies installed:

### Required Tools

- GCC/G++ 8.3.0 or newer (with C++17 support)
- GNU Make 4.2.1 or newer
- CMake 3.10 or newer (optional, for some graphics libraries)

### Required Libraries

For the core application:
- libdl (Dynamic Loading Library)

For the NCurses graphics library:
- libncurses5-dev

For the SDL2 graphics library:
- libsdl2-dev
- libsdl2-ttf-dev
- libsdl2-image-dev

For the Allegro5 graphics library:
- liballegro5-dev
- liballegro-ttf5-dev
- liballegro-primitives5-dev

### Installing Dependencies on Debian/Ubuntu

```bash
# Core dependencies
sudo apt-get update
sudo apt-get install build-essential libdl-dev cmake

# NCurses
sudo apt-get install libncurses5-dev

# SDL2
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

# Allegro5
sudo apt-get install liballegro5-dev liballegro-ttf5-dev liballegro-primitives5-dev
```

### Installing Dependencies on Arch/Manjaro

```bash
# Core dependencies
sudo pacman -S base-devel

# NCurses
sudo pacman -S ncurses

# SDL2
sudo pacman -S sdl2 sdl2_ttf sdl2_image

# Allegro5
sudo pacman -S allegro5 allegro-ttf5 allegro-primitives5
```

## Building the Project

### Standard Build

To build the entire project with all available graphics and game libraries:

```bash
make
```

This will compile:
- The core application (`arcade`)
- All available graphics libraries (in `lib/`)
- All available game libraries (in `lib/`)

### Partial Build Options

To build only specific components:

```bash
# Build only the core application
make arcade

# Build only graphics libraries
make graphics_libs

# Build only game libraries
make game_libs

# Build a specific graphics library (e.g., Allegro5)
make lib/arcade_Allegro5Graphics.so

# Build a specific game library (e.g., Snake)
make lib/arcade_SnakeGame.so
```

### Cleaning the Build

To clean the build artifacts:

```bash
# Remove object files
make clean

# Remove object files and executables/libraries
make fclean

# Rebuild everything from scratch
make re
```

## Running the Arcade Project

### Basic Usage

To start the Arcade application with the default graphics library:

```bash
./arcade
```

### Specifying a Graphics Library

To start with a specific graphics library:

```bash
./arcade lib/arcade_NcursesGraphics.so
./arcade lib/arcade_SDL2Graphics.so
./arcade lib/arcade_Allegro5Graphics.so
```

### Command-Line Arguments

The Arcade application accepts the following command-line arguments:

```
Usage: ./arcade [graphics_library_path]
  graphics_library_path    Path to a graphics library to load at startup
```

## Controls

While running the application, you can use the following controls:

### Menu Navigation

- **Up/Down Arrow Keys**: Navigate through menu items
- **Enter**: Select the highlighted menu item
- **Escape**: Return to the previous menu or exit

### In-Game Controls

- **Arrow Keys**: Control the game (movement)
- **Enter**: Confirm/Select
- **Escape**: Pause/Return to menu

### Special Keys

- **7**: Switch to the next game library
- **9**: Switch to the next graphics library

## Directory Structure

- `src/`: Source code for the core application and libraries
- `include/`: Header files
- `lib/`: Compiled library files (.so)
- `assets/`: Game assets (images, fonts, etc.)
- `docs/`: Documentation

## Troubleshooting

### Common Build Issues

#### Missing Libraries

If you encounter errors about missing libraries:

```
error: cannot find -lncurses
error: cannot find -lsfml-graphics
```

Ensure you have installed all required dependencies as listed in the Prerequisites section.

#### Compilation Errors

For compilation errors related to C++17 features:

```
error: 'optional' is not a member of 'std'
```

Make sure you're using a compiler that supports C++17 features.

### Runtime Issues

#### Library Loading Failures

If you encounter errors like:

```
Error: Could not load graphics library: <library_path>
dlopen: <library_path>: cannot open shared object file: No such file or directory
```

Check:
1. The library path is correct
2. The library exists and has the proper permissions
3. The library can find its dependencies (use `ldd` to check)

#### Graphics-Specific Issues

For graphics-related issues:

- **NCurses**: Ensure your terminal supports color and has the right size
- **SDL2**: Check if required SDL2 libraries are in the path
- **Allegro5**: Verify Allegro5 is installed

## Next Steps

For more information on other aspects of the project:
- [Core Engine Documentation](CORE.md)
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Game Libraries Documentation](GAME_LIBS.md) 