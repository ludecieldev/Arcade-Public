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
    /**
     * @brief CellState enum for the state of each cell
     * @param HIDDEN: cell is hidden
     * @param REVEALED: cell is revealed
     * @param FLAGGED: cell is flagged
     */
    enum class CellState { HIDDEN, REVEALED, FLAGGED };
    struct Cell {
        bool isMine;
        int adjacentMines;
        CellState state;
    };

    /**
     * @brief Game state variables
     */
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

    /**
     * @brief Timer variables
     */
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::seconds _maxTime;
    bool _timerStarted;

    /**
     * @brief Initializes the game state
     * Resets the game state and initializes the grid.
     */
    void initializeGrid();

    /**
     * @brief Places mines randomly on the grid
     */
    void placeMines();

    /**
     * @brief Calculates the number of adjacent mines for each cell
     */
    void calculateAdjacentMines();

    /**
     * @brief Reveals a cell and its adjacent cells if necessary
     * @param row Row of the cell to reveal
     * @param col Column of the cell to reveal
     */
    void revealCell(int row, int col);

    /**
     * @brief Toggles a flag on a cell
     * @param row Row of the cell to toggle
     * @param col Column of the cell to toggle
     */
    void toggleFlag(int row, int col);

    /**
     * @brief Checks if the win condition is met
     * @return True if win condition is met, false otherwise
     */
    bool isWinConditionMet() const;

    /**
     * @brief Checks if the game is over
     * @return True if the game is over, false otherwise
     */
    int getRemainingTimeSeconds() const;

    /**
     * @brief Updates the score based on the number of revealed cells
     */
    void updateScore();

public:
    /**
     * @brief Constructor for MinesweeperGame
     * Initializes the game state and grid.
     */
    MinesweeperGame();

    /**
     * @brief Destructor for MinesweeperGame
     * Cleans up the game state.
     */
    ~MinesweeperGame() override = default;

    /**
     * @brief Initializes the game state
     * Resets the game state and initializes the grid.
     */
    void initialize() override;

    /**
     * @brief Updates the game state
     * Checks for win condition and updates the score.
     */
    void update() override;

    /**
     * @brief Renders the game
     * Displays the grid and game information.
     * @param graphics Reference to the graphics library
     */
    void render(IGraphicsLibrary& graphics) override;

    /**
     * @brief Handles user input
     * @param key Key pressed by the user
     */
    void handleInput(int key) override;

    /**
     * @brief Checks if the game is over
     * @return True if the game is over, false otherwise
     */
    bool isGameOver() const override;

    /**
     * @brief Checks if the game is won
     * @return True if the game is won, false otherwise
     */
    void cleanup() override;

    /**
     * @brief Gets the score
     * @return Current score
     */
    int getScore() const override;

    /**
     * @brief Gets the name of the game
     * @return Name of the game
     */
    std::string getName() const override;

    /**
     * @brief Restarts the game
     * Resets the game state and reinitializes the grid.
     */
    void restart() override;

};

} // namespace arcd

#endif // MINESWEEPER_GAME_HPP