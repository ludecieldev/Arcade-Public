/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** ScoreManager
*/

#ifndef SCORE_MANAGER_HPP
#define SCORE_MANAGER_HPP

#include <string>
#include <map>
#include <vector>

namespace arcd {

/**
 * @brief Structure to store a player's score
 */
struct Score {
    std::string playerName;     
    std::string gameName;   
    int value;               

    /**
     * @brief Constructor for Score
     * @param player name of the player
     * @param game name of the game
     * @param score score value
     */
    Score(const std::string& player, const std::string& game, int score)
        : playerName(player), gameName(game), value(score) {}
};

    /**
     * @brief Manager class for handling game scores
     */
    class ScoreManager {
        private:
            /**
             * @brief Map of game names to score lists
             */
            std::map<std::string, std::vector<Score>> _scores;

        public:
            /**
             * @brief Default constructor
             */
            ScoreManager() = default;
            
            /**
             * @brief Default destructor
             */
            ~ScoreManager() = default;

            /**
             * @brief Add a new score
             * @param playerName name of the player
             * @param gameName name of the game
             * @param score score value
             */
            void addScore(const std::string& playerName, const std::string& gameName, int score);

            /**
             * @brief Get all scores for a specific game
             * @param gameName name of the game
             * @return vector of Score objects for that game
             */
            std::vector<Score> getScores(const std::string& gameName) const;

            /**
             * @brief Get the best score for a specific game
             * @param gameName name of the game
             * @return Score object with the best score
             */
            Score getBestScore(const std::string& gameName) const;

            /**
             * @brief Get all scores for all games
             * @return map of game names to score vectors
             */
            std::map<std::string, std::vector<Score>> getAllScores() const;

            /**
             * @brief Save scores to a file
             * @param filename name of the file to save to
             * @return true if save succeeded, false otherwise
             */
            bool saveScores(const std::string& filename) const;

            /**
             * @brief Load scores from a file
             * @param filename name of the file to load from
             * @return true if load succeeded, false otherwise
             */
            bool loadScores(const std::string& filename);
    };

}

#endif // SCORE_MANAGER_HPP
