#include "../../include/games/Minesweeper.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>

namespace arcd {

MinesweeperGame::MinesweeperGame()
    : _rows(9), _cols(9), _mines(10),
      _gameOver(false), _win(false), _flagsUsed(0),
      _cursorRow(0), _cursorCol(0)
{
    // Initialize the game in constructor to ensure grid is always valid
    initializeGrid();
}

void MinesweeperGame::initialize()
{
    // Reset game state
    _gameOver = false;
    _win = false;
    _flagsUsed = 0;
    _cursorRow = 0;
    _cursorCol = 0;

    // Initialize grid
    initializeGrid();
}

void MinesweeperGame::initializeGrid()
{
    // Resize and initialize the grid
    _grid.clear();
    _grid.resize(_rows, std::vector<Cell>(_cols));

    // Initialize all cells
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            _grid[i][j].isMine = false;
            _grid[i][j].adjacentMines = 0;
            _grid[i][j].state = CellState::HIDDEN;
        }
    }

    // Place mines and calculate adjacent mines
    placeMines();
    calculateAdjacentMines();
}

void MinesweeperGame::placeMines()
{
    // Create a vector of all possible positions
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            positions.push_back({i, j});
        }
    }

    // Use a random number generator with time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(positions.begin(), positions.end(), std::default_random_engine(seed));

    // Place mines at the first N positions (where N = _mines)
    for (int i = 0; i < _mines && i < static_cast<int>(positions.size()); i++) {
        int row = positions[i].first;
        int col = positions[i].second;
        _grid[row][col].isMine = true;
    }
}

void MinesweeperGame::calculateAdjacentMines()
{
    // For each cell, count adjacent mines
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            if (_grid[i][j].isMine) continue; // Skip mine cells

            int count = 0;
            // Check all 8 adjacent cells
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue; // Skip self

                    int ni = i + di;
                    int nj = j + dj;

                    // Bounds check
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

void MinesweeperGame::update()
{
    // Check for win condition
    if (!_gameOver && !_win) {
        _win = isWinConditionMet();
        if (_win) {
            _gameOver = true;
        }
    }
}

void MinesweeperGame::render(IGraphicsLibrary& graphics)
{
    // Always validate that the grid is properly initialized
    if (_grid.empty() || _grid[0].empty()) {
        initializeGrid();
    }

    // Constants for rendering
    const int startX = 5;
    const int startY = 3;
    const int cellWidth = 3;
    const int cellHeight = 1;

    // Clear screen and draw title
    graphics.clear();
    graphics.drawText(startX, startY - 2, "MINESWEEPER", Color::GREEN);

    // Draw game info
    std::string minesInfo = "Mines: " + std::to_string(_mines) + "  Flags: "
                          + std::to_string(_flagsUsed) + "/" + std::to_string(_mines);
    graphics.drawText(startX, startY - 1, minesInfo, Color::WHITE);

    // Draw grid
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            int x = startX + j * cellWidth;
            int y = startY + i * cellHeight;

            // Draw cell based on state
            if (_grid[i][j].state == CellState::HIDDEN) {
                // Highlight cursor position
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
            } else { // REVEALED
                if (_grid[i][j].isMine) {
                    if (i == _cursorRow && j == _cursorCol) {
                        graphics.drawText(x, y, "[*]", Color::CYAN);
                    } else {
                        graphics.drawText(x, y, "[*]", Color::RED);
                    }
                } else {
                    Color numColor;
                    std::string cellContent;

                    // Set color based on number of adjacent mines
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

    // Game over / win message
    if (_gameOver) {
        if (_win) {
            graphics.drawText(startX, startY + _rows * cellHeight + 1, "You Win! Press R to restart", Color::GREEN);
        } else {
            graphics.drawText(startX, startY + _rows * cellHeight + 1, "Game Over! Press R to restart", Color::RED);
        }
    } else {
        graphics.drawText(startX, startY + _rows * cellHeight + 1, "SPACE: Reveal, F: Flag, ESC: Exit", Color::WHITE);
    }

    graphics.refresh();
}

void MinesweeperGame::handleInput(int key)
{
    if (_gameOver) {
        // Only handle restart in game over state
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
        case ' ': // Reveal cell
            revealCell(_cursorRow, _cursorCol);
            break;
        case 'f': // Toggle flag
        case 'F':
            toggleFlag(_cursorRow, _cursorCol);
            break;
        case IGraphicsLibrary::KEY_RESTART_GAME: // Restart game
            restart();
            break;
    }
}

void MinesweeperGame::revealCell(int row, int col)
{
    // Bounds check and already revealed check
    if (row < 0 || row >= _rows || col < 0 || col >= _cols ||
        _grid[row][col].state == CellState::REVEALED) {
        return;
    }

    // Can't reveal flagged cells
    if (_grid[row][col].state == CellState::FLAGGED) {
        return;
    }

    // Reveal the cell
    _grid[row][col].state = CellState::REVEALED;

    // Check if mine was hit
    if (_grid[row][col].isMine) {
        // Reveal all mines
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

    // If empty cell (no adjacent mines), reveal neighbors recursively
    if (_grid[row][col].adjacentMines == 0) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if (di == 0 && dj == 0) continue;
                revealCell(row + di, col + dj);
            }
        }
    }
}

void MinesweeperGame::toggleFlag(int row, int col)
{
    // Bounds check and already revealed check
    if (row < 0 || row >= _rows || col < 0 || col >= _cols ||
        _grid[row][col].state == CellState::REVEALED) {
        return;
    }

    // Toggle flag
    if (_grid[row][col].state == CellState::HIDDEN) {
        _grid[row][col].state = CellState::FLAGGED;
        _flagsUsed++;
    } else if (_grid[row][col].state == CellState::FLAGGED) {
        _grid[row][col].state = CellState::HIDDEN;
        _flagsUsed--;
    }
}

bool MinesweeperGame::isWinConditionMet() const
{
    // Win condition: all non-mine cells are revealed
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            if (!_grid[i][j].isMine && _grid[i][j].state != CellState::REVEALED) {
                return false;
            }
        }
    }
    return true;
}

bool MinesweeperGame::isGameOver() const
{
    return _gameOver;
}

void MinesweeperGame::cleanup()
{
    // Nothing special to clean up
}

int MinesweeperGame::getScore() const
{
    // Calculate score based on cells revealed
    int revealedCount = 0;

    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            if (_grid[i][j].state == CellState::REVEALED && !_grid[i][j].isMine) {
                revealedCount++;
            }
        }
    }

    return revealedCount * 10; // 10 points per revealed cell
}

std::string MinesweeperGame::getName() const
{
    return "Minesweeper";
}

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