/*
** EPITECH PROJECT, 2025
** Arcade
** File description:
** Interface for managing graphical libraries
*/

#ifndef ILIBRARY_HPP_
    #define ILIBRARY_HPP_

#include "IDisplay.hpp"
#include "ITexture.hpp"
#include "ISound.hpp"
#include "IFont.hpp"

/**
 * @namespace arcd
 * Namespace used to group all interfaces and classes related to the Arcade project.
 */
namespace arcd {

    /**
     * @brief Interface for managing graphical libraries.
     *
     * This interface provides access to all the functionalities offered by a graphical library,
     * such as display, textures, sounds, and fonts. It acts as a central point for interacting
     * with the library's features.
     */
    class ILibrary {
        public:
            virtual ~ILibrary() = default;

            /**
             * @brief Provides access to the display functionality.
             *
             * @return A reference to the IDisplay interface implemented by the library.
             */
            virtual IDisplay &display() = 0;

            /**
             * @brief Provides access to the texture functionality.
             *
             * @return A pointer to the ITexture interface implemented by the library, or nullptr if not supported.
             */
            virtual std::shared_ptr<ITexture> texture()
            {
                return nullptr;
            }

            /**
             * @brief Provides access to the sound functionality.
             *
             * @return A pointer to the ISound interface implemented by the library, or nullptr if not supported.
             */
            virtual std::shared_ptr<ISound> sound()
            {
                return nullptr;
            }

            /**
             * @brief Provides access to the font functionality.
             *
             * @return A pointer to the IFont interface implemented by the library, or nullptr if not supported.
             */
            virtual std::shared_ptr<IFont> font()
            {
                return nullptr;
            }

            /**
             * @brief Retrieves the name of the library.
             *
             * @return The name of the library as a string.
             */
            virtual const std::string &getName() const = 0;
    };

}

#endif /* !ILIBRARY_HPP_ */
