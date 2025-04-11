/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** IEvent - Interface for input events
*/

#ifndef I_EVENT_HPP
#define I_EVENT_HPP

#include <string>
#include <memory>
#include <map>
#include <any>

namespace arcd {

// Event types
enum class EventType {
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_PRESSED,
    MOUSE_RELEASED,
    MOUSE_MOVED,
    WINDOW_RESIZED,
    CUSTOM
};

// Event class
class IEvent {
public:
    virtual ~IEvent() = default;

    // Event type
    virtual EventType getType() const = 0;
    
    // Key events
    virtual int getKeyCode() const = 0;
    
    // Mouse events
    virtual int getMouseX() const = 0;
    virtual int getMouseY() const = 0;
    virtual int getMouseButton() const = 0; // Left=1, Right=2, Middle=3
    
    // Window events
    virtual int getWindowWidth() const = 0;
    virtual int getWindowHeight() const = 0;
    
    // Custom data
    virtual std::any getCustomData(const std::string& key) const = 0;
    
    // Create events factory methods
    static std::unique_ptr<IEvent> createKeyEvent(int keyCode, bool isPressed);
    static std::unique_ptr<IEvent> createMouseEvent(int x, int y, int button, EventType type);
    static std::unique_ptr<IEvent> createWindowEvent(int width, int height);
    static std::unique_ptr<IEvent> createCustomEvent(const std::map<std::string, std::any>& data);
};

} // namespace arcd

#endif // I_EVENT_HPP 