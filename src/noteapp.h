#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct Note {
    std::string content;
    std::string title;
    std::string id;
    std::string timestamp;
};

class NoteApp {
private:
    const int WINDOW_WIDTH = 1366;
    const int WINDOW_HEIGHT = 768;
    const float SIDEBAR_WIDTH = 280.0f;
    
    // Dracula theme colors
    const sf::Color BACKGROUND{40, 42, 54};
    const sf::Color CURRENT_LINE{68, 71, 90};
    const sf::Color FOREGROUND{248, 248, 242};
    const sf::Color COMMENT{98, 114, 164};
    const sf::Color PURPLE{189, 147, 249};
    
    sf::RenderWindow mainWindow;
    sf::RectangleShape sidebar;
    sf::RectangleShape editArea;
    sf::Font font;
    
    std::vector<Note> notes;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    std::vector<sf::RectangleShape> noteListItems;
    std::vector<sf::Text> noteListTexts;
    
    std::string currentNoteContent;
    int selectedNoteIndex;
    bool isEditing;
    
    void setupUI();
    void handleInput(sf::Event& event);
    void createNewNote();
    void saveCurrentNote(); // Changed signature
    void loadNotes();
    void updateNotesList();
    void draw();

public:
    NoteApp();
    void run();
};
