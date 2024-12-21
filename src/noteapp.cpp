#include "noteapp.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <sstream>

NoteApp::NoteApp() 
    : mainWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Note--")
    , selectedNoteIndex(-1)
    , isEditing(false) {
    
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
    sidebar.setPosition(0, 0);
    
    // Setup edit area
    editArea.setSize(sf::Vector2f(WINDOW_WIDTH - SIDEBAR_WIDTH, WINDOW_HEIGHT));
    editArea.setFillColor(BACKGROUND);
    editArea.setPosition(SIDEBAR_WIDTH, 0);
    
    // Setup buttons
    std::vector<std::string> buttonLabels = {"New Note", "Save", "Load Notes"};
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

void NoteApp::handleInput(sf::Event& event) {
    if (event.type == sf::Event::TextEntered && isEditing) {
        if (event.text.unicode == '\b' && !currentNoteContent.empty()) {
            currentNoteContent.pop_back();
        }
        else if (event.text.unicode < 128 && event.text.unicode != '\b') {
            currentNoteContent += static_cast<char>(event.text.unicode);
        }
    }
}

void NoteApp::createNewNote() {
    currentNoteContent = "";
    isEditing = true;
    selectedNoteIndex = -1;
}

void NoteApp::saveCurrentNote() {
    if (!currentNoteContent.empty()) {
        Note note;
        note.content = currentNoteContent;
        note.timestamp = std::to_string(time(nullptr));
        note.id = "note_" + note.timestamp;
        note.title = "Note " + note.timestamp;
        
        if (selectedNoteIndex >= 0 && selectedNoteIndex < notes.size()) {
            notes[selectedNoteIndex] = note;
        } else {
            notes.push_back(note);
        }
        
        // Save to file
        std::filesystem::create_directory("notes");
        std::string filepath = "notes/" + note.id + ".txt";
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << note.content;
            file.close();
        }
        
        updateNotesList();
    }
}

void NoteApp::loadNotes() {
    notes.clear();
    std::string notesDir = "notes/";
    
    if (!std::filesystem::exists(notesDir)) {
        std::filesystem::create_directory(notesDir);
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(notesDir)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            if (file.is_open()) {
                Note note;
                std::stringstream buffer;
                buffer << file.rdbuf();
                note.content = buffer.str();
                note.id = entry.path().stem().string();
                note.timestamp = note.id.substr(5); // Remove "note_" prefix
                note.title = "Note " + note.timestamp;
                notes.push_back(note);
                file.close();
            }
        }
    }
    updateNotesList();
}

void NoteApp::updateNotesList() {
    noteListItems.clear();
    noteListTexts.clear();
    
    float itemHeight = 50.0f;
    float itemMargin = 5.0f;
    float startY = 180.0f;
    
    for (size_t i = 0; i < notes.size(); i++) {
        sf::RectangleShape item(sf::Vector2f(SIDEBAR_WIDTH - 20, itemHeight));
        item.setPosition(10, startY + i * (itemHeight + itemMargin));
        item.setFillColor(CURRENT_LINE);
        if (i == selectedNoteIndex) {
            item.setFillColor(PURPLE);
        }
        noteListItems.push_back(item);
        
        sf::Text text;
        text.setFont(font);
        text.setString(notes[i].title);
        text.setCharacterSize(16);
        text.setFillColor(FOREGROUND);
        text.setPosition(20, startY + i * (itemHeight + itemMargin) + 15);
        noteListTexts.push_back(text);
    }
}

void NoteApp::draw() {
    mainWindow.clear(BACKGROUND);
    
    mainWindow.draw(sidebar);
    mainWindow.draw(editArea);
    
    // Draw buttons
    for (size_t i = 0; i < buttons.size(); i++) {
        mainWindow.draw(buttons[i]);
        mainWindow.draw(buttonTexts[i]);
    }
    
    // Draw note list
    for (size_t i = 0; i < noteListItems.size(); i++) {
        mainWindow.draw(noteListItems[i]);
        mainWindow.draw(noteListTexts[i]);
    }
    
    // Draw edit area content
    if (isEditing) {
        sf::Text content;
        content.setFont(font);
        content.setString(currentNoteContent);
        content.setCharacterSize(16);
        content.setFillColor(FOREGROUND);
        content.setPosition(SIDEBAR_WIDTH + 20, 20);
        mainWindow.draw(content);
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
            
            handleInput(event);
            
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(mainWindow);
                
                // Check button clicks
                for (size_t i = 0; i < buttons.size(); i++) {
                    if (buttons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        switch(i) {
                            case 0: createNewNote(); break;
                            case 1: saveCurrentNote(); break;
                            case 2: loadNotes(); break;
                        }
                    }
                }
                
                // Check note list clicks
                for (size_t i = 0; i < noteListItems.size(); i++) {
                    if (noteListItems[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedNoteIndex = i;
                        currentNoteContent = notes[i].content;
                        isEditing = true;
                        updateNotesList();
                    }
                }
            }
        }
        
        draw();
    }
}
