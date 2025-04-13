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

    /**
     * @brief Structure to store a player's score for a specific game
     */
    struct GameScore {
        std::string gameName;  
        int score;             
        std::string date;      
    };

    /**
     * @brief Interface for managing player data and scores
     */
    class IPlayerManager {
        public:
            /**
             * @brief Virtual destructor
             */
            virtual ~IPlayerManager() = default;

            /**
             * @brief Set the player's name
             * @param name player name to set
             */
            virtual void setPlayerName(const std::string& name) = 0;
            
            /**
             * @brief Get the current player's name
             * @return current player name
             */
            virtual std::string getPlayerName() const = 0;
            
            /**
             * @brief Check if a player name has been set
             * @return true if player name exists, false otherwise
             */
            virtual bool hasPlayerName() const = 0;

            /**
             * @brief Add a new score for the current player
             * @param gameName name of the game
             * @param score score achieved
             */
            virtual void addScore(const std::string& gameName, int score) = 0;
            
            /**
             * @brief Get the best score for a specific game
             * @param gameName name of the game
             * @return best score, or 0 if no scores exist
             */
            virtual int getBestScore(const std::string& gameName) const = 0;
            
            /**
             * @brief Get all scores for a specific game
             * @param gameName name of the game
             * @return vector of GameScore objects for that game
             */
            virtual std::vector<GameScore> getScores(const std::string& gameName) const = 0;
            
            /**
             * @brief Get all scores for all games
             * @return vector of all GameScore objects
             */
            virtual std::vector<GameScore> getAllScores() const = 0;

            /**
             * @brief Save player data to persistent storage
             * @return true if save succeeded, false otherwise
             */
            virtual bool saveData() = 0;
            
            /**
             * @brief Load player data from persistent storage
             * @return true if load succeeded, false otherwise
             */
            virtual bool loadData() = 0;
    };

}

/**
 * @brief Factory functions for creating and destroying player manager instances
 */
extern "C" {
    /**
     * @brief Create a new player manager instance
     * @return unique pointer to the created player manager
     */
    std::unique_ptr<arcd::IPlayerManager> createPlayerManager();
    
    /**
     * @brief Destroy a player manager instance (kept for compatibility)
     * @param manager pointer to the player manager to destroy
     */
    void destroyPlayerManager(arcd::IPlayerManager* manager);
}

#endif // IPLAYER_MANAGER_HPP