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

void ScoreManager::addScore(const std::string& playerName, const std::string& gameName, int score)
{
    Score newScore(playerName, gameName, score);
    _scores[gameName].push_back(newScore);
    
    // Sort scores in descending order
    std::sort(_scores[gameName].begin(), _scores[gameName].end(), 
              [](const Score& a, const Score& b) { return a.value > b.value; });
}

std::vector<Score> ScoreManager::getScores(const std::string& gameName) const
{
    auto it = _scores.find(gameName);
    if (it != _scores.end()) {
        auto scores = it->second;
        // Sort scores in descending order in case the original ordering was lost
        std::sort(scores.begin(), scores.end(), 
                 [](const Score& a, const Score& b) { return a.value > b.value; });
        return scores;
    }
    return {};
}

Score ScoreManager::getBestScore(const std::string& gameName) const
{
    auto scores = getScores(gameName);
    if (!scores.empty()) {
        return scores[0]; // Already sorted in descending order
    }
    return Score("", gameName, 0);
}

std::map<std::string, std::vector<Score>> ScoreManager::getAllScores() const
{
    return _scores;
}

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
