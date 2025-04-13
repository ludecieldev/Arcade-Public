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

    /**
     * @brief Implementation of the IPlayerManager interface for managing player data
     */
    class PlayerManager : public IPlayerManager {
        private:
            std::string _playerName;
            std::map<std::string, std::vector<GameScore>> _scores;
            const std::string _saveDirectory;
            const std::string _saveExtension;

            /**
             * @brief Get the current date as a formatted string
             * @return string with current date
             */
            std::string getCurrentDate() const;
            
            /**
             * @brief Get the file path for the current player's save file
             * @return string with file path
             */
            std::string getPlayerFilePath() const;

        public:
            /**
             * @brief Constructor for PlayerManager
             */
            PlayerManager();
            
            /**
             * @brief Default destructor
             */
            ~PlayerManager() override = default;

            /**
             * @brief Set the player's name
             * @param name player name to set
             */
            void setPlayerName(const std::string& name) override;
            
            /**
             * @brief Get the current player's name
             * @return current player name
             */
            std::string getPlayerName() const override;
            
            /**
             * @brief Check if a player name has been set
             * @return true if player name exists, false otherwise
             */
            bool hasPlayerName() const override;

            /**
             * @brief Add a new score for the current player
             * @param gameName name of the game
             * @param score score achieved
             */
            void addScore(const std::string& gameName, int score) override;
            
            /**
             * @brief Get the best score for a specific game
             * @param gameName name of the game
             * @return best score, or 0 if no scores exist
             */
            int getBestScore(const std::string& gameName) const override;
            
            /**
             * @brief Get all scores for a specific game
             * @param gameName name of the game
             * @return vector of GameScore objects for that game
             */
            std::vector<GameScore> getScores(const std::string& gameName) const override;
            
            /**
             * @brief Get all scores for all games
             * @return vector of all GameScore objects
             */
            std::vector<GameScore> getAllScores() const override;

            /**
             * @brief Save player data to persistent storage
             * @return true if save succeeded, false otherwise
             */
            bool saveData() override;
            
            /**
             * @brief Load player data from persistent storage
             * @return true if load succeeded, false otherwise
             */
            bool loadData() override;
    };

}

#endif // PLAYER_MANAGER_HPP