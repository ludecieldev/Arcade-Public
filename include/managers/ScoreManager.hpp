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

struct Score {
    std::string playerName;
    std::string gameName;
    int value;

    Score(const std::string& player, const std::string& game, int score)
        : playerName(player), gameName(game), value(score) {}
};

    class ScoreManager {
        private:
            std::map<std::string, std::vector<Score>> _scores; // Game name -> scores

        public:
            ScoreManager() = default;
            ~ScoreManager() = default;

            // Add a new score
            void addScore(const std::string& playerName, const std::string& gameName, int score);

            // Get all scores for a specific game
            std::vector<Score> getScores(const std::string& gameName) const;

            // Get the best score for a specific game
            Score getBestScore(const std::string& gameName) const;

            // Get all scores
            std::map<std::string, std::vector<Score>> getAllScores() const;

            // Save scores to file
            bool saveScores(const std::string& filename) const;

            // Load scores from file
            bool loadScores(const std::string& filename);
    };

}

#endif // SCORE_MANAGER_HPP
