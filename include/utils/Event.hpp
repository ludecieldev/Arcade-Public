/*
** EPITECH PROJECT, 2025
** Arcade-Public
** File description:
** Event - Concrete implementation of IEvent
*/

#ifndef EVENT_HPP
#define EVENT_HPP

#include "../interfaces/IEvent.hpp"
#include <map>
#include <string>
#include <any>
#include <memory>

namespace arcd {

class Event : public IEvent {
private:
    EventType _type;
    int _keyCode;
    int _mouseX;
    int _mouseY;
    int _mouseButton;
    int _windowWidth;
    int _windowHeight;
    std::map<std::string, std::any> _customData;

public:
    // Key event constructor
    Event(int keyCode, bool isPressed)
        : _type(isPressed ? EventType::KEY_PRESSED : EventType::KEY_RELEASED),
          _keyCode(keyCode), _mouseX(0), _mouseY(0), _mouseButton(0),
          _windowWidth(0), _windowHeight(0) {}
    
    // Mouse event constructor
    Event(int x, int y, int button, EventType type)
        : _type(type), _keyCode(0), _mouseX(x), _mouseY(y), _mouseButton(button),
          _windowWidth(0), _windowHeight(0) {
        if (type != EventType::MOUSE_PRESSED && type != EventType::MOUSE_RELEASED && type != EventType::MOUSE_MOVED) {
            throw std::invalid_argument("Invalid event type for mouse event");
        }
    }
    
    // Window event constructor
    Event(int width, int height)
        : _type(EventType::WINDOW_RESIZED), _keyCode(0), _mouseX(0), _mouseY(0),
          _mouseButton(0), _windowWidth(width), _windowHeight(height) {}
    
    // Custom event constructor
    Event(const std::map<std::string, std::any>& data)
        : _type(EventType::CUSTOM), _keyCode(0), _mouseX(0), _mouseY(0),
          _mouseButton(0), _windowWidth(0), _windowHeight(0), _customData(data) {}
    
    ~Event() override = default;

    // Event type
    EventType getType() const override { return _type; }
    
    // Key events
    int getKeyCode() const override { return _keyCode; }
    
    // Mouse events
    int getMouseX() const override { return _mouseX; }
    int getMouseY() const override { return _mouseY; }
    int getMouseButton() const override { return _mouseButton; }
    
    // Window events
    int getWindowWidth() const override { return _windowWidth; }
    int getWindowHeight() const override { return _windowHeight; }
    
    // Custom data
    std::any getCustomData(const std::string& key) const override {
        auto it = _customData.find(key);
        return (it != _customData.end()) ? it->second : std::any();
    }
    
    // Add custom data
    void setCustomData(const std::string& key, const std::any& value) {
        _customData[key] = value;
    }
};

// Factory methods implementation
std::unique_ptr<IEvent> IEvent::createKeyEvent(int keyCode, bool isPressed) {
    return std::make_unique<Event>(keyCode, isPressed);
}

std::unique_ptr<IEvent> IEvent::createMouseEvent(int x, int y, int button, EventType type) {
    return std::make_unique<Event>(x, y, button, type);
}

std::unique_ptr<IEvent> IEvent::createWindowEvent(int width, int height) {
    return std::make_unique<Event>(width, height);
}

std::unique_ptr<IEvent> IEvent::createCustomEvent(const std::map<std::string, std::any>& data) {
    return std::make_unique<Event>(data);
}

} // namespace arcd

#endif // EVENT_HPP 