#include "noteapp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

NoteApp::NoteApp() : mainWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Notes - Dracula Theme") {
    if (!font.loadFromFile("/home/ramimohamed/.local/share/fonts/Ubuntu-M.ttf")) {
        throw std::runtime_error("Could not load font");
    }
    
    setupUI();
    loadNotes();
    mainWindow.setFramerateLimit(60);
}

void NoteApp::setupUI() {
    // Setup sidebar
    sidebar.setSize(sf::Vector2f(SIDEBAR_WIDTH, WINDOW_HEIGHT));
    sidebar.setFillColor(CURRENT_LINE);
    
    // Setup buttons
    std::vector<std::string> buttonLabels = {"New Note", "Save", "Delete"};
    float buttonHeight = 50.0f;
    float buttonMargin = 10.0f;

    for (size_t i = 0; i < buttonLabels.size(); i++) {
        sf::RectangleShape button(sf::Vector2f(SIDEBAR_WIDTH - 20, buttonHeight));
        button.setPosition(10, 10 + i * (buttonHeight + buttonMargin));
        button.setFillColor(PURPLE);
        buttons.push_back(button);

        sf::Text text;
        text.setFont(font);
        text.setString(buttonLabels[i]);
        text.setCharacterSize(20);
        text.setFillColor(FOREGROUND);
        text.setPosition(
            button.getPosition().x + 10,
            button.getPosition().y + (buttonHeight - text.getCharacterSize()) / 2
        );
        buttonTexts.push_back(text);
    }
}

void NoteApp::createNewNote() {
    sf::RenderWindow noteWindow(sf::VideoMode(800, 600), "New Note");
    std::string content;
    
    sf::Text noteText;
    noteText.setFont(font);
    noteText.setCharacterSize(20);
    noteText.setFillColor(FOREGROUND);
    noteText.setPosition(10, 10);

    while (noteWindow.isOpen()) {
        sf::Event event;
        while (noteWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (!content.empty()) {
                    saveCurrentNote(content);
                }
                noteWindow.close();
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

        noteWindow.clear(BACKGROUND);
        noteWindow.draw(noteText);
        noteWindow.display();
    }
}

void NoteApp::saveCurrentNote(const std::string& content) {
    std::string filename = "note_" + std::to_string(time(nullptr)) + ".txt";
    std::ofstream file("notes/" + filename);
    if (file.is_open()) {
        file << content;
        file.close();
        loadNotes(); // Refresh notes list
    }
}

void NoteApp::loadNotes() {
    notes.clear();
    for (const auto& entry : std::filesystem::directory_iterator("notes")) {
        if (entry.path().extension() == ".txt") {
            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                Note note;
                note.content = buffer.str();
                note.filename = entry.path().filename();
                notes.push_back(note);
                file.close();
            }
        }
    }
    updateNotesList();
}

void NoteApp::updateNotesList() {
    noteCards.clear();
    float cardHeight = 80.0f;
    float cardMargin = 10.0f;
    float startX = SIDEBAR_WIDTH + 10;
    
    for (size_t i = 0; i < notes.size(); i++) {
        sf::RectangleShape card(sf::Vector2f(WINDOW_WIDTH - SIDEBAR_WIDTH - 20, cardHeight));
        card.setPosition(startX, 10 + i * (cardHeight + cardMargin));
        card.setFillColor(CURRENT_LINE);
        noteCards.push_back(card);
    }
}

void NoteApp::handleClick(int x, int y) {
    // Handle button clicks
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i].getGlobalBounds().contains(x, y)) {
            switch(i) {
                case 0: createNewNote(); break;
                case 1: if (selectedNoteIndex >= 0) saveCurrentNote(notes[selectedNoteIndex].content); break;
                case 2: if (selectedNoteIndex >= 0) deleteSelectedNote(); break;
            }
            return;
        }
    }

    // Handle note selection
    for (size_t i = 0; i < noteCards.size(); i++) {
        if (noteCards[i].getGlobalBounds().contains(x, y)) {
            selectedNoteIndex = i;
            return;
        }
    }
}

void NoteApp::deleteSelectedNote() {
    if (selectedNoteIndex >= 0 && selectedNoteIndex < notes.size()) {
        std::filesystem::remove("notes/" + notes[selectedNoteIndex].filename);
        loadNotes();
        selectedNoteIndex = -1;
    }
}

void NoteApp::draw() {
    mainWindow.clear(BACKGROUND);

    // Draw sidebar
    mainWindow.draw(sidebar);

    // Draw buttons
    for (size_t i = 0; i < buttons.size(); i++) {
        mainWindow.draw(buttons[i]);
        mainWindow.draw(buttonTexts[i]);
    }

    // Draw note cards
    for (size_t i = 0; i < noteCards.size(); i++) {
        mainWindow.draw(noteCards[i]);
        
        // Draw note preview
        sf::Text preview;
        preview.setFont(font);
        preview.setString(notes[i].content.substr(0, 50) + "...");
        preview.setCharacterSize(16);
        preview.setFillColor(FOREGROUND);
        preview.setPosition(
            noteCards[i].getPosition().x + 10,
            noteCards[i].getPosition().y + 10
        );
        mainWindow.draw(preview);
    }

    mainWindow.display();
}

void NoteApp::run() {
    while (mainWindow.isOpen()) {
        sf::Event event;
        while (mainWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mainWindow.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                handleClick(event.mouseButton.x, event.mouseButton.y);
            }
        }

        draw();
    }
}
