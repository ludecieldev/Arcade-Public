#ifndef MINESWEEPER_GAME_HPP
#define MINESWEEPER_GAME_HPP

#include "../interfaces/IGameLibrary.hpp"
#include "../interfaces/IGraphicsLibrary.hpp"
#include <vector>
#include <string>
#include <chrono>

namespace arcd {

class MinesweeperGame : public IGameLibrary {
private:
    enum class CellState { HIDDEN, REVEALED, FLAGGED };
    struct Cell {
        bool isMine;
        int adjacentMines;
        CellState state;
    };

    std::vector<std::vector<Cell>> _grid;
    int _rows;
    int _cols;
    int _mines;
    bool _gameOver;
    bool _win;
    int _flagsUsed;
    int _cursorRow;
    int _cursorCol;
    int _currentScore;

    // Timer related members
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::seconds _maxTime;
    bool _timerStarted;

    // Helper methods
    void initializeGrid();
    void placeMines();
    void calculateAdjacentMines();
    void revealCell(int row, int col);
    void toggleFlag(int row, int col);
    bool isWinConditionMet() const;
    int getRemainingTimeSeconds() const;
    void updateScore();

public:
    MinesweeperGame();
    ~MinesweeperGame() override = default;

    // IGameLibrary methods
    void initialize() override;
    void update() override;
    void render(IGraphicsLibrary& graphics) override;
    void handleInput(int key) override;
    bool isGameOver() const override;
    void cleanup() override;
    int getScore() const override;
    std::string getName() const override;
    void restart() override;
};

} // namespace arcd

#endif // MINESWEEPER_GAME_HPP