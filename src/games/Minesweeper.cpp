#include "../../include/games/Minesweeper.hpp"
#include <cstdlib>
#include <ctime>
#include <stdexcept>

namespace arcd {

MinesweeperGame::MinesweeperGame()
    : _rows(10), _cols(10), _mines(15), _gameOver(false), _win(false), _flagsUsed(0) {
    std::srand(std::time(nullptr));
}

void MinesweeperGame::initialize() {
    _gameOver = false;
    _win = false;
    _flagsUsed = 0;
    initializeGrid();
    placeMines();
    calculateAdjacentMines();
}

void MinesweeperGame::initializeGrid() {
    _grid = std::vector<std::vector<Cell>>(_rows, std::vector<Cell>(_cols, {false, 0, CellState::HIDDEN}));
}

void MinesweeperGame::placeMines() {
    int placedMines = 0;
    while (placedMines < _mines) {
        int row = std::rand() % _rows;
        int col = std::rand() % _cols;
        if (!_grid[row][col].isMine) {
            _grid[row][col].isMine = true;
            placedMines++;
        }
    }
}

void MinesweeperGame::calculateAdjacentMines() {
    for (int row = 0; row < _rows; ++row) {
        for (int col = 0; col < _cols; ++col) {
            if (_grid[row][col].isMine) continue;
            int count = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = row + dr, nc = col + dc;
                    if (nr >= 0 && nr < _rows && nc >= 0 && nc < _cols && _grid[nr][nc].isMine) {
                        count++;
                    }
                }
            }
            _grid[row][col].adjacentMines = count;
        }
    }
}

void MinesweeperGame::revealCell(int row, int col) {
    if (row < 0 || row >= _rows || col < 0 || col >= _cols || _grid[row][col].state != CellState::HIDDEN) {
        return;
    }

    _grid[row][col].state = CellState::REVEALED;

    if (_grid[row][col].isMine) {
        _gameOver = true;
        return;
    }

    if (_grid[row][col].adjacentMines == 0) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                revealCell(row + dr, col + dc);
            }
        }
    }

    if (isWinConditionMet()) {
        _win = true;
        _gameOver = true;
    }
}

void MinesweeperGame::toggleFlag(int row, int col) {
    if (row < 0 || row >= _rows || col < 0 || col >= _cols) return;

    if (_grid[row][col].state == CellState::HIDDEN && _flagsUsed < _mines) {
        _grid[row][col].state = CellState::FLAGGED;
        _flagsUsed++;
    } else if (_grid[row][col].state == CellState::FLAGGED) {
        _grid[row][col].state = CellState::HIDDEN;
        _flagsUsed--;
    }
}

bool MinesweeperGame::isWinConditionMet() const {
    for (const auto& row : _grid) {
        for (const auto& cell : row) {
            if (!cell.isMine && cell.state != CellState::REVEALED) {
                return false;
            }
        }
    }
    return true;
}

void MinesweeperGame::update() {
    // No update
}

void MinesweeperGame::render(IGraphicsLibrary& graphics) {
    int cellSize = 20;
    for (int row = 0; row < _rows; ++row) {
        for (int col = 0; col < _cols; ++col) {
            int x = col * cellSize;
            int y = row * cellSize;
            switch (_grid[row][col].state) {
                case CellState::HIDDEN:
                    graphics.drawBox(x, y, cellSize, cellSize, Color::BLUE);
                    break;
                case CellState::REVEALED:
                    if (_grid[row][col].isMine) {
                        graphics.drawBox(x, y, cellSize, cellSize, Color::RED);
                    } else {
                        graphics.drawBox(x, y, cellSize, cellSize, Color::WHITE);
                        if (_grid[row][col].adjacentMines > 0) {
                            graphics.drawText(x + 5, y + 5, std::to_string(_grid[row][col].adjacentMines), Color::BLACK);
                        }
                    }
                    break;
                case CellState::FLAGGED:
                    graphics.drawBox(x, y, cellSize, cellSize, Color::YELLOW);
                    break;
            }
        }
    }
}

void MinesweeperGame::handleInput(int key) {
    key = key;
}

void MinesweeperGame::handleMouseInput(const IGraphicsLibrary::MouseEvent& mouseEvent) {
    if (!mouseEvent.hasEvent) return;

    int cellSize = 20;
    int row = mouseEvent.y / cellSize;
    int col = mouseEvent.x / cellSize;

    if (mouseEvent.button == IGraphicsLibrary::MOUSE_LEFT_BUTTON && mouseEvent.pressed) {
        revealCell(row, col);
    } else if (mouseEvent.button == IGraphicsLibrary::MOUSE_RIGHT_BUTTON && mouseEvent.pressed) {
        toggleFlag(row, col);
    }
}

bool MinesweeperGame::isGameOver() const {
    return _gameOver;
}

int MinesweeperGame::getScore() const {
    return _win ? 100 : 0;
}

std::string MinesweeperGame::getName() const {
    return "Minesweeper";
}

void MinesweeperGame::restart() {
    initialize();
}

} // namespace arcd