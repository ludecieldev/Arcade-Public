# Arcade Project Architecture

This document provides an overview of the Arcade project's architecture, explaining how different components interact with each other.

## High-Level Architecture

The Arcade project is built around a modular architecture with three main components:

1. **Core Engine**: The central component that manages the application lifecycle and coordinates between games and graphics libraries.
2. **Graphics Libraries**: Dynamically loaded libraries that handle rendering and user input.
3. **Game Libraries**: Dynamically loaded libraries that contain game logic and definitions.

![Arcade Architecture Diagram](architecture-diagram.png)

## Component Interactions

### Loading Process

1. The application starts and the Core engine initializes.
2. The Core loads the specified (or default) graphics library.
3. The Core loads the specified (or default) game library.
4. The Core initializes both libraries and enters the main loop.

### Main Loop

During the main application loop:

1. The Core processes any system events (like switching libraries).
2. The Core passes user input from the graphics library to the active game.
3. The game updates its state based on input and elapsed time.
4. The game sends drawing commands to the graphics library through the Core.
5. The graphics library renders the frame.
6. The loop repeats until exit.

## Dynamic Library System

The Arcade project uses dynamic loading of libraries at runtime:

- Libraries are compiled as shared objects (.so files)
- Each library must implement specific entry points for creation and destruction
- The Core uses `dlopen()` and related functions to load and interact with libraries

## Interface-Based Design

Communication between components follows an interface-based approach:

- `IGraphicsLibrary`: Interface that all graphics libraries must implement
- `IGameLibrary`: Interface that all game libraries must implement
- Core communicates with libraries only through these interfaces

This design enables:
- Clear separation of concerns
- Easy swapping of components
- Consistent API regardless of implementation

## Directory Structure

```
arcade/
├── include/            # Interface and header files
│   ├── interfaces/     # Core interfaces (IGraphicsLibrary, IGameLibrary)
│   ├── graphics/       # Graphics library headers
│   └── games/          # Game library headers
├── src/                # Implementation files
│   ├── core/           # Core engine implementation
│   ├── graphics/       # Graphics library implementations
│   └── games/          # Game library implementations
├── lib/                # Compiled library outputs
├── assets/             # Game and graphics assets
└── docs/               # Documentation
```

## Event Flow

Events flow through the system as follows:

1. User inputs are captured by the graphics library
2. The graphics library passes events to the Core
3. The Core processes system-level events (switching libraries)
4. Gameplay events are passed to the active game
5. The game updates its state
6. The game instructs the graphics library what to render

## Error Handling

The architecture includes robust error handling:
- Library loading errors are caught and reported
- Runtime errors in libraries are isolated when possible
- Resource management follows RAII principles

## Next Steps

For more detailed information on specific components:
- [Core Engine Documentation](CORE.md)
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Game Libraries Documentation](GAME_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 