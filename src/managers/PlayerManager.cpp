/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** PlayerManager - Manages player data
*/

#include "interfaces/IPlayerManager.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <algorithm>

namespace arcd {

class PlayerManager : public IPlayerManager {
private:
    std::string _playerName;
    std::map<std::string, std::vector<GameScore>> _scores;
    const std::string _saveDirectory;
    
    std::string getCurrentDate() const {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string getPlayerFilePath() const {
        return _saveDirectory + _playerName + ".save";
    }

public:
    PlayerManager() : _playerName("Player"), _saveDirectory("./scores/") {
        std::filesystem::create_directories(_saveDirectory);
    }
    
    ~PlayerManager() override = default;

    void setPlayerName(const std::string& name) override {
        if (!name.empty()) {
            _playerName = name;
            loadData(); // Try to load existing data for this player
        }
    }
    
    std::string getPlayerName() const override {
        return _playerName;
    }
    
    bool hasPlayerName() const override {
        return !_playerName.empty();
    }

    void addScore(const std::string& gameName, int score) override {
        GameScore newScore{gameName, score, getCurrentDate()};
        _scores[gameName].push_back(newScore);
        
        // Sort scores for this game (highest first)
        std::sort(_scores[gameName].begin(), _scores[gameName].end(), 
                 [](const GameScore& a, const GameScore& b) {
                     return a.score > b.score;
                 });
                 
        saveData();
    }
    
    int getBestScore(const std::string& gameName) const override {
        auto it = _scores.find(gameName);
        if (it != _scores.end() && !it->second.empty()) {
            return it->second[0].score;
        }
        return 0;
    }
    
    std::vector<GameScore> getScores(const std::string& gameName) const override {
        auto it = _scores.find(gameName);
        if (it != _scores.end()) {
            return it->second;
        }
        return {};
    }
    
    std::vector<GameScore> getAllScores() const override {
        std::vector<GameScore> allScores;
        for (const auto& [game, scores] : _scores) {
            allScores.insert(allScores.end(), scores.begin(), scores.end());
        }
        
        // Sort all scores (highest first)
        std::sort(allScores.begin(), allScores.end(), 
                 [](const GameScore& a, const GameScore& b) {
                     return a.score > b.score;
                 });
                 
        return allScores;
    }
    
    bool saveData() override {
        try {
            std::ofstream file(getPlayerFilePath());
            if (!file.is_open()) {
                return false;
            }
            
            // Write player name
            file << _playerName << std::endl;
            
            // Write scores
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
    
    bool loadData() override {
        try {
            std::ifstream file(getPlayerFilePath());
            if (!file.is_open()) {
                return false;
            }
            
            // Clear existing scores
            _scores.clear();
            
            // Read player name (first line)
            std::string line;
            std::getline(file, line);
            // _playerName is already set by setPlayerName
            
            // Read scores
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string gameName, scoreStr, date;
                
                std::getline(iss, gameName, ',');
                std::getline(iss, scoreStr, ',');
                std::getline(iss, date);
                
                int score = std::stoi(scoreStr);
                _scores[gameName].push_back({gameName, score, date});
            }
            
            // Sort all game scores
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
};

// Factory function to create a PlayerManager
extern "C" {
    std::unique_ptr<IPlayerManager> createPlayerManager() {
        return std::make_unique<PlayerManager>();
    }
    
    void destroyPlayerManager([[maybe_unused]] IPlayerManager* manager) {
        // No need to delete with smart pointers
    }
}

} // namespace arcd