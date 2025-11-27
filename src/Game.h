#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "Grid.h"
#include "Player.h"
#include "UI.h"
#include "Config.h"

// UI states
enum class UIState { MainMenu, Settings, Playing, Pause, LevelFail, LevelComplete, GameComplete };

// Game phases (within Playing)
enum class GamePhase { Planning, Executing };

// Difficulty
enum class Difficulty { Easy, Normal, Hard };

// Simple settings helper
struct Settings {
    Difficulty difficulty = Difficulty::Normal;
    int blocksPerTurn() const {
        switch (difficulty) {
            case Difficulty::Easy: return 3;
            case Difficulty::Normal: return 2;
            case Difficulty::Hard: return 1;
        }
        return 2;
    }
    int turnLimit() const {
        switch (difficulty) {
            case Difficulty::Easy: return -1; // infinite
            case Difficulty::Normal: return 5;
            case Difficulty::Hard: return 3;
        }
        return -1;
    }
    std::string difficultyName() const {
        switch (difficulty) {
            case Difficulty::Easy: return "Easy";
            case Difficulty::Normal: return "Normal";
            case Difficulty::Hard: return "Hard";
        }
        return "Normal";
    }
};

// Level runtime bookkeeping for turn-limited modes
struct LevelState {
    int initialTurns = -1;    // -1 => infinite
    int turnsRemaining = -1;
};


// Record one planning action (either a Move or a Block placement)
struct ActionRecord {
    bool isBlock = false;               // true => block placement, false => move
    Direction moveDir = Direction::Up;  // valid if isBlock == false
    sf::Vector2i blockPos{0,0};         // valid if isBlock == true
};

class Game {
public:
    Game();
    void run();

private:
    // event & update
    void handleEvent(const sf::Event& e);
    void update();
    void render();
    void updateLetterboxView(unsigned int newWidth, unsigned int newHeight);

    // UI drawing helpers
    void drawMainMenu();
    void drawSettingsMenu();
    void drawPauseMenu();
    void drawLevelFail();
    void drawLevelComplete();

    // playing loop helpers
    void handleInputPlaying(const sf::Event& e);
    void updatePlaying();
    void renderPlaying();
    void drawPlannedMoves();

    // level lifecycle
    void applyDifficulty();
    void startLevel(int index);
    void completeLevel();
    void failLevel();

    // helpers
    static std::string formatFloatTrim(float v, int precision = 1);
    static bool pointInRect(const sf::Vector2f& p, const sf::FloatRect& r);

    // Window / view / timing
    sf::RenderWindow window;
    sf::View view;
    sf::Font font;

    // HUD texts
    std::unique_ptr<sf::Text> timerText;
    std::unique_ptr<sf::Text> blocksLeftText;
    std::unique_ptr<sf::Text> turnsText;
    std::unique_ptr<sf::Text> tooltipText;
    std::unique_ptr<sf::Text> levelTitleText;
    std::unique_ptr<sf::Text> toastText;

    // core systems
    Grid grid;
    Player player;

    // game flow
    UIState uiState = UIState::MainMenu;
    GamePhase phase = GamePhase::Planning;
    float planningTime = 10.f;
    sf::Clock phaseClock;

    // levels
    std::vector<std::vector<std::string>> levels;
    int currentLevel = 0;

    // blocks / actions
    int blocksLeft = 3;
    std::vector<sf::Vector2i> placedBlocks;      // order of placements
    std::vector<ActionRecord> actionHistory;     // LIFO history for undo

    // settings & level state
    Settings settings;
    LevelState levelState;

    // toast timing
    sf::Clock toastClock;

    // frame delta for button animations
    sf::Clock frameDeltaClock;

    // UI buttons (persistent members) — use unique_ptr to construct after font is ready
    std::unique_ptr<ElevatedButton> mainPlayBtn;
    std::unique_ptr<ElevatedButton> mainSettingsBtn;
    std::unique_ptr<ElevatedButton> mainQuitBtn;

    std::unique_ptr<ElevatedButton> settingsEasyBtn;
    std::unique_ptr<ElevatedButton> settingsNormalBtn;
    std::unique_ptr<ElevatedButton> settingsHardBtn;

    std::unique_ptr<ElevatedButton> pauseResumeBtn;
    std::unique_ptr<ElevatedButton> pauseRestartBtn;
    std::unique_ptr<ElevatedButton> pauseSettingsBtn;
    std::unique_ptr<ElevatedButton> pauseMenuBtn;

    std::unique_ptr<ElevatedButton> failRetryBtn;
    std::unique_ptr<ElevatedButton> failMenuBtn;

    std::unique_ptr<ElevatedButton> completeNextBtn;
    std::unique_ptr<ElevatedButton> completeRetryBtn;
    std::unique_ptr<ElevatedButton> completeMenuBtn;

    // (optional) you may add separate GameComplete buttons later if desired
};

