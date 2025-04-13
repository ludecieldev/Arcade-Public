#include "games/SnakeGame.hpp"
#include "interfaces/IGraphicsLibrary.hpp"
#include <algorithm>

namespace arcd {

SnakeGame::SnakeGame()
    : _gameOver(false)
    , _exitRequested(false)
    , _score(0)
    , _direction(Direction::RIGHT)
    , _nextDirection(Direction::RIGHT)
    , _rng(std::random_device{}())
{
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
    initialize();
}

void SnakeGame::initialize()
{
    // Reset game state
    _gameOver = false;
    _exitRequested = false;
    _score = 0;
    _direction = Direction::RIGHT;
    _nextDirection = Direction::RIGHT;
    _snake.clear();
    
    // Create initial snake in the middle of the board
    int startX = BOARD_WIDTH / 2 - INITIAL_SNAKE_LENGTH;
    int startY = BOARD_HEIGHT / 2;
    
    // Create the snake extending to the left (since we'll move right)
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        _snake.push_front({startX + i, startY}); // Snake starts moving right
    }
    
    // Spawn initial food
    spawnFood();
    
    // Reset timer
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

void SnakeGame::update()
{
    if (_gameOver) {
        return;
    }
    
    // Check if enough time has passed since the last update
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - _lastUpdateTime).count();
    
    if (elapsedTime < UPDATE_INTERVAL_MS) {
        return; // Not time to update yet
    }
    
    // Update the direction
    _direction = _nextDirection;
    
    // Move the snake
    moveSnake();
    
    // Check for food collision
    if (_snake.front().x == _food.x && _snake.front().y == _food.y) {
        // Don't remove the tail segment, effectively growing the snake
        _score += 10;
        spawnFood();
    } else {
        // Remove the tail segment to maintain the same length
        _snake.pop_back();
    }
    
    // Check for collisions with walls and self
    if (checkCollision(_snake.front())) {
        _gameOver = true;
    }
    
    // Reset timer
    _lastUpdateTime = currentTime;
}

void SnakeGame::handleInput(int key)
{
    // Handle restart on 'R' key press regardless of game state
    if (key == 'r' || key == 'R') {
        restart();
        return;
    }
    
    // Dans l'écran de game over, permettre de quitter au menu
    if (_gameOver) {
        if (key == 'm' || key == 'M' || key == 'q' || key == 'Q' || key == IGraphicsLibrary::KEY_ESC_CODE) {
            _exitRequested = true;
        }
        return;
    }
    
    // Handle movement keys
    switch (key) {
        case IGraphicsLibrary::KEY_UP_CODE:
            if (_direction != Direction::DOWN) {
                _nextDirection = Direction::UP;
            }
            break;
        case IGraphicsLibrary::KEY_DOWN_CODE:
            if (_direction != Direction::UP) {
                _nextDirection = Direction::DOWN;
            }
            break;
        case IGraphicsLibrary::KEY_LEFT_CODE:
            if (_direction != Direction::RIGHT) {
                _nextDirection = Direction::LEFT;
            }
            break;
        case IGraphicsLibrary::KEY_RIGHT_CODE:
            if (_direction != Direction::LEFT) {
                _nextDirection = Direction::RIGHT;
            }
            break;
        
        // The following keys are typically handled by the Core, but we implement 
        // them here to ensure the game responds correctly if invoked directly
        case IGraphicsLibrary::KEY_NEXT_LIB_CODE: // '9'
            // Switch to next graphics library (handled by Core)
            break;
            
        case IGraphicsLibrary::KEY_NEXT_GAME_CODE: // '7'
            // Switch to next game (handled by Core)
            break;
            
        case 'e':
        case 'E':
        case IGraphicsLibrary::KEY_ESC_CODE:
            // Exit the game (handled by Core)
            break;
    }
}

void SnakeGame::handleMouseInput(const IGraphicsLibrary::MouseEvent& event)
{
    (void)event;
}

void SnakeGame::render(IGraphicsLibrary& graphicsLib)
{
    // Get the screen dimensions
    int screenWidth = graphicsLib.getWidth();
    int screenHeight = graphicsLib.getHeight();
    
    // Determine cell size based on the graphics library type and window size
    int cellWidth = 2;
    int cellHeight = 1;
    
    // Special handling for Allegro5
    bool isAllegro = (graphicsLib.getName() == "Allegro5");
    
    if (isAllegro) {
        // For Allegro, we need larger cell sizes
        cellWidth = 15;
        cellHeight = 15;
    } else if (screenWidth < 80) { // For very small displays with other libraries
        cellWidth = 1;
    }
    
    // Calculate position to center the board on screen
    int startX = (screenWidth - BOARD_WIDTH * cellWidth) / 2;
    int startY = (screenHeight - BOARD_HEIGHT * cellHeight) / 2;
    
    // For Allegro, adjust the Y position to account for font size differences
    if (isAllegro) {
        startY = (screenHeight - BOARD_HEIGHT * cellHeight) / 3;
    }
    
    // Make sure the board fits on screen
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    
    // Draw the game elements
    drawBoard(graphicsLib, startX, startY, cellWidth, cellHeight);
    drawSnake(graphicsLib, startX, startY, cellWidth, cellHeight);
    drawFood(graphicsLib, startX, startY, cellWidth, cellHeight);
    drawInfo(graphicsLib, startX, startY, cellWidth, cellHeight);
}

