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

/**
 * @brief Initialize the game state
 * - Reset game state
 * - Create initial snake in the middle of the board
 * - Create the snake extending to the left (since we'll move right)
 * - Spawn initial food
 * - Reset timer
 */
void SnakeGame::initialize()
{
    _gameOver = false;
    _exitRequested = false;
    _score = 0;
    _direction = Direction::RIGHT;
    _nextDirection = Direction::RIGHT;
    _snake.clear();
    
    int startX = BOARD_WIDTH / 2 - INITIAL_SNAKE_LENGTH;
    int startY = BOARD_HEIGHT / 2;
    
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        _snake.push_front({startX + i, startY});
    }
    
    spawnFood();
    
    _lastUpdateTime = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Update the game state
 * - Check if enough time has passed for update
 * - Update snake direction
 * - Move the snake
 * - Check for food collision
 * - Check for collisions with walls and self
 * - Reset timer
 */
void SnakeGame::update()
{
    if (_gameOver) {
        return;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - _lastUpdateTime).count();
    
    if (elapsedTime < UPDATE_INTERVAL_MS) {
        return;
    }
    
    _direction = _nextDirection;
    
    moveSnake();
    
    if (_snake.front().x == _food.x && _snake.front().y == _food.y) {
        _score += 10;
        spawnFood();
    } else {
        _snake.pop_back();
    }
    
    if (checkCollision(_snake.front())) {
        _gameOver = true;
    }
    
    _lastUpdateTime = currentTime;
}

/**
 * @brief Handle input from the user
 * - Handle restart on 'R' key press regardless of game state
 * - In game over screen, allow quitting to menu
 * - Handle movement keys based on current direction
 * - Handle special keys for library and game switching
 * 
 * @param key Key code from the input system
 */
void SnakeGame::handleInput(int key)
{
    if (key == 'r' || key == 'R') {
        restart();
        return;
    }
    
    if (_gameOver) {
        if (key == 'm' || key == 'M' || key == 'q' || key == 'Q' || key == IGraphicsLibrary::KEY_ESC_CODE) {
            _exitRequested = true;
        }
        return;
    }
    
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
        
        case IGraphicsLibrary::KEY_NEXT_LIB_CODE:
            break;
            
        case IGraphicsLibrary::KEY_NEXT_GAME_CODE:
            break;
            
        case 'e':
        case 'E':
        case IGraphicsLibrary::KEY_ESC_CODE:
            break;
    }
}

/**
 * @brief Render the game
 * - Clear the screen
 * - Draw the board, snake, food and info
 * - Refresh the display
 * 
 * @param graphicsLib Graphics library to use for rendering
 */
void SnakeGame::render(IGraphicsLibrary& graphicsLib)
{
    int screenWidth = graphicsLib.getWidth();
    int screenHeight = graphicsLib.getHeight();
    
    int cellWidth = 2;
    int cellHeight = 1;
    
    if (screenWidth < 80) {
        cellWidth = 1;
    }
    
    int startX = (screenWidth - BOARD_WIDTH * cellWidth) / 2;
    int startY = (screenHeight - BOARD_HEIGHT * cellHeight) / 2;
    
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    
    drawBoard(graphicsLib, startX, startY, cellWidth, cellHeight);
    drawSnake(graphicsLib, startX, startY, cellWidth, cellHeight);
    drawFood(graphicsLib, startX, startY, cellWidth, cellHeight);
    drawInfo(graphicsLib, startX, startY, cellWidth, cellHeight);
}

/**
 * @brief Draw the game board
 * - Draw the border box around the play area
 * 
 * @param graphicsLib Graphics library to use for rendering
 * @param startX Starting X position of the board
 * @param startY Starting Y position of the board
 * @param cellWidth Width of each cell
 * @param cellHeight Height of each cell
 */
void SnakeGame::drawBoard(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    graphicsLib.drawBox(
        startX - 1, 
        startY - 1, 
        BOARD_WIDTH * cellWidth + 2, 
        BOARD_HEIGHT * cellHeight + 2, 
        Color::CYAN
    );
}

/**
 * @brief Draw the snake on the board
 * - Draw the snake head with a different character
 * - Draw each snake body segment
 * 
 * @param graphicsLib Graphics library to use for rendering
 * @param startX Starting X position of the board
 * @param startY Starting Y position of the board
 * @param cellWidth Width of each cell
 * @param cellHeight Height of each cell
 */
void SnakeGame::drawSnake(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    (void)cellHeight; // Mark as unused to avoid compiler warnings
    
    if (_snake.empty()) {
        return;
    }
    
    Position head = _snake.front();
    std::string headChar = "O";
    
    graphicsLib.drawText(startX + head.x * cellWidth, startY + head.y, headChar, Color::YELLOW);
    
    for (size_t i = 1; i < _snake.size(); i++) {
        Position segment = _snake[i];
        std::string bodyChar = "o";
        graphicsLib.drawText(startX + segment.x * cellWidth, startY + segment.y, bodyChar, Color::GREEN);
    }
}

/**
 * @brief Draw the food on the board
 * - Draw the food with a special character
 * 
 * @param graphicsLib Graphics library to use for rendering
 * @param startX Starting X position of the board
 * @param startY Starting Y position of the board
 * @param cellWidth Width of each cell
 * @param cellHeight Height of each cell
 */
