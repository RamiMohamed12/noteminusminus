#include "noteapp.h"
#include <iostream>

NoteApp::NoteApp() : mainWindow(sf::VideoMode(WINDOW_WIDTH/3, WINDOW_HEIGHT/3), "Note App"), 
                     noteWindow(nullptr) {
    notesPath = "notes.txt";
    
    if (!font.loadFromFile("/home/ramimohamed/.local/share/fonts/Ubuntu-M.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // Center button in window
    newNoteButton.setSize(sf::Vector2f(200, 50));
    newNoteButton.setPosition((WINDOW_WIDTH/3 - 200)/2, (WINDOW_HEIGHT/3 - 50)/2);
    newNoteButton.setFillColor(sf::Color::Green);

    buttonText.setFont(font);
    buttonText.setString("New Note");
    buttonText.setCharacterSize(24);
    buttonText.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        newNoteButton.getPosition().x + (newNoteButton.getSize().x - textBounds.width) / 2,
        newNoteButton.getPosition().y + (newNoteButton.getSize().y - textBounds.height) / 2
    );
}

void NoteApp::createNoteWindow() {
    if (noteWindow != nullptr) {
        delete noteWindow;
    }
    noteWindow = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), "New Note");
    noteWindow->setPosition(sf::Vector2i(WINDOW_WIDTH/4, WINDOW_HEIGHT/4));
    
    std::string content = loadNote();
    sf::Text noteText;
    noteText.setFont(font);
    noteText.setCharacterSize(20);
    noteText.setFillColor(sf::Color::Black);
    noteText.setPosition(10, 10);
    noteText.setString(content);

    while (noteWindow->isOpen()) {
        sf::Event event;
        while (noteWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                saveNote(noteText.getString());
                noteWindow->close();
                delete noteWindow;
                noteWindow = nullptr;
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && content.size() > 0) {
                    content.pop_back();
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    content += static_cast<char>(event.text.unicode);
                }
                noteText.setString(content);
            }
        }

        noteWindow->clear(sf::Color::White);
        noteWindow->draw(noteText);
        noteWindow->display();
    }
}

void NoteApp::saveNote(const std::string& content) {
    std::ofstream file(notesPath);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

std::string NoteApp::loadNote() {
    std::string content;
    std::ifstream file(notesPath);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        file.close();
    }
    return content;
}

void NoteApp::run() {
    while (mainWindow.isOpen()) {
        sf::Event event;
        while (mainWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mainWindow.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(mainWindow);
                    if (newNoteButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        createNoteWindow();
                    }
                }
            }
        }

        mainWindow.clear(sf::Color::White);
        mainWindow.draw(newNoteButton);
        mainWindow.draw(buttonText);
        mainWindow.display();
    }
}