void SnakeGame::drawBoard(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    // Draw the border box
    graphicsLib.drawBox(
        startX - 1, 
        startY - 1, 
        BOARD_WIDTH * cellWidth + 2, 
        BOARD_HEIGHT * cellHeight + 2, 
        Color::CYAN
    );
    
    // For SDL2 and other graphics libraries, we'll use a minimal approach
    // for the board background - empty cells are just blank
}

void SnakeGame::drawSnake(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    bool isAllegro = (graphicsLib.getName() == "Allegro5");
    
    if (_snake.empty()) {
        return;
    }
    
    // Draw the snake head
    Position head = _snake.front();
    // Use simpler characters for better compatibility
    std::string headChar = "O";
    
    // For Allegro, we need to adjust the y-coordinate for each segment
    if (isAllegro) {
        graphicsLib.drawText(startX + head.x * cellWidth, startY + head.y * cellHeight, headChar, Color::YELLOW);
        
        // Draw the snake body
        for (size_t i = 1; i < _snake.size(); i++) {
            Position segment = _snake[i];
            std::string bodyChar = "o";
            graphicsLib.drawText(startX + segment.x * cellWidth, startY + segment.y * cellHeight, bodyChar, Color::GREEN);
        }
    } else {
        graphicsLib.drawText(startX + head.x * cellWidth, startY + head.y, headChar, Color::YELLOW);
        
        // Draw the snake body
        for (size_t i = 1; i < _snake.size(); i++) {
            Position segment = _snake[i];
            std::string bodyChar = "o";
            graphicsLib.drawText(startX + segment.x * cellWidth, startY + segment.y, bodyChar, Color::GREEN);
        }
    }
}

void SnakeGame::drawFood(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    bool isAllegro = (graphicsLib.getName() == "Allegro5");
    
    // Use simpler characters for better compatibility
    std::string foodChar = "*";
    
    if (isAllegro) {
        graphicsLib.drawText(startX + _food.x * cellWidth, startY + _food.y * cellHeight, foodChar, Color::RED);
    } else {
        graphicsLib.drawText(startX + _food.x * cellWidth, startY + _food.y, foodChar, Color::RED);
    }
}

