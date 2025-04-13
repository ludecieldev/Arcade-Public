#include "../../include/games/Minesweeper.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>

namespace arcd {

/**
 * @brief Minesweeper game implementation
 * This class implements the Minesweeper game logic.
 */
MinesweeperGame::MinesweeperGame()
    : _rows(9), _cols(9), _mines(10),
      _gameOver(false), _win(false), _flagsUsed(0),
      _cursorRow(0), _cursorCol(0), _currentScore(0),
      _maxTime(std::chrono::seconds(300)), // 5 minutes
      _timerStarted(false)
{
    initializeGrid();
}

/**
 * @brief Initializes the game state
 * Resets the game state and initializes the grid.
 */
void MinesweeperGame::initialize()
{
    _gameOver = false;
    _win = false;
    _flagsUsed = 0;
    _cursorRow = 0;
    _cursorCol = 0;
    _currentScore = 0;
    _timerStarted = false;

    initializeGrid();
}

/**
 * @brief Initializes the grid with cells and mines
 * Resizes the grid and initializes all cells.
 */
void MinesweeperGame::initializeGrid()
{
    _grid.clear();
    _grid.resize(_rows, std::vector<Cell>(_cols));

    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            _grid[i][j].isMine = false;
            _grid[i][j].adjacentMines = 0;
            _grid[i][j].state = CellState::HIDDEN;
        }
    }

    placeMines();
    calculateAdjacentMines();
}

/**
 * @brief Handles user input
 * @param key Key pressed by the user
 */
void MinesweeperGame::placeMines()
{
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            positions.push_back({i, j});
        }
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(positions.begin(), positions.end(), std::default_random_engine(seed));

    for (int i = 0; i < _mines && i < static_cast<int>(positions.size()); i++) {
        int row = positions[i].first;
        int col = positions[i].second;
        _grid[row][col].isMine = true;
    }
}

/**
 * @brief Reveals a cell
 * @param row Row of the cell to reveal
 * @param col Column of the cell to reveal
 */
void MinesweeperGame::calculateAdjacentMines()
{
    // For each cell, count adjacent mines
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            if (_grid[i][j].isMine) continue;

            int count = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue;

                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < _rows && nj >= 0 && nj < _cols) {
                        if (_grid[ni][nj].isMine) {
                            count++;
                        }
                    }
                }
            }

            _grid[i][j].adjacentMines = count;
        }
    }
}

/**
 * @brief Reveals a cell
 * @param row Row of the cell to reveal
 * @param col Column of the cell to reveal
 */
int MinesweeperGame::getRemainingTimeSeconds() const
{
    if (!_timerStarted) {
        return _maxTime.count();
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _startTime);

    int remaining = _maxTime.count() - elapsed.count();
    return std::max(0, remaining);
}

/**
 * @brief Toggles a flag on a cell
 * @param row Row of the cell to toggle
 * @param col Column of the cell to toggle
 */
void MinesweeperGame::updateScore()
{
    int revealedCount = 0;

    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            if (_grid[i][j].state == CellState::REVEALED && !_grid[i][j].isMine) {
                revealedCount++;
            }
        }
    }

    _currentScore = revealedCount * 10;
}

/**
 * @brief Checks if the game is over
 * @return True if the game is over, false otherwise
 */
void MinesweeperGame::update()
{
    if (!_timerStarted) {
        _startTime = std::chrono::steady_clock::now();
        _timerStarted = true;
    }

    updateScore();

    if (getRemainingTimeSeconds() <= 0 && !_gameOver) {
        _gameOver = true;
        _win = false;
    }

    if (!_gameOver && !_win) {
        _win = isWinConditionMet();
        if (_win) {
            _currentScore += getRemainingTimeSeconds() * 2;
            _gameOver = true;
        }
    }
}

/**
 * @brief Renders the game
 * @param graphics Reference to the graphics library
 */
