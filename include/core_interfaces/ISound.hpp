/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for sound management
*/

#ifndef ISOUND_HPP_
    #define ISOUND_HPP_

#include <string>

/**
 * @namespace arcd
 * Namespace used to group all interfaces and classes related to the Arcade project.
 */
namespace arcd {

    /**
     * @brief Interface for managing sounds.
     *
     * This interface defines methods for loading, playing, stopping, and adjusting sound volume.
     */
    class ISound {
        public:
            virtual ~ISound() = default;

            /**
             * @brief Loads a sound from a file.
             *
             * @param path The file path to the sound.
             */
            virtual void loadSound(const std::string &path) = 0;

            /**
             * @brief Plays a loaded sound.
             *
             * @param id The ID of the sound to play.
             */
            virtual void playSound(int id) = 0;

            /**
             * @brief Stops a currently playing sound.
             *
             * @param id The ID of the sound to stop.
             */
            virtual void stopSound(int id) = 0;

            /**
             * @brief Adjusts the volume of a sound.
             *
             * @param id The ID of the sound whose volume will be adjusted.
             * @param volume The new volume level (0-100).
             */
            virtual void setVolume(int id, int volume) = 0;
    };

}

#endif /* !ISOUND_HPP_ */
