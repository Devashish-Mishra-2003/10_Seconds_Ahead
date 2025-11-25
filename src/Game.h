#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Grid.h"
#include "Player.h"

// Game states
enum class GameState { Planning, Executing };

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
    void handleInput(const sf::Event& e);
    void update(sf::Time dt);
    void render();
    void drawPlannedMoves();
    void updateLetterboxView(unsigned int newWidth, unsigned int newHeight);

    // Window / view
    sf::RenderWindow window;
    sf::View view;
    sf::Font font;

    // HUD texts
    std::unique_ptr<sf::Text> timerText;
    std::unique_ptr<sf::Text> blocksLeftText;
    std::unique_ptr<sf::Text> tooltipText;

    // core systems
    Grid grid;
    Player player;

    // game flow
    GameState state = GameState::Planning;
    float planningTime = 10.f;
    sf::Clock phaseClock;

    // block system + bookkeeping
    int blocksLeft = 3;
    std::vector<sf::Vector2i> placedBlocks;      // order of placements
    std::vector<ActionRecord> actionHistory;     // LIFO history for undo
};









