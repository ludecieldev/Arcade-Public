/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IGraphic
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace arcd {

/**
 * @brief Structure representing an input event
 */
struct Input {
    /**
     * @brief Type of input event
     */
    enum class Type {
        KEYBOARD,  
        MOUSE,
        NONE       
    };

    Type type;     
    int key;       
    int x;         
    int y;         
    bool pressed;  
};

/**
 * @brief Structure representing the visual state of a game
 */
struct GameState {
    /**
     * @brief Structure representing a visual entity to be rendered
     */
    struct Entity {
        /**
         * @brief Type of visual entity
         */
        enum class Type {
            RECTANGLE,  
            CIRCLE,     
            TEXT,       
            SPRITE      
        };

        Type type;                    
        float x;                      
        float y;                      
        float width;                  
        float height;                 
        std::string text;             
        std::vector<unsigned char> color;
    };

    std::vector<Entity> entities;  
    int score;                     
    std::string playerName;        
};

/**
 * @brief Interface for graphics rendering libraries
 */
class IGraphic {
public:
    /**
     * @brief Smart pointer type for IGraphic with custom deleter
     */
    using Ptr = std::unique_ptr<IGraphic, void(*)(IGraphic*)>;

    /**
     * @brief Virtual destructor
     */
    virtual ~IGraphic() = default;

    /**
     * @brief Initialize the graphics library
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Clean up resources used by the graphics library
     */
    virtual void cleanup() = 0;

    /**
     * @brief Get the current input event
     * @return Input structure with event details
     */
    virtual Input getInput() = 0;

    /**
     * @brief Render the current game state
     * @param state current game state to render
     */
    virtual void render(const GameState& state) = 0;
    
    /**
     * @brief Clear the display
     */
    virtual void clear() = 0;
    
    /**
     * @brief Update the display with rendered content
     */
    virtual void display() = 0;

    /**
     * @brief Get the width of the display
     * @return width in pixels
     */
    virtual int getWidth() const = 0;
    
    /**
     * @brief Get the height of the display
     * @return height in pixels
     */
    virtual int getHeight() const = 0;
    
    /**
     * @brief Set the window title
     * @param title new window title
     */
    virtual void setTitle(const std::string& title) = 0;

    /**
     * @brief Get the player's name via user input
     * @param playerName reference to string to store the player name
     */
    virtual void getPlayerName(std::string& playerName) = 0;

    /**
     * @brief Get the name of the graphics library
     * @return name of the graphics library
     */
    virtual std::string getName() const = 0;
};

} // namespace arcd

/**
 * @brief Factory functions for creating and destroying graphic library instances
 */
extern "C" {
    /**
     * @brief Create a new graphic library instance
     * @return pointer to the created graphic library
     */
    arcd::IGraphic* create_graphic();
    
    /**
     * @brief Destroy a graphic library instance
     * @param graphic pointer to the graphic library to destroy
     */
    void destroy_graphic(arcd::IGraphic*);
} 