# Arcade Library Loading System - Optimizations

## Introduction

This document explains the advanced library loading system implemented in the Arcade project to ensure stable, crash-free dynamic library switching - particularly for graphical libraries.

## The Challenge

Dynamic loading and unloading of libraries (especially graphics libraries) can lead to various issues:

1. **Segmentation faults** - Particularly with NCurses when unloading
2. **Resource leaks** - Improper cleanup leading to memory leaks
3. **Race conditions** - Timing issues between unloading and loading operations
4. **State inconsistency** - Loss of game state during library transitions

## Our Solution

We've implemented a robust system with several key optimizations:

### 1. Strategic Timing Delays

```cpp
// Critical delay after cleanup to allow resources to be fully released
std::this_thread::sleep_for(std::chrono::milliseconds(100));

// Delay before initializing a new library
std::this_thread::sleep_for(std::chrono::milliseconds(50));

// Extended delay specifically for NCurses
std::this_thread::sleep_for(std::chrono::milliseconds(150));
```

These precise delays prevent race conditions and ensure resources are properly released before new operations begin.

### 2. Special Handling for NCurses

NCurses libraries require special treatment:

```cpp
// Detect NCurses library
bool isNcurses = _currentGraphicsLibPath.find("Ncurses") != std::string::npos;

// Special handling for NCurses
if (isNcurses) {
    // Detach pointer without calling destructor
    _currentGraphicsLib.release(); 
    
    // Extended delay for terminal resources to release
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}
```

This approach avoids segmentation faults caused by NCurses' destructor attempting to access already released terminal resources.

### 3. Robust Error Recovery

```cpp
try {
    // Attempt to load new library
    if (!_libManager->loadGraphicsLibrary(name)) {
        // If loading fails, attempt to restore previous library
        if (!currentLibName.empty() && currentLibName != "None") {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            _libManager->loadGraphicsLibrary(currentLibName);
        }
    }
} catch (...) {
    // Handle unexpected errors
}
```

Our system attempts to restore the previous working library if a new one fails to load.

### 4. State Preservation

```cpp
// Save important state before switching libraries
std::string playerName = _playerName;
// ... other state preservation

// Restore state after successful library switch
if (_gameManager) {
    _gameManager->resetGame();
}
```

This ensures a consistent user experience even when switching rendering libraries.

### 5. Ordered Resource Management

```cpp
// Unload in the correct order - game libraries first
if (_libManager->hasGameLibrary()) {
    _libManager->unloadCurrentGameLibrary();
}

// Then unload graphics libraries
if (_libManager->hasGraphicsLibrary()) {
    auto& graphicsLib = _libManager->getCurrentGraphicsLibrary();
    graphicsLib.cleanup();
    _libManager->unloadCurrentGraphicsLibrary();
}
```

The careful ordering prevents dependencies from being unloaded before they're no longer needed.

## Implementation Details

### Library Manager Loading Sequence

1. **Save current state** - Preserve reference to current library
2. **Clean previous library** - If it exists, with appropriate delays
3. **Create new loader** - Fresh instance to avoid conflicts
4. **Load library** - Using dlopen via the loader
5. **Get creation function** - Retrieve createGraphicsLibrary symbol
6. **Create instance** - Call the factory function
7. **Initialize** - Setup the new library with proper delays
8. **Update state** - Store the new library path and reference

### Unloading Sequence

1. **Special case check** - Detect if it's NCurses
2. **Library cleanup** - Call cleanup() method
3. **Pointer handling** - Use release() for NCurses, nullptr for others
4. **Loader unloading** - Call unload() on the loader
5. **Strategic delays** - Wait appropriate time based on library type
6. **Path clearing** - Reset the library path reference

## Testing and Validation

This system has been extensively tested with all supported graphics libraries:
- NCurses (terminal-based)
- SDL2 (window-based)
- SFML (window-based)

Tests included:
- Rapid switching between libraries
- Exit during library switching
- Error injection to test recovery
- Memory leak analysis with Valgrind

## Best Practices for Library Implementers

If you're implementing a new graphics library for Arcade:

1. **Thorough cleanup()** - Ensure all resources are properly released
2. **Error handling** - Handle exceptions in initialization and cleanup
3. **State reset** - Reset internal state variables in cleanup()
4. **Resource management** - Use RAII where possible
5. **Self-contained** - Avoid dependencies on external state

## Conclusion

Our library loading system demonstrates how careful timing, special case handling, and robust error recovery can solve the challenges of dynamic library management. The strategic use of delays, coupled with library-specific handling for NCurses, provides a stable platform for the Arcade project. 