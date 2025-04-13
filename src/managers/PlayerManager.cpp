/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** PlayerManager - Manages player data
*/

#include "managers/PlayerManager.hpp"
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

namespace arcd {

/**
 * @brief Get the current date as a formatted string
 * @return string with current date
 */
std::string PlayerManager::getCurrentDate() const {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

/**
 * @brief Get the file path for the current player's save file
 * @return string with file path
 */
std::string PlayerManager::getPlayerFilePath() const {
    return _saveDirectory + _playerName + _saveExtension;
}

/**
 * @brief Constructor for PlayerManager
 */
PlayerManager::PlayerManager() : _playerName("Player"), _saveDirectory("./scores/"), _saveExtension(".save") {
    std::filesystem::create_directories(_saveDirectory);
}

/**
 * @brief Set the player's name
 * @param name player name to set
 */
void PlayerManager::setPlayerName(const std::string& name) {
    if (!name.empty()) {
        _playerName = name;
        loadData();
    }
}

/**
 * @brief Get the current player's name
 * @return current player name
 */
std::string PlayerManager::getPlayerName() const {
    return _playerName;
}

/**
 * @brief Check if a player name has been set
 * @return true if player name exists, false otherwise
 */
bool PlayerManager::hasPlayerName() const {
    return !_playerName.empty();
}

/**
 * @brief Add a new score for the current player
 * @param gameName name of the game
 * @param score score achieved
 */
void PlayerManager::addScore(const std::string& gameName, int score) {
    GameScore newScore{gameName, score, getCurrentDate()};
    _scores[gameName].push_back(newScore);
    
    std::sort(_scores[gameName].begin(), _scores[gameName].end(), 
             [](const GameScore& a, const GameScore& b) {
                 return a.score > b.score;
             });
             
    saveData();
}

/**
 * @brief Get the best score for a specific game
 * @param gameName name of the game
 * @return best score, or 0 if no scores exist
 */
int PlayerManager::getBestScore(const std::string& gameName) const {
    auto it = _scores.find(gameName);
    if (it != _scores.end() && !it->second.empty()) {
        return it->second[0].score;
    }
    return 0;
}

/**
 * @brief Get all scores for a specific game
 * @param gameName name of the game
 * @return vector of GameScore objects for that game
 */
std::vector<GameScore> PlayerManager::getScores(const std::string& gameName) const {
    auto it = _scores.find(gameName);
    if (it != _scores.end()) {
        return it->second;
    }
    return {};
}

/**
 * @brief Get all scores for all games
 * @return vector of all GameScore objects
 */
std::vector<GameScore> PlayerManager::getAllScores() const {
    std::vector<GameScore> allScores;
    for (const auto& [game, scores] : _scores) {
        allScores.insert(allScores.end(), scores.begin(), scores.end());
    }
    
    std::sort(allScores.begin(), allScores.end(), 
             [](const GameScore& a, const GameScore& b) {
                 return a.score > b.score;
             });
             
    return allScores;
}

/**
 * @brief Save player data to persistent storage
 * @return true if save succeeded, false otherwise
 */
bool PlayerManager::saveData() {
    try {
        std::ofstream file(getPlayerFilePath());
        if (!file.is_open()) {
            return false;
        }
        
        file << _playerName << std::endl;
        for (const auto& [game, scores] : _scores) {
            for (const auto& score : scores) {
                file << score.gameName << "," << score.score << "," << score.date << std::endl;
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving player data: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Load player data from persistent storage
 * @return true if load succeeded, false otherwise
 */
bool PlayerManager::loadData() {
    try {
        std::ifstream file(getPlayerFilePath());
        if (!file.is_open()) {
            return false;
        }
        
        _scores.clear();
        
        std::string line;
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string gameName, scoreStr, date;
            
            std::getline(iss, gameName, ',');
            std::getline(iss, scoreStr, ',');
            std::getline(iss, date);
            
            int score = std::stoi(scoreStr);
            _scores[gameName].push_back({gameName, score, date});
        }
        
        for (auto& [game, scores] : _scores) {
            std::sort(scores.begin(), scores.end(), 
                     [](const GameScore& a, const GameScore& b) {
                         return a.score > b.score;
                     });
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading player data: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Factory function to create a PlayerManager instance
 * @return unique_ptr to a new PlayerManager
 */
extern "C" {
    std::unique_ptr<IPlayerManager> createPlayerManager() {
        return std::make_unique<PlayerManager>();
    }
}

} // namespace arcd