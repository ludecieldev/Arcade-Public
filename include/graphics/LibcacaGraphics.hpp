#ifndef LIBCACA_GRAPHICS_HPP
#define LIBCACA_GRAPHICS_HPP

#include "../interfaces/IGraphicsLibrary.hpp"
#include <caca.h>
#include <string>
#include <map>
#include <memory>

namespace arcd {
    /**
     * @brief Destructeur personnalisé pour les objets canvas de libcaca
     */
    struct CacaCanvasDeleter {
        void operator()(caca_canvas_t* c) { if (c) caca_free_canvas(c); }
    };

    /**
     * @brief Destructeur personnalisé pour les objets display de libcaca
     */
    struct CacaDisplayDeleter {
        void operator()(caca_display_t* d) { if (d) caca_free_display(d); }
    };

    /**
     * @brief Types personnalisés pour les smart pointers de libcaca
     */
    using CanvasPtr = std::unique_ptr<caca_canvas_t, CacaCanvasDeleter>;
    using DisplayPtr = std::unique_ptr<caca_display_t, CacaDisplayDeleter>;

    /**
     * @brief Implémentation de l'interface IGraphicsLibrary pour la bibliothèque libcaca
     * 
     * Cette classe encapsule les fonctionnalités de la bibliothèque libcaca
     * pour l'affichage en mode texte avec des couleurs et des effets visuels.
     */
    class LibcacaGraphics : public IGraphicsLibrary {
    public:
        /**
         * @brief Constructeur par défaut
         * 
         * Initialise les dimensions de la fenêtre et la carte des couleurs
         */
        LibcacaGraphics();
        
        /**
         * @brief Destructeur
         * 
         * Les smart pointers gèrent automatiquement la libération des ressources
         */
        ~LibcacaGraphics();

        /**
         * @brief Obtient le nom de la bibliothèque graphique
         * @return Le nom de la bibliothèque (libcaca)
         */
        std::string getName() const override;
        
        /**
         * @brief Initialise la bibliothèque graphique
         * @return true si l'initialisation a réussi, false sinon
         */
        bool initialize() override;
        
        /**
         * @brief Libère les ressources utilisées par la bibliothèque
         */
        void cleanup() override;
        
        /**
         * @brief Efface le contenu du canvas
         */
        void clear() override;
        
        /**
         * @brief Rafraîchit l'affichage pour refléter les changements
         */
        void refresh() override;
        