void SnakeGame::drawInfo(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    bool isAllegro = (graphicsLib.getName() == "Allegro5");
    
    // Draw a box for the score
    int scoreBoxWidth = 20;
    int scoreBoxX = startX + (BOARD_WIDTH * cellWidth - scoreBoxWidth) / 2;
    int scoreBoxY;
    
    if (isAllegro) {
        // Pour Allegro, position the score above the board with more space
        scoreBoxY = startY - cellHeight * 3;
        graphicsLib.drawBox(scoreBoxX - 1, scoreBoxY, scoreBoxWidth, cellHeight, Color::WHITE);
        
        // Draw the score
        std::string scoreText = "Score: " + std::to_string(_score);
        int scoreX = startX + (BOARD_WIDTH * cellWidth - scoreText.length() * 8) / 2; // Adjust for text width in Allegro
        graphicsLib.drawText(scoreX, scoreBoxY, scoreText, Color::WHITE);
    } else {
        // For other libraries
        scoreBoxY = startY - 3;
        graphicsLib.drawBox(scoreBoxX - 1, scoreBoxY, scoreBoxWidth, 1, Color::WHITE);
        
        // Draw the score
        std::string scoreText = "Score: " + std::to_string(_score);
        int scoreX = startX + (BOARD_WIDTH * cellWidth - scoreText.length()) / 2;
        graphicsLib.drawText(scoreX, scoreBoxY, scoreText, Color::WHITE);
    }
    
    // Draw game over message if applicable
    if (_gameOver) {
        std::string gameOverText = "Game Over!";
        std::string restartText = "Press R to restart";
        int gameOverBoxWidth = std::max(gameOverText.length(), restartText.length()) + 4;
        int gameOverX = startX + (BOARD_WIDTH * cellWidth - gameOverBoxWidth) / 2;
        int gameOverY;
        
        if (isAllegro) {
            // Pour Allegro, centre le message "Game Over" dans le plateau
            gameOverY = startY + (BOARD_HEIGHT * cellHeight) / 2 - cellHeight;
            // Draw box around game over message - adjust size for Allegro
            graphicsLib.drawBox(gameOverX - 2, gameOverY - 1, gameOverBoxWidth, cellHeight * 3, Color::RED);
            
            // Draw the message
            int textX = startX + (BOARD_WIDTH * cellWidth - gameOverText.length() * 8) / 2;
            graphicsLib.drawText(textX, gameOverY, gameOverText, Color::RED);
            
            // Draw restart instruction
            int restartX = startX + (BOARD_WIDTH * cellWidth - restartText.length() * 8) / 2;
            graphicsLib.drawText(restartX, gameOverY + cellHeight, restartText, Color::RED);
        } else {
            // Pour les autres bibliothèques, centre le message "Game Over" dans le plateau
            gameOverY = startY + BOARD_HEIGHT / 2;
            graphicsLib.drawBox(gameOverX - 2, gameOverY - 1, gameOverBoxWidth, 3, Color::RED);
            
            // Draw the message
            int textX = startX + (BOARD_WIDTH * cellWidth - gameOverText.length()) / 2;
            graphicsLib.drawText(textX, gameOverY, gameOverText, Color::RED);
            
            // Draw restart instruction
            int restartX = startX + (BOARD_WIDTH * cellWidth - restartText.length()) / 2;
            graphicsLib.drawText(restartX, gameOverY + 1, restartText, Color::RED);
        }
    }
    
    // Draw controls help
    std::vector<std::string> controlsInfo = {
        "Controls:",
        "Arrows: Move snake",
        "L: Switch graphics",
        "G: Change game",
        "R: Restart game",
        "M: Back to menu", 
        "Q: Quit game",
        "ESC/E: Exit"
    };
    
    int controlsX = startX + BOARD_WIDTH * cellWidth + 4;
    int controlsY;
    
    if (isAllegro) {
        controlsY = startY;
        
        // Display controls with proper spacing for Allegro - Increased spacing between lines 
        for (size_t i = 0; i < controlsInfo.size(); i++) {
            // Multiplier 2 to create more space between lines
            graphicsLib.drawText(controlsX, controlsY + i * cellHeight * 2, controlsInfo[i], Color::CYAN);
        }
    } else {
        controlsY = startY + 1;
        
        // Display controls with proper spacing for other libraries
        for (size_t i = 0; i < controlsInfo.size(); i++) {
            graphicsLib.drawText(controlsX, controlsY + i, controlsInfo[i], Color::CYAN);
        }
    }
}

void SnakeGame::moveSnake()
{
    // Ensure the snake is not empty
    if (_snake.empty()) {
        return;
    }
    
    // Get current head position
    Position newHead = _snake.front();
    
    // Calculate new head position based on direction
    switch (_direction) {
        case Direction::UP:
            newHead.y--;
            break;
        case Direction::DOWN:
            newHead.y++;
            break;
        case Direction::LEFT:
            newHead.x--;
            break;
        case Direction::RIGHT:
            newHead.x++;
            break;
    }
    
    // Add new head to the front of the snake
    _snake.push_front(newHead);
}

bool SnakeGame::checkCollision(const Position& pos)
{
    // Vérifier les collisions avec les murs
    if (pos.x < 0 || pos.x >= BOARD_WIDTH || pos.y < 0 || pos.y >= BOARD_HEIGHT) {
        return true;
    }
    
    // Check for self collision (skip the head)
    auto it = _snake.begin();
    ++it; // Skip the head since we're checking if the head collides with the body
    
    for (; it != _snake.end(); ++it) {
        if (pos.x == it->x && pos.y == it->y) {
            return true;
        }
    }
    
    return false;
}

void SnakeGame::spawnFood()
{
    // Create distributions for random positions
    std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
    
    // Try to find a position that doesn't overlap with the snake
    bool validPosition = false;
    
    while (!validPosition) {
        _food.x = distX(_rng);
        _food.y = distY(_rng);
        
        validPosition = true;
        
        // Check if the food spawned on the snake
        for (const auto& segment : _snake) {
            if (_food.x == segment.x && _food.y == segment.y) {
                validPosition = false;
                break;
            }
        }
    }
}

void SnakeGame::restart()
{
    initialize();
}

void SnakeGame::cleanup()
{
    // No resources to clean up
}

bool SnakeGame::isGameOver() const
{
    return _exitRequested;
}

int SnakeGame::getScore() const
{
    return _score;
}

std::string SnakeGame::getName() const
{
    return "Snake";
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary() {
        return std::make_unique<arcd::SnakeGame>();
    }
    
    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib) {
        // With smart pointers, this function is not needed anymore
        // but we keep it for compatibility
    }
} 