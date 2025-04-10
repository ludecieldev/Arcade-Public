/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IPlayerManager
*/

#ifndef IPLAYER_MANAGER_HPP
#define IPLAYER_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace arcd {

    struct GameScore {
        std::string gameName;
        int score;
        std::string date;
    };

    class IPlayerManager {
        public:
            virtual ~IPlayerManager() = default;

            // Player name management
            virtual void setPlayerName(const std::string& name) = 0;
            virtual std::string getPlayerName() const = 0;
            virtual bool hasPlayerName() const = 0;

            // Score management
            virtual void addScore(const std::string& gameName, int score) = 0;
            virtual int getBestScore(const std::string& gameName) const = 0;
            virtual std::vector<GameScore> getScores(const std::string& gameName) const = 0;
            virtual std::vector<GameScore> getAllScores() const = 0;

            // Save/load player data
            virtual bool saveData() = 0;
            virtual bool loadData() = 0;
    };

}

// Dynamic library entry points with smart pointers
extern "C" {
    std::unique_ptr<arcd::IPlayerManager> createPlayerManager();
    void destroyPlayerManager(arcd::IPlayerManager* manager); // Keep for compatibility
}

#endif // IPLAYER_MANAGER_HPP