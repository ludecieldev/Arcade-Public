/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** PlayerManager
*/

#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "../interfaces/IPlayerManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <fstream>

namespace arcd {

    class PlayerManager : public IPlayerManager {
        private:
            std::string _playerName;
            std::map<std::string, std::vector<GameScore>> _scores;
            const std::string _saveDirectory;
            const std::string _saveExtension;

            // Helper methods
            std::string getCurrentDate() const;
            std::string getPlayerFilePath() const;

        public:
            PlayerManager();
            ~PlayerManager() override = default;

            // Player name management
            void setPlayerName(const std::string& name) override;
            std::string getPlayerName() const override;
            bool hasPlayerName() const override;

            // Score management
            void addScore(const std::string& gameName, int score) override;
            int getBestScore(const std::string& gameName) const override;
            std::vector<GameScore> getScores(const std::string& gameName) const override;
            std::vector<GameScore> getAllScores() const override;

            // Save/load player data
            bool saveData() override;
            bool loadData() override;
    };

}

#endif // PLAYER_MANAGER_HPP