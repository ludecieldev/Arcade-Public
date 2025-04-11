# Arcade Project Architecture

This document provides an overview of the Arcade project's architecture, explaining how different components interact with each other.

## High-Level Architecture

The Arcade project is built around a modular architecture with three main components, each with well-defined responsibilities:

1. **Core Engine**: The central component that manages the application lifecycle, coordinates between game and graphics libraries, and handles transitions between states (menu, game, etc.).
2. **Graphics Libraries**: Dynamically loaded libraries that handle only visual rendering and user input capture.
3. **Game Libraries**: Dynamically loaded libraries that contain only game logic and entity definitions.

## Separation of Concerns

### Graphics Libraries

Graphics libraries have the following responsibilities:
- Manage initialization and cleanup of graphical resources
- Capture user inputs and transmit them to the Core without interpretation
- Render user interface elements (menus, scores, etc.)
- Render the game state provided by the Core
- Contain **no** game logic

Implementation examples: SFML, SDL2, Ncurses

### Game Libraries

Game libraries have the following responsibilities:
- Implement all game logic (rules, mechanics, etc.)
- Update the game state based on user inputs
- Provide a generic game state that can be rendered by any graphics library
- Contain **no** specific rendering logic

Implementation examples: Snake, Nibbler

### Core

The Core has the following responsibilities:
- Load and manage dynamic libraries
- Serve as a mediator between graphics libraries and games
- Manage transitions between different states (menu, game, game selection, etc.)
- Transfer events from graphics libraries to games
- Retrieve the game state and pass it to the graphics library for rendering
- Manage score persistence and other data

## Data and Event Flow

### Input Capture and Processing
1. The active graphics library captures user inputs (keyboard, mouse)
2. It converts these inputs into generic events (EventType, KeyCode)
3. The Core retrieves these events and determines where to send them based on current state
4. If the event is intended for the game, the Core transmits it to the active game library
5. The game interprets the event and updates its internal state

### Update and Rendering
1. The Core calls the game's update method with elapsed time (deltaTime)
2. The game updates its internal state (entity positions, scores, etc.)
3. The Core requests the current game state as a GameState object
4. The Core passes this state to the graphics library
5. The graphics library visually renders this state without additional interpretation

## Dynamic Loading System

The Arcade project uses dynamic library loading at runtime:

- Libraries are compiled as shared objects (.so files)
- Each library must implement specific entry points for creation and destruction
- The Core uses LibraryManager to load and manage libraries
- Each library is instantiated via its exposed createGraphicsLibrary/createGameLibrary functions

## Interface-Based Design

Communication between components follows an interface-based approach:

- `IGraphicsLibrary`: Interface that all graphics libraries must implement
- `IGameLibrary`: Interface that all game libraries must implement
- `IGameState`: Interface for representing game state in a generic way
- `IEvent`: Interface for representing input events in a generic way

This design enables:
- Clear separation of concerns
- Easy interchangeability of components
- Consistent API regardless of implementation

## Error Handling

The architecture includes robust error handling:
- Library loading errors are caught and reported
- Runtime errors in libraries are isolated when possible
- Resource management follows RAII principles (Resource Acquisition Is Initialization)
- Checks are performed to avoid null pointers and invalid memory access

## Next Steps

For more detailed information on specific components:
- [Core Engine Documentation](CORE.md)
- [Graphics Libraries Documentation](GRAPHICS_LIBS.md)
- [Game Libraries Documentation](GAME_LIBS.md)
- [Interfaces Documentation](INTERFACES.md) 