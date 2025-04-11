/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** KeyCodes - Standard key codes for input handling
*/

#ifndef KEY_CODES_HPP
#define KEY_CODES_HPP

namespace arcd {

namespace KeyCode {
    // Control keys
    constexpr int ESC = 27;
    constexpr int ENTER = 10;
    constexpr int SPACE = 32;
    constexpr int BACKSPACE = 127;
    constexpr int TAB = 9;
    
    // Arrow keys
    constexpr int UP = 259;
    constexpr int DOWN = 258;
    constexpr int LEFT = 260;
    constexpr int RIGHT = 261;
    
    // Function keys
    constexpr int F1 = 265;
    constexpr int F2 = 266;
    constexpr int F3 = 267;
    constexpr int F4 = 268;
    constexpr int F5 = 269;
    constexpr int F6 = 270;
    constexpr int F7 = 271;
    constexpr int F8 = 272;
    constexpr int F9 = 273;
    constexpr int F10 = 274;
    constexpr int F11 = 275;
    constexpr int F12 = 276;
    
    // Special keys for arcade
    constexpr int NEXT_GAME = '7';
    constexpr int PREV_GAME = '8';
    constexpr int NEXT_LIB = '9';
    constexpr int PREV_LIB = '0';
    constexpr int RESTART = 'r';
    constexpr int QUIT = 'q';
    constexpr int PAUSE = 'p';
}

} // namespace arcd

#endif // KEY_CODES_HPP 