void SnakeGame::drawFood(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    (void)cellHeight; // Mark as unused to avoid compiler warnings
    
    std::string foodChar = "*";
    graphicsLib.drawText(startX + _food.x * cellWidth, startY + _food.y, foodChar, Color::RED);
}

/**
 * @brief Draw game information and UI elements
 * - Draw score box
 * - Draw game over message if applicable
 * - Draw game controls help
 * 
 * @param graphicsLib Graphics library to use for rendering
 * @param startX Starting X position of the board
 * @param startY Starting Y position of the board
 * @param cellWidth Width of each cell
 * @param cellHeight Height of each cell
 */
void SnakeGame::drawInfo(IGraphicsLibrary& graphicsLib, int startX, int startY, int cellWidth, int cellHeight)
{
    (void)cellHeight; // Mark as unused to avoid compiler warnings
    
    int scoreBoxWidth = 20;
    int scoreBoxX = startX + (BOARD_WIDTH * cellWidth - scoreBoxWidth) / 2;
    int scoreBoxY = startY - 3;
    
    graphicsLib.drawBox(scoreBoxX - 1, scoreBoxY, scoreBoxWidth, 1, Color::WHITE);
    
    std::string scoreText = "Score: " + std::to_string(_score);
    int scoreX = startX + (BOARD_WIDTH * cellWidth - scoreText.length()) / 2;
    graphicsLib.drawText(scoreX, scoreBoxY, scoreText, Color::WHITE);
    
    if (_gameOver) {
        std::string gameOverText = "Game Over!";
        std::string restartText = "Press R to restart";
        int gameOverBoxWidth = std::max(gameOverText.length(), restartText.length()) + 4;
        int gameOverX = startX + (BOARD_WIDTH * cellWidth - gameOverBoxWidth) / 2;
        int gameOverY = startY + BOARD_HEIGHT / 2;
        
        graphicsLib.drawBox(gameOverX - 2, gameOverY - 1, gameOverBoxWidth, 3, Color::RED);
        
        int textX = startX + (BOARD_WIDTH * cellWidth - gameOverText.length()) / 2;
        graphicsLib.drawText(textX, gameOverY, gameOverText, Color::RED);
        
        int restartX = startX + (BOARD_WIDTH * cellWidth - restartText.length()) / 2;
        graphicsLib.drawText(restartX, gameOverY + 1, restartText, Color::RED);
    }
    
    std::vector<std::string> controlsInfo = {
        "Controls:",
        "Arrows: Move snake",
        "L: Switch graphics",
        "G: Change game",
        "R: Restart game",
        "M: Back to menu", 
        "Q: Quit game",
        "ESC/E: Exit",
        "MENU: L for Leaderboard"
    };
    
    int controlsX = startX + BOARD_WIDTH * cellWidth + 4;
    int controlsY = startY + 1;
    
    for (size_t i = 0; i < controlsInfo.size(); i++) {
        graphicsLib.drawText(controlsX, controlsY + i, controlsInfo[i], Color::CYAN);
    }
}

/**
 * @brief Move the snake in the current direction
 * - Get current head position
 * - Calculate new head position based on direction
 * - Add new head to the front of the snake
 */
void SnakeGame::moveSnake()
{
    if (_snake.empty()) {
        return;
    }
    
    Position newHead = _snake.front();
    
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
    
    _snake.push_front(newHead);
}

/**
 * @brief Check if there is a collision at the given position
 * - Check for collisions with walls
 * - Check for self collision (skip the head)
 * 
 * @param pos Position to check for collision
 * @return true if there is a collision, false otherwise
 */
bool SnakeGame::checkCollision(const Position& pos)
{
    if (pos.x < 0 || pos.x >= BOARD_WIDTH || pos.y < 0 || pos.y >= BOARD_HEIGHT) {
        return true;
    }
    
    auto it = _snake.begin();
    ++it;
    
    for (; it != _snake.end(); ++it) {
        if (pos.x == it->x && pos.y == it->y) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Spawn food at a random position
 * - Create distributions for random positions
 * - Try to find a position that doesn't overlap with the snake
 * - Keep generating positions until a valid one is found
 */
void SnakeGame::spawnFood()
{
    std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
    
    bool validPosition = false;
    
    while (!validPosition) {
        _food.x = distX(_rng);
        _food.y = distY(_rng);
        
        validPosition = true;
        
        for (const auto& segment : _snake) {
            if (_food.x == segment.x && _food.y == segment.y) {
                validPosition = false;
                break;
            }
        }
    }
}

/**
 * @brief Restart the game
 * - Call initialize to reset the game state
 */
void SnakeGame::restart()
{
    initialize();
}

/**
 * @brief Clean up resources
 * - No resources to clean up in this implementation
 */
void SnakeGame::cleanup()
{
}

/**
 * @brief Check if the game is over
 * 
 * @return true if the game is over or exit requested, false otherwise
 */
bool SnakeGame::isGameOver() const
{
    return _gameOver || _exitRequested;
}

/**
 * @brief Get the current score
 * 
 * @return Current score
 */
int SnakeGame::getScore() const
{
    return _score;
}

/**
 * @brief Get the name of the game
 * 
 * @return Name of the game
 */
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
    }
} 