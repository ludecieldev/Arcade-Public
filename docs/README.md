# Arcade Project Documentation

Welcome to the Arcade project documentation. This collection of documents provides comprehensive information about the Arcade project architecture, components, and how to use or extend them.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](ARCHITECTURE.md)
3. [Core Engine](CORE.md)
4. [Graphics Libraries](GRAPHICS_LIBS.md)
5. [Game Libraries](GAME_LIBS.md)
6. [Interfaces](INTERFACES.md)
7. [Building and Running](BUILDING.md)
8. [Contributing](CONTRIBUTING.md)
9. [API Reference](API_REFERENCE.md)

## Project Overview

The Arcade project is a modular gaming platform that allows different games to be played with different graphical rendering libraries. The platform is designed to be extensible, with a clear separation of concerns between the core engine, graphical displays, and game logic.

### Key Features

- **Dynamic Library Loading**: Games and graphics libraries can be loaded at runtime.
- **Switchable Graphics**: Change the graphical rendering library without restarting the game.
- **Switchable Games**: Change the current game without restarting the application.
- **Modular Architecture**: Clearly defined interfaces for each component.
- **Separation of Concerns**: Graphics libraries handle only rendering, games handle only logic.
- **Clean Communication**: Core acts as mediator between graphics and game libraries.
- **Smart Memory Management**: Modern C++ practices with smart pointers.
- **Cross-Platform**: Works on various operating systems with minimal dependencies.

### Project Structure

- **Core**: The central engine that coordinates between games and graphics libraries.
- **Graphics Libraries**: Libraries implementing the `IGraphicsLibrary` interface for rendering.
- **Game Libraries**: Libraries implementing the `IGameLibrary` interface with game logic.
- **Interfaces**: Shared interfaces and utility classes.

### Separation of Concerns

A key feature of the Arcade project is its strict separation of responsibilities:

- **Graphics Libraries** (SFML, SDL2, Ncurses, etc.)
  - Handle only visual rendering and user input capture
  - Pass captured events to the Core without interpretation
  - Render game state provided by the Core
  - Contain no game logic

- **Game Libraries** (Snake, etc.)
  - Implement game rules and logic
  - Process input events passed from the Core
  - Maintain and update game state
  - Contain no rendering code

- **Core**
  - Acts as mediator between graphics and game libraries
  - Routes input events from graphics to games
  - Passes game state from games to graphics for rendering
  - Manages transitions between states (menu, gameplay, etc.)

## Getting Started

To start using and understanding the Arcade project, we recommend beginning with the [Architecture Overview](ARCHITECTURE.md) document to get a high-level understanding of how the system works, then dive into specific components based on your interests or needs.

For developers who want to run the project immediately, please refer to the [Building and Running](BUILDING.md) guide. 