void MinesweeperGame::render(IGraphicsLibrary& graphics)
{
    if (_grid.empty() || _grid[0].empty()) {
        initializeGrid();
    }

    const int startX = 5;
    const int startY = 3;
    const int cellWidth = 3;
    const int cellHeight = 1;

    graphics.clear();
    graphics.drawText(startX, startY - 2, "MINESWEEPER", Color::GREEN);

    std::string minesInfo = "Mines: " + std::to_string(_mines) + "  Flags: "
                          + std::to_string(_flagsUsed) + "/" + std::to_string(_mines);
    graphics.drawText(startX, startY - 1, minesInfo, Color::WHITE);

    int remainingTime = getRemainingTimeSeconds();
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;

    std::string timeStr = "Time: ";
    timeStr += (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":";
    timeStr += (seconds < 10 ? "0" : "") + std::to_string(seconds);

    Color timerColor = Color::GREEN;
    if (remainingTime < 60) {
        timerColor = Color::RED;
    } else if (remainingTime < 120) {
        timerColor = Color::YELLOW;
    }

    graphics.drawText(startX + 25, startY - 1, timeStr, timerColor);

    std::string scoreText = "Score: " + std::to_string(_currentScore);
    graphics.drawText(startX + 40, startY - 1, scoreText, Color::CYAN);

    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            int x = startX + j * cellWidth;
            int y = startY + i * cellHeight;

            if (_grid[i][j].state == CellState::HIDDEN) {
                if (i == _cursorRow && j == _cursorCol) {
                    graphics.drawText(x, y, "[#]", Color::CYAN);
                } else {
                    graphics.drawText(x, y, "[ ]", Color::WHITE);
                }
            } else if (_grid[i][j].state == CellState::FLAGGED) {
                if (i == _cursorRow && j == _cursorCol) {
                    graphics.drawText(x, y, "[F]", Color::CYAN);
                } else {
                    graphics.drawText(x, y, "[F]", Color::RED);
                }
            } else {
                if (_grid[i][j].isMine) {
                    if (i == _cursorRow && j == _cursorCol) {
                        graphics.drawText(x, y, "[*]", Color::CYAN);
                    } else {
                        graphics.drawText(x, y, "[*]", Color::RED);
                    }
                } else {
                    Color numColor;
                    std::string cellContent;

                    switch (_grid[i][j].adjacentMines) {
                        case 0: numColor = Color::DEFAULT; cellContent = "   "; break;
                        case 1: numColor = Color::BLUE; cellContent = " 1 "; break;
                        case 2: numColor = Color::GREEN; cellContent = " 2 "; break;
                        case 3: numColor = Color::RED; cellContent = " 3 "; break;
                        case 4: numColor = Color::MAGENTA; cellContent = " 4 "; break;
                        case 5: numColor = Color::YELLOW; cellContent = " 5 "; break;
                        case 6: numColor = Color::CYAN; cellContent = " 6 "; break;
                        case 7: numColor = Color::WHITE; cellContent = " 7 "; break;
                        default: numColor = Color::WHITE; cellContent = " 8 "; break;
                    }

                    if (i == _cursorRow && j == _cursorCol) {
                        graphics.drawText(x, y, cellContent, Color::CYAN);
                    } else {
                        graphics.drawText(x, y, cellContent, numColor);
                    }
                }
            }
        }
    }

    if (_gameOver) {
        if (_win) {
            std::string winMsg = "You Win! Final Score: " + std::to_string(_currentScore) + " - Press R to restart";
            graphics.drawText(startX, startY + _rows * cellHeight + 1, winMsg, Color::GREEN);
        } else {
            std::string overMsg = "Game Over! Score: " + std::to_string(_currentScore) + " - Press R to restart";
            graphics.drawText(startX, startY + _rows * cellHeight + 1, overMsg, Color::RED);
        }
    } else {
        graphics.drawText(startX, startY + _rows * cellHeight + 1, "SPACE: Reveal, F: Flag, ESC: Exit, L: Switch graphics, R: Restart game", Color::WHITE);
    }

    graphics.refresh();
}

/**
 * @brief Handles user input
 * @param key Key pressed by the user
 */