        /**
         * @brief Dessine du texte à une position spécifique
         * @param x Position horizontale
         * @param y Position verticale
         * @param text Texte à dessiner
         * @param color Couleur du texte
         */
        void drawText(int x, int y, const std::string& text, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Dessine une boîte rectangulaire
         * @param x Position horizontale du coin supérieur gauche
         * @param y Position verticale du coin supérieur gauche
         * @param width Largeur de la boîte
         * @param height Hauteur de la boîte
         * @param color Couleur de la boîte
         */
        void drawBox(int x, int y, int width, int height, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Dessine une liste d'éléments avec un élément sélectionné
         * @param x Position horizontale
         * @param y Position verticale
         * @param items Liste des éléments à afficher
         * @param selectedIndex Index de l'élément sélectionné
         * @param color Couleur des éléments non sélectionnés
         */
        void drawList(int x, int y, const std::vector<std::string>& items, int selectedIndex, Color color = Color::DEFAULT) override;
        
        /**
         * @brief Dessine le menu principal avec jeux, bibliothèques graphiques et options du joueur
         * @param title Titre du menu
         * @param games Liste des jeux disponibles
         * @param graphics Liste des bibliothèques graphiques disponibles
         * @param playerName Nom du joueur
         * @param selectedMenu Section sélectionnée (0=jeux, 1=graphiques, 2=joueur)
         * @param selectedGame Index du jeu sélectionné
         * @param selectedGraphic Index de la bibliothèque graphique sélectionnée
         * @param playerOptionSelected Option du joueur sélectionnée
         */
        void drawMenu(
            const std::string& title,
            const std::vector<std::string>& games,
            const std::vector<std::string>& graphics,
            const std::string& playerName,
            int selectedMenu,
            int selectedGame,
            int selectedGraphic,
            int playerOptionSelected
        ) override;
        
        /**
         * @brief Récupère une touche pressée par l'utilisateur
         * @return Code de la touche selon les constantes définies dans IGraphicsLibrary
         */
        int getKey() override;
        
        /**
         * @brief Permet à l'utilisateur de saisir son nom
         * @param playerName Référence au nom du joueur à modifier
         */
        void getPlayerName(std::string& playerName) override;
        
        /**
         * @brief Obtient la largeur de la fenêtre
         * @return Largeur en caractères
         */
        int getWidth() const override;
        
        /**
         * @brief Obtient la hauteur de la fenêtre
         * @return Hauteur en caractères
         */
        int getHeight() const override;

    private:
        /**
         * @brief Smart pointer vers le canvas libcaca
         */
        CanvasPtr canvas;
        
        /**
         * @brief Smart pointer vers le display libcaca
         */
        DisplayPtr display;
        
        /**
         * @brief Structure d'événement libcaca pour la gestion des entrées
         */
        caca_event_t event;
        
        /**
         * @brief Largeur de la fenêtre en caractères
         */
        int windowWidth;
        
        /**
         * @brief Hauteur de la fenêtre en caractères
         */
        int windowHeight;
        
        /**
         * @brief Correspondance entre les couleurs de l'enumération Color et les couleurs libcaca
         */
        std::map<Color, uint32_t> colorMap;

        /**
         * @brief Initialise la carte des couleurs
         */
        void initColorMap();
        
        /**
         * @brief Convertit une valeur Color en valeur de couleur libcaca
         * @param color Couleur à convertir
         * @return Valeur de couleur libcaca correspondante
         */
        uint32_t getColor(Color color) const;
        
        /**
         * @brief Définit les couleurs de premier plan et d'arrière-plan
         * @param fg Couleur de premier plan
         * @param bg Couleur d'arrière-plan
         */
        void setColorAnsi(uint32_t fg, uint32_t bg) const;
        
        /**
         * @brief Affiche un caractère à une position spécifique
         * @param x Position horizontale
         * @param y Position verticale
         * @param c Caractère à afficher
         */
        void putChar(int x, int y, char c) const;
        
        /**
         * @brief Affiche une chaîne de caractères à une position spécifique
         * @param x Position horizontale
         * @param y Position verticale
         * @param str Chaîne à afficher
         */
        void putStr(int x, int y, const std::string& str) const;
        
        /**
         * @brief Récupère un événement clavier et remplit la variable key
         * @param key Référence à la variable qui recevra le code de la touche
         * @return true si un événement clavier a été capturé, false sinon
         */
        bool getKeyEvent(int& key);
        
        /**
         * @brief Définit le titre de la fenêtre
         * @param title Titre à définir
         */
        void setDisplayTitle(const std::string& title) const;
        
        /**
         * @brief Obtient le code de touche à partir d'un événement
         * @param event Événement contenant le code de touche
         * @return Code de la touche
         */
        int getEventKeyCode(const caca_event_t* event) const;
    };
}

/**
 * @brief Fonctions C externes pour la création et destruction de la bibliothèque graphique
 */
extern "C" {
    /**
     * @brief Crée une instance de la bibliothèque graphique
     * @return Smart pointer vers l'instance créée
     */
    std::unique_ptr<arcd::IGraphicsLibrary> createGraphicsLibrary();
    
    /**
     * @brief Détruit une instance de la bibliothèque graphique
     * @param library Pointeur vers l'instance à détruire
     */
    void destroyGraphicsLibrary(arcd::IGraphicsLibrary* library);
}

#endif // LIBCACA_GRAPHICS_HPP 