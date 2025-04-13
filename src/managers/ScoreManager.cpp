/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** ScoreManager
*/

#include "managers/ScoreManager.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>

namespace arcd {

/**
 * @brief Constructor for ScoreManager
 * Initializes the score manager with an empty score map
 */
void ScoreManager::addScore(const std::string& playerName, const std::string& gameName, int score)
{
    Score newScore(playerName, gameName, score);
    _scores[gameName].push_back(newScore);
    
    std::sort(_scores[gameName].begin(), _scores[gameName].end(), 
              [](const Score& a, const Score& b) { return a.value > b.value; });
}

/**
 * @brief Retrieves all scores for a specific game
 * @param gameName Name of the game
 * @return Vector of scores for the specified game
 */
std::vector<Score> ScoreManager::getScores(const std::string& gameName) const
{
    auto it = _scores.find(gameName);
    if (it != _scores.end()) {
        auto scores = it->second;
        std::sort(scores.begin(), scores.end(), 
                 [](const Score& a, const Score& b) { return a.value > b.value; });
        return scores;
    }
    return {};
}

/**
 * @brief Retrieves the best score for a specific game
 * @param gameName Name of the game
 * @return Best score for the specified game
 */
Score ScoreManager::getBestScore(const std::string& gameName) const
{
    auto scores = getScores(gameName);
    if (!scores.empty()) {
        return scores[0];
    }
    return Score("", gameName, 0);
}

/**
 * @brief Retrieves all scores for all games
 * @return Map of game names to their respective scores
 */
std::map<std::string, std::vector<Score>> ScoreManager::getAllScores() const
{
    return _scores;
}

/**
 * @brief Saves the scores to a file
 * @param filename Name of the file to save the scores to
 * @return True if successful, false otherwise
 */
bool ScoreManager::saveScores(const std::string& filename) const
{
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& [gameName, scores] : _scores) {
            for (const auto& score : scores) {
                file << gameName << "," << score.playerName << "," << score.value << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving scores: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Loads scores from a file
 * @param filename Name of the file to load the scores from
 * @return True if successful, false otherwise
 */
bool ScoreManager::loadScores(const std::string& filename)
{
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        _scores.clear();
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos1 = line.find(',');
            if (pos1 == std::string::npos) continue;
            
            size_t pos2 = line.find(',', pos1 + 1);
            if (pos2 == std::string::npos) continue;
            
            std::string gameName = line.substr(0, pos1);
            std::string playerName = line.substr(pos1 + 1, pos2 - pos1 - 1);
            int score = std::stoi(line.substr(pos2 + 1));
            
            addScore(playerName, gameName, score);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading scores: " << e.what() << std::endl;
        return false;
    }
}

} // namespace arcd