void MinesweeperGame::handleInput(int key)
{
    if (_gameOver) {
        if (key == IGraphicsLibrary::KEY_RESTART_GAME) {
            restart();
        }
        return;
    }

    // Movement
    switch (key) {
        case IGraphicsLibrary::KEY_UP_CODE:
            _cursorRow = std::max(0, _cursorRow - 1);
            break;
        case IGraphicsLibrary::KEY_DOWN_CODE:
            _cursorRow = std::min(_rows - 1, _cursorRow + 1);
            break;
        case IGraphicsLibrary::KEY_LEFT_CODE:
            _cursorCol = std::max(0, _cursorCol - 1);
            break;
        case IGraphicsLibrary::KEY_RIGHT_CODE:
            _cursorCol = std::min(_cols - 1, _cursorCol + 1);
            break;
        case ' ':
            revealCell(_cursorRow, _cursorCol);
            break;
        case 'f':
        case 'F':
            toggleFlag(_cursorRow, _cursorCol);
            break;
        case IGraphicsLibrary::KEY_RESTART_GAME:
            restart();
            break;
    }
}

/**
 * @brief Reveals a cell and handles game logic
 * @param row Row of the cell to reveal
 * @param col Column of the cell to reveal
 */
void MinesweeperGame::revealCell(int row, int col)
{
    if (row < 0 || row >= _rows || col < 0 || col >= _cols ||
        _grid[row][col].state == CellState::REVEALED) {
        return;
    }

    if (_grid[row][col].state == CellState::FLAGGED) {
        return;
    }

    _grid[row][col].state = CellState::REVEALED;

    if (_grid[row][col].isMine) {
        for (int i = 0; i < _rows; i++) {
            for (int j = 0; j < _cols; j++) {
                if (_grid[i][j].isMine) {
                    _grid[i][j].state = CellState::REVEALED;
                }
            }
        }
        _gameOver = true;
        return;
    }

    updateScore();

    if (_grid[row][col].adjacentMines == 0) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if (di == 0 && dj == 0) continue;
                revealCell(row + di, col + dj);
            }
        }
    }
}

/**
 * @brief Toggles a flag on a cell
 * @param row Row of the cell to toggle
 * @param col Column of the cell to toggle
 */
void MinesweeperGame::toggleFlag(int row, int col)
{
    if (row < 0 || row >= _rows || col < 0 || col >= _cols ||
        _grid[row][col].state == CellState::REVEALED) {
        return;
    }

    if (_grid[row][col].state == CellState::HIDDEN) {
        _grid[row][col].state = CellState::FLAGGED;
        _flagsUsed++;
    } else if (_grid[row][col].state == CellState::FLAGGED) {
        _grid[row][col].state = CellState::HIDDEN;
        _flagsUsed--;
    }
}

/**
 * @brief Checks if the win condition is met
 * @return True if win condition is met, false otherwise
 */
bool MinesweeperGame::isWinConditionMet() const
{
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            if (!_grid[i][j].isMine && _grid[i][j].state != CellState::REVEALED) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Checks if the game is over
 * @return True if the game is over, false otherwise
 */
bool MinesweeperGame::isGameOver() const
{
    return _gameOver;
}

/**
 * @brief Cleans up the game state
 * Currently does nothing, but can be used for future cleanup tasks.
 */
void MinesweeperGame::cleanup()
{
}

/**
 * @brief Returns the current score
 * @return Current score of the game
 */
int MinesweeperGame::getScore() const
{
    if (_win) {
        return _currentScore;
    }
    return _currentScore;
}

/**
 * @brief Returns the name of the game
 * @return Name of the game
 */
std::string MinesweeperGame::getName() const
{
    return "Minesweeper";
}

/**
 * @brief Restarts the game
 * Resets the game state and reinitializes the grid.
 */
void MinesweeperGame::restart()
{
    initialize();
}

} // namespace arcd

extern "C" {
    std::unique_ptr<arcd::IGameLibrary> createGameLibrary()
    {
        return std::make_unique<arcd::MinesweeperGame>();
    }

    void destroyGameLibrary([[maybe_unused]] arcd::IGameLibrary* gameLib)
    {
        // Using smart pointers, so no need to manually delete
    }
}