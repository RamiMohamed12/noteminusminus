#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

class NoteApp {
private:
    sf::RenderWindow mainWindow;
    sf::RenderWindow* noteWindow;
    sf::RectangleShape newNoteButton;
    sf::Font font;
    sf::Text buttonText;
    std::string notesPath;
    const int WINDOW_WIDTH = 1366;
    const int WINDOW_HEIGHT = 768;

    void createNoteWindow();
    void saveNote(const std::string& content);
    std::string loadNote();

public:
    NoteApp();
    void run();
};
