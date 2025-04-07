class SFMLGraphics : public IGraphicsLibrary {
    private:
        bool _initialized;
        int _width;
        int _height;
        int _lastKey;
        int _frameCounter;  // For animations
        int _selectedMenuItem; // Pour stocker l'élément de menu sélectionné
        sf::RenderWindow _window;
        sf::Font _font;
        std::map<int, sf::Color> _colors;
        std::optional<std::reference_wrapper<Core>> _core;
}; 