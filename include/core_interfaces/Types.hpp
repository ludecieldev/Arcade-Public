/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Common types for the Arcade project
*/

#ifndef TYPES_HPP_
    #define TYPES_HPP_

namespace arcd {

    /**
     * @brief Enumeration of possible keyboard events.
     *
     * This enum defines all possible keyboard events that can be
     * captured by the graphics libraries and processed by the games.
     */
    enum class KeysEvent {
        None,
        KeyUp,
        KeyDown,
        KeyLeft,
        KeyRight,
        KeyA,
        KeyB,
        KeyC,
        KeyD,
        KeyE,
        KeyF,
        KeyG,
        KeyH,
        KeyI,
        KeyJ,
        KeyK,
        KeyL,
        KeyM,
        KeyN,
        KeyO,
        KeyP,
        KeyQ,
        KeyR,
        KeyS,
        KeyT,
        KeyU,
        KeyV,
        KeyW,
        KeyX,
        KeyY,
        KeyZ,
        KeySpace,
        KeyEnter,
        KeyEscape,
        KeyTab,
        KeyBackspace
    };

    /**
     * @brief Enumeration of possible shapes to draw.
     *
     * This enum defines all shapes that can be drawn by the graphics libraries.
     */
    enum class ShapeType {
        Rectangle,
        Circle,
        Line,
        Point,
        Triangle
    };

    /**
     * @brief Structure representing a color in RGBA format.
     */
    struct Color {
        unsigned char r; // (0-255)
        unsigned char g; // ditto
        unsigned char b; // ditto
        unsigned char a; // ditto (for opacity here)
    };

    /**
     * @brief Structure representing a 2D position.
     */
    struct Vector2i {
        int x;
        int y;
    };

}

#endif /* !TYPES_HPP_ */
