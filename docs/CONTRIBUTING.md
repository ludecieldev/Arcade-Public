# Contributing to the Arcade Project

This document provides guidelines and information for contributing to the Arcade project, whether you're fixing bugs, adding features, or creating new libraries.

## Table of Contents
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Adding Graphics Libraries](#adding-graphics-libraries)
- [Adding Game Libraries](#adding-game-libraries)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)

## Getting Started

### Prerequisites

Ensure you have all the required dependencies installed as detailed in the [Building documentation](BUILDING.md).

### Setting Up the Development Environment

1. Clone the repository:
```bash
git clone https://github.com/your-username/Arcade-Public.git
cd Arcade-Public
```

2. Build the project to ensure everything works:
```bash
make
```

## Development Workflow

1. Create a branch for your work:
```bash
git checkout -b feature/your-feature-name
```

2. Make your changes following the coding standards
3. Build and test your changes
4. Commit your changes with descriptive messages
5. Push your branch and submit a pull request

## Coding Standards

### General Guidelines

- Follow a consistent coding style
- Write clear, meaningful comments
- Use descriptive variable and function names
- Keep functions small and focused on a single task
- Write modular, reusable code

### C++ Specific Standards

- Use C++17 features when appropriate
- Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with these exceptions:
  - Use snake_case for function names and variables
  - Use PascalCase for class names
  - Use camelCase for method names
- Use smart pointers instead of raw pointers where possible
- Use const correctness
- Prefer STL containers and algorithms

### Header Files

- Use header guards:
```cpp
#ifndef FILE_NAME_HPP_
#define FILE_NAME_HPP_

// Code here

#endif // FILE_NAME_HPP_
```

- Include only necessary headers
- Forward declare classes when possible

### Error Handling

- Use exceptions for exceptional conditions
- Return error codes for expected failure cases
- Document error handling behavior

## Adding Graphics Libraries

To add a new graphics library to the Arcade project:

1. Create a new class that implements the `IGraphicsLibrary` interface
2. Follow the file naming convention: `[LibraryName]Graphics.hpp/cpp`
3. Place implementation files in `src/graphics/`
4. Place header files in `include/graphics/`
5. Implement all required methods
6. Add external C functions for dynamic loading
7. Update the Makefile to compile the new library

### Example Header Structure

```cpp
/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** YourLibraryGraphics
*/

#ifndef YOUR_LIBRARY_GRAPHICS_HPP_
#define YOUR_LIBRARY_GRAPHICS_HPP_

#include "../interfaces/IGraphicsLibrary.hpp"
#include <your-library-headers>

namespace arcd {

class YourLibraryGraphics : public IGraphicsLibrary {
private:
    // Private members and methods
    
public:
    YourLibraryGraphics();
    ~YourLibraryGraphics() override;
    
    // IGraphicsLibrary implementation
    bool initialize() override;
    void cleanup() override;
    void clear() override;
    void refresh() override;
    void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
    void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
    void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;
    int getKey() override;
    void getPlayerName(std::string& playerName) override;
    std::string getName() const override;
    int getWidth() const override;
    int getHeight() const override;
};

}

extern "C" {
    arcd::IGraphicsLibrary* createGraphicsLibrary();
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* graphicsLib);
}

#endif // YOUR_LIBRARY_GRAPHICS_HPP_
```

## Adding Game Libraries

To add a new game to the Arcade project:

1. Create a new class that implements the `IGameLibrary` interface
2. Follow the file naming convention: `[GameName]Game.hpp/cpp`
3. Place implementation files in `src/games/`
4. Place header files in `include/games/`
5. Implement all required methods
6. Add external C functions for dynamic loading
7. Update the Makefile to compile the new game

### Example Header Structure

```cpp
/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** YourGame
*/

#ifndef YOUR_GAME_HPP_
#define YOUR_GAME_HPP_

#include "../interfaces/IGameLibrary.hpp"
#include "../Core.hpp"

namespace arcd {

class YourGame : public IGameLibrary {
private:
    // Private members and methods
    
public:
    YourGame();
    ~YourGame() override;
    
    // IGameLibrary implementation
    bool initialize() override;
    void cleanup() override;
    void update(Core* core) override;
    void render(IGraphicsLibrary* graphicsLib) override;
    void handleInput(int key) override;
    std::string getName() const override;
    std::string getDescription() const override;
    int getScore() const override;
    void resetGame() override;
    bool isGameOver() const override;
};

}

extern "C" {
    arcd::IGameLibrary* createGameLibrary();
    void destroyGameLibrary(arcd::IGameLibrary* gameLib);
}

#endif // YOUR_GAME_HPP_
```

## Testing

### Manual Testing

Before submitting changes, manually test:

1. Building the project
2. Running with different graphics libraries
3. Switching between graphics libraries at runtime
4. Playing each game
5. Switching between games at runtime
6. Edge cases (e.g., window resizing, invalid inputs)

### Writing Tests

If adding automated tests:

1. Place test files in a `tests/` directory
2. Create tests for each component
3. Use a consistent testing framework
4. Include both unit tests and integration tests

## Submitting Changes

1. Ensure your code builds without warnings
2. Run all tests
3. Format your code according to the style guide
4. Create a pull request with a clear description:
   - What problem does it solve?
   - How does it solve the problem?
   - Any notes on implementation or design decisions
   
5. Link any related issues

## Questions?

If you have any questions about contributing, please open an issue or contact the project maintainers. 