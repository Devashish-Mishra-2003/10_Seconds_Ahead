#include "Game.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "Config.h"

// --- Level literals (20x20) ---
static const std::vector<std::string> L1 = {
"....................",
"....................",
"....................",
".........Lv.........",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
".........I..........",
"....................",
"....P...............",
"....................",
"....................",
"....................",
"....................",
"....................",
"..........I.........",
"...................."
};

static const std::vector<std::string> L2 = {
"....................",
"....................",
"....C>..I..C<.......",
"....................",
"....................",
"....................",
"....................",
".....I..............",
"....................",
"....................",
"....................",
"....................",
"....P...............",
"....................",
".........Lv.........",
"....................",
"....................",
"....................",
"..........I.........",
"...................."
};

static const std::vector<std::string> L3 = {
"..............T.....",
"....................",
"....C>......I.......",
"....................",
"..I....I....C<......",
"....................",
"....................",
".........Lv.........",
"....................",
"....TTT.............",
"....T~T.............",
"P...T~T......I......",
"....T~T.............",
"....TTT.............",
"....................",
"...........Lv.......",
"....................",
"......C>............",
".................TT.",
"....T.......I......."
};

static const std::vector<std::string> L4 = {
"....................",
"...C>.......C<......",
"...TTT......TTT.....",
"...T~T......T~T..I..",
"...T~T......T~T.....",
"...TTT......TTT.....",
"....................",
"......I.............",
"....C>......C<......",
"....................",
"....................",
"..Lv................",
"......I.......I.....",
"....................",
"....................",
"........I...........",
"....................",
"....................",
"P...................",
"...................."
};

static const std::vector<std::string> L5 = {
"..C>..........C<....",
"....................",
"......I.......I....",
"....................",
"....TTT.............",
"....T~T.............",
"....T~T.....I.......",
"....TTT.............",
"....................",
"....................",
"..Lv.......Lv.......",
"....................",
"....I.........I.....",
"....................",
"....................",
"....................",
".........I..........",
"....................",
"P...................",
"...................."
};

static const std::vector<std::string> L6 = {
".........C>.........",
"....................",
"...I.........I......",
"....................",
"..Lv.....I....Lv....",
"....................",
"....................",
".....TTT............",
".....T~T.....I......",
".....T~T............",
".....TTT............",
"....................",
"...............C<...",
"....................",
"....................",
"....................",
"....I...............",
"....................",
"P...................",
"...................."
};

// ---------------- Helpers ----------------

static std::string formatFloatTrim(float v, int precision = 1) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.*f", precision, v);
    return std::string(buf);
}

static bool pointInRect(const sf::Vector2f& p, const sf::FloatRect& r) {
    // SFML3: FloatRect has .position and .size
    return (p.x >= r.position.x && p.x <= r.position.x + r.size.x
         && p.y >= r.position.y && p.y <= r.position.y + r.size.y);
}

// --- formatting helper (implement the member declared in Game.h) ---
std::string Game::formatFloatTrim(float v, int precision)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%.*f", precision, v);
    return std::string(buf);
}

// ---------------- Game implementation ----------------

Game::Game()
: window(sf::VideoMode({WindowWidth, WindowHeight}), "10 Seconds Ahead"),
  view()
{
    // letterbox view init
    view.setCenter({WindowWidth / 2.f, WindowHeight / 2.f});
    view.setSize({(float)WindowWidth, (float)WindowHeight});
    window.setView(view);

    // load grid textures
    grid.load();

    // load font and HUD text early
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Missing font: assets/arial.ttf\n";
    }

    // Now create buttons (font is available for sf::Text inside ElevatedButton)
    mainPlayBtn     = std::make_unique<ElevatedButton>(font, "Play");
    mainSettingsBtn = std::make_unique<ElevatedButton>(font, "Settings");
    mainQuitBtn     = std::make_unique<ElevatedButton>(font, "Quit");

    settingsEasyBtn   = std::make_unique<ElevatedButton>(font, "Easy  (Blocks=3)");
    settingsNormalBtn = std::make_unique<ElevatedButton>(font, "Normal(Blocks=2)");
    settingsHardBtn   = std::make_unique<ElevatedButton>(font, "Hard  (Blocks=1)");

    pauseResumeBtn   = std::make_unique<ElevatedButton>(font, "Resume", sf::Vector2f{240.f,44.f});
    pauseRestartBtn  = std::make_unique<ElevatedButton>(font, "Restart Level", sf::Vector2f{240.f,44.f});
    pauseSettingsBtn = std::make_unique<ElevatedButton>(font, "Settings", sf::Vector2f{240.f,44.f});
    pauseMenuBtn     = std::make_unique<ElevatedButton>(font, "Main Menu", sf::Vector2f{240.f,44.f});

    failRetryBtn = std::make_unique<ElevatedButton>(font, "Retry Level", sf::Vector2f{240.f,44.f});
    failMenuBtn  = std::make_unique<ElevatedButton>(font, "Main Menu", sf::Vector2f{240.f,44.f});

    completeNextBtn  = std::make_unique<ElevatedButton>(font, "Next Level", sf::Vector2f{240.f,44.f});
    completeRetryBtn = std::make_unique<ElevatedButton>(font, "Retry", sf::Vector2f{240.f,44.f});
    completeMenuBtn  = std::make_unique<ElevatedButton>(font, "Main Menu", sf::Vector2f{240.f,44.f});

    // SFML3 Text constructor: Text(const Font& font, String string="", unsigned int characterSize=30)
    timerText = std::make_unique<sf::Text>(font, "10.0", 24u);
    timerText->setFillColor(sf::Color::White);
    timerText->setPosition({10.f, 10.f});

    blocksLeftText = std::make_unique<sf::Text>(font, "Blocks Left: 3", 20u);
    blocksLeftText->setFillColor(sf::Color::White);
    blocksLeftText->setPosition({10.f, 40.f});

    turnsText = std::make_unique<sf::Text>(font, "Turns: ∞", 20u);
    turnsText->setFillColor(sf::Color::White);
    turnsText->setPosition({(float)WindowWidth - 220.f, 10.f});

    tooltipText = std::make_unique<sf::Text>(font, "WASD Move | B Block | K Undo | ESC Pause", 18u);
    tooltipText->setFillColor(sf::Color(230,230,230,200));
    tooltipText->setPosition({10.f, (float)WindowHeight - 32.f});

    levelTitleText = std::make_unique<sf::Text>(font, "Level 1", 20u);
    levelTitleText->setFillColor(sf::Color::White);
    levelTitleText->setPosition({(float)WindowWidth - 200.f, 40.f});

    toastText = std::make_unique<sf::Text>(font, "", 22u);
    toastText->setFillColor(sf::Color(255, 200, 80));
    toastText->setPosition({(float)WindowWidth/2.f - 140.f, (float)WindowHeight - 80.f});

    // prepare built-in levels
    levels.push_back(L1);
    levels.push_back(L2);
    levels.push_back(L3);
    levels.push_back(L4);
    levels.push_back(L5);
    levels.push_back(L6);

    // start at main menu
    uiState = UIState::MainMenu;

    // defaults
    settings.difficulty = Difficulty::Normal; // default to Normal for challenge
    applyDifficulty();

    phase = GamePhase::Planning;
    phaseClock.restart();

    // wire callbacks (capture this)
    mainPlayBtn->setCallback([this](){
        startLevel(0);
        uiState = UIState::Playing;
    });
    mainSettingsBtn->setCallback([this](){
        uiState = UIState::Settings;
    });
    mainQuitBtn->setCallback([this](){
        window.close();
    });

    settingsEasyBtn->setCallback([this](){
        settings.difficulty = Difficulty::Easy;
        applyDifficulty();
        uiState = UIState::MainMenu;
    });
    settingsNormalBtn->setCallback([this](){
        settings.difficulty = Difficulty::Normal;
        applyDifficulty();
        uiState = UIState::MainMenu;
    });
    settingsHardBtn->setCallback([this](){
        settings.difficulty = Difficulty::Hard;
        applyDifficulty();
        uiState = UIState::MainMenu;
    });

    pauseResumeBtn->setCallback([this](){ uiState = UIState::Playing; });
    pauseRestartBtn->setCallback([this](){ startLevel(currentLevel); uiState = UIState::Playing; });
    pauseSettingsBtn->setCallback([this](){ uiState = UIState::Settings; });
    pauseMenuBtn->setCallback([this](){ uiState = UIState::MainMenu; });

    failRetryBtn->setCallback([this](){ startLevel(currentLevel); uiState = UIState::Playing; });
    failMenuBtn->setCallback([this](){ uiState = UIState::MainMenu; });

    completeNextBtn->setCallback([this](){
        // advance to next level (wrap if necessary)
        int next = currentLevel + 1;
        if (next >= (int)levels.size()) next = 0;
        startLevel(next);
        uiState = UIState::Playing;
    });
    completeRetryBtn->setCallback([this](){ startLevel(currentLevel); uiState = UIState::Playing; });
    completeMenuBtn->setCallback([this](){ uiState = UIState::MainMenu; });

    // Set button positions (these will match old layout)
    float bw = 220.f, bh = 48.f;
    mainPlayBtn->setPosition({(WindowWidth/2.f) - bw/2.f, 220.f});
    mainSettingsBtn->setPosition({(WindowWidth/2.f) - bw/2.f, 290.f});
    mainQuitBtn->setPosition({(WindowWidth/2.f) - bw/2.f, 360.f});

    float sbw = 220.f, sbh = 48.f;
    float scx = (WindowWidth/2.f) - sbw/2.f;

    settingsEasyBtn->setPosition({scx, 220.f});
    settingsNormalBtn->setPosition({scx, 290.f});
    settingsHardBtn->setPosition({scx, 360.f});

    float pbw = 240.f;
    pauseResumeBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 200.f});
    pauseRestartBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 260.f});
    pauseSettingsBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 320.f});
    pauseMenuBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 380.f});

    failRetryBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 300.f});
    failMenuBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 360.f});

    completeNextBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 240.f});
    completeRetryBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 300.f});
    completeMenuBtn->setPosition({(WindowWidth/2.f) - pbw/2.f, 360.f});
}

void Game::run()
{
    while (window.isOpen())
    {
        while (auto ev = window.pollEvent())
        {
            const sf::Event& e = *ev;
            if (e.is<sf::Event::Closed>()) {
                window.close();
                return;
            }
            else if (auto* rs = e.getIf<sf::Event::Resized>()) {
                updateLetterboxView(rs->size.x, rs->size.y);
            }

            handleEvent(e);
        }

        update();
        render();
    }
}

// ---------------- Event handling (delegates to UI/playing) ----------------

void Game::handleEvent(const sf::Event& e)
{
    // universal key handling
    if (e.is<sf::Event::KeyPressed>()) {
        auto key = e.getIf<sf::Event::KeyPressed>()->code;
        if (key == sf::Keyboard::Key::Escape) {
            if (uiState == UIState::Playing) {
                uiState = UIState::Pause;
            } else if (uiState == UIState::Pause) {
                uiState = UIState::Playing;
            }
        }
    }

    // dispatch by UI state
    if (uiState == UIState::Playing) {
        handleInputPlaying(e);
    } else if (uiState == UIState::MainMenu) {
        // we now use per-frame mouse polling; no event-based handling here
    } else if (uiState == UIState::Settings) {
        if (e.is<sf::Event::KeyPressed>()) {
            if (e.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                uiState = UIState::MainMenu;
            }
        }
    } else if (uiState == UIState::Pause) {
        // pause handled via buttons (per-frame)
    } else if (uiState == UIState::LevelFail || uiState == UIState::LevelComplete || uiState == UIState::GameComplete) {
        // handled via buttons per-frame
    }
}

// ---------------- Update loop ----------------

void Game::update()
{
    // frame dt
    sf::Time dt = frameDeltaClock.restart();

    // toast lifetime
    if (toastText->getString() != "") {
        if (toastClock.getElapsedTime().asSeconds() > 0.9f) {
            toastText->setString("");
        }
    }

    // idle hazard ticks (both in menu and in-game so visuals animate)
    static sf::Clock hazardClock;
    if (hazardClock.getElapsedTime().asMilliseconds() > 220) {
        hazardClock.restart();
        grid.stepProjectiles();
        grid.stepBeams();
    }
    grid.computeBeams();

    // Per-frame mouse state used by buttons
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window); // PASS window
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
    bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    // Update buttons visible in current UI state: handle mouse, update animations
    if (uiState == UIState::MainMenu) {
        mainPlayBtn->handleMouse(mouseWorld, mouseDown);
        mainSettingsBtn->handleMouse(mouseWorld, mouseDown);
        mainQuitBtn->handleMouse(mouseWorld, mouseDown);

        mainPlayBtn->update(dt);
        mainSettingsBtn->update(dt);
        mainQuitBtn->update(dt);
    } else if (uiState == UIState::Settings) {
        settingsEasyBtn->handleMouse(mouseWorld, mouseDown);
        settingsNormalBtn->handleMouse(mouseWorld, mouseDown);
        settingsHardBtn->handleMouse(mouseWorld, mouseDown);

        settingsEasyBtn->update(dt);
        settingsNormalBtn->update(dt);
        settingsHardBtn->update(dt);
    } else if (uiState == UIState::Pause) {
        pauseResumeBtn->handleMouse(mouseWorld, mouseDown);
        pauseRestartBtn->handleMouse(mouseWorld, mouseDown);
        pauseSettingsBtn->handleMouse(mouseWorld, mouseDown);
        pauseMenuBtn->handleMouse(mouseWorld, mouseDown);

        pauseResumeBtn->update(dt);
        pauseRestartBtn->update(dt);
        pauseSettingsBtn->update(dt);
        pauseMenuBtn->update(dt);
    } else if (uiState == UIState::LevelFail) {
        failRetryBtn->handleMouse(mouseWorld, mouseDown);
        failMenuBtn->handleMouse(mouseWorld, mouseDown);

        failRetryBtn->update(dt);
        failMenuBtn->update(dt);
    } else if (uiState == UIState::LevelComplete) {
        // restored: handle Next + Retry + Menu
        completeNextBtn->handleMouse(mouseWorld, mouseDown);
        completeRetryBtn->handleMouse(mouseWorld, mouseDown);
        completeMenuBtn->handleMouse(mouseWorld, mouseDown);

        completeNextBtn->update(dt);
        completeRetryBtn->update(dt);
        completeMenuBtn->update(dt);
    } else if (uiState == UIState::GameComplete) {
        // Final-game complete: Replay (start level 0), Retry (same level), Main Menu
        // We'll reuse completeRetryBtn (acts as Retry) and completeMenuBtn.
        completeRetryBtn->handleMouse(mouseWorld, mouseDown); // Retry same level
        completeMenuBtn->handleMouse(mouseWorld, mouseDown);  // Back to menu
        // For "Replay" (start from 0), reuse completeNextBtn but callback should start level 0.
        completeNextBtn->handleMouse(mouseWorld, mouseDown);

        completeNextBtn->update(dt);
        completeRetryBtn->update(dt);
        completeMenuBtn->update(dt);
    }

    if (uiState == UIState::Playing) {
        updatePlaying();
    }
}

// ---------------- Playing update + turn logic ----------------

void Game::updatePlaying()
{
    // update HUD strings
    float remaining = planningTime - phaseClock.getElapsedTime().asSeconds();
    if (remaining < 0.f) remaining = 0.f;
    timerText->setString(formatFloatTrim(remaining, 1));
    blocksLeftText->setString("Blocks Left : " + std::to_string(blocksLeft));

    if (levelState.initialTurns < 0) {
        turnsText->setString("Turns : Infinite");
    } else {
        turnsText->setString("Turns : " + std::to_string(levelState.turnsRemaining));
    }

    // phase transitions
    if (phase == GamePhase::Planning) {
        if (remaining <= 0.f) {
            // start execution
            phase = GamePhase::Executing;
            phaseClock.restart();
        }
    } else { // Executing
        static sf::Clock moveClock;
        if (moveClock.getElapsedTime().asMilliseconds() > 250) {
            moveClock.restart();

            if (!player.moves.empty()) {
                sf::Vector2i nextPos = player.peekNextMove();

                if (!grid.isBlocked(nextPos)) {
                    player.executeNextMove();
                    bool picked = grid.checkItemAt(player.gridPos);

                    // After move, check hazards (beams/projectiles)
                    bool hitByBeam = grid.cellHasBeam(player.gridPos);
                    bool hitByProjectile = grid.cellHasProjectile(player.gridPos);

                    if (hitByBeam || hitByProjectile) {
                        // Player dies: apply penalties and reset level state appropriately
                        if (levelState.initialTurns >= 0) {
                            levelState.turnsRemaining -= 1;
                        }

                        if (settings.difficulty != Difficulty::Easy && levelState.initialTurns >= 0) {
                            levelState.turnsRemaining -= 1;
                        }

                        // Show toast
                        if (settings.difficulty == Difficulty::Easy) {
                            toastText->setString("You died !");
                        } else {
                            toastText->setString("You died ! Turns -2");
                        }
                        toastClock.restart();

                        // Reset level to initial
                        player.resetPosition();
                        while (!player.moves.empty()) player.moves.pop();
                        grid.clearBlocks();
                        grid.clearProjectiles();
                        grid.resetItemsToOriginal();

                        placedBlocks.clear();
                        actionHistory.clear();
                        blocksLeft = settings.blocksPerTurn();

                        // Check fail condition
                        if (levelState.initialTurns >= 0 && levelState.turnsRemaining <= 0) {
                            failLevel();
                            return;
                        }

                        phase = GamePhase::Planning;
                        phaseClock.restart();
                        return;
                    }

                    // If picked an item and that was the last -> level complete immediately
                    if (picked && grid.allItemsCollected()) {
                        completeLevel();
                        return;
                    }

                } else {
                    // move was blocked; consume this planned move without moving
                    player.moves.pop();
                }
            } else {
                // execution finished normally (no more planned moves)
                if (levelState.initialTurns >= 0) {
                    levelState.turnsRemaining -= 1;
                }

                // If all items collected by the end -> complete
                if (grid.allItemsCollected()) {
                    completeLevel();
                    return;
                }

                // If turns exhausted -> fail
                if (levelState.initialTurns >= 0 && levelState.turnsRemaining <= 0) {
                    failLevel();
                    return;
                }

                // else reset planning state for next turn
                grid.clearBlocks();
                grid.clearProjectiles();
                placedBlocks.clear();
                actionHistory.clear();
                blocksLeft = settings.blocksPerTurn();
                phase = GamePhase::Planning;
                phaseClock.restart();
                return;
            }
        }
    }
}

// ---------------- Input handling during playing (planning phase only) ----------------

void Game::handleInputPlaying(const sf::Event& e)
{
    // Only handle inputs in Planning phase
    if (phase != GamePhase::Planning) return;

    if (e.is<sf::Event::KeyPressed>()) {
        auto key = e.getIf<sf::Event::KeyPressed>()->code;
        using Key = sf::Keyboard::Key;

        if (key == Key::W) {
            player.enqueueMove(Direction::Up);
            actionHistory.push_back({false, Direction::Up, {}});
        }
        else if (key == Key::S) {
            player.enqueueMove(Direction::Down);
            actionHistory.push_back({false, Direction::Down, {}});
        }
        else if (key == Key::A) {
            player.enqueueMove(Direction::Left);
            actionHistory.push_back({false, Direction::Left, {}});
        }
        else if (key == Key::D) {
            player.enqueueMove(Direction::Right);
            actionHistory.push_back({false, Direction::Right, {}});
        }
        else if (key == Key::K) {
            if (actionHistory.empty()) return;
            ActionRecord last = actionHistory.back();
            actionHistory.pop_back();
            if (last.isBlock) {
                grid.removeBlock(last.blockPos);
                if (!placedBlocks.empty() && placedBlocks.back() == last.blockPos) placedBlocks.pop_back();
                else {
                    for (auto it = placedBlocks.begin(); it != placedBlocks.end(); ++it)
                        if (*it == last.blockPos) { placedBlocks.erase(it); break; }
                }
                blocksLeft++;
            } else {
                player.undoLastMove();
            }
        }
        else if (key == Key::B) {
            if (blocksLeft <= 0) return;
            // compute ghost pos
            sf::Vector2i ghostPos = player.gridPos;
            auto temp = player.moves;
            while (!temp.empty()) {
                Direction d = temp.front(); temp.pop();
                if (d == Direction::Up && ghostPos.y > 0) ghostPos.y--;
                else if (d == Direction::Down && ghostPos.y < GridSize - 1) ghostPos.y++;
                else if (d == Direction::Left && ghostPos.x > 0) ghostPos.x--;
                else if (d == Direction::Right && ghostPos.x < GridSize - 1) ghostPos.x++;

                if (grid.isBlocked(ghostPos)) break;
            }

            if (!grid.isBlocked(ghostPos) && !grid.hasBlockAt(ghostPos)) {
                grid.placeBlock(ghostPos);
                placedBlocks.push_back(ghostPos);
                actionHistory.push_back({true, Direction::Up, ghostPos});

                if (actionHistory.size() >= 2) {
                    ActionRecord prev = actionHistory[actionHistory.size()-2];
                    if (!prev.isBlock) {
                        actionHistory.erase(actionHistory.end()-2);
                        player.undoLastMove();
                    }
                }
                blocksLeft--;
            }
        }
    }
}

// ---------------- Rendering ----------------

void Game::render()
{
    window.clear(sf::Color(167,216,255));

    if (uiState == UIState::MainMenu) {
        drawMainMenu();
        // draw buttons
        mainPlayBtn->draw(window);
        mainSettingsBtn->draw(window);
        mainQuitBtn->draw(window);
    } else if (uiState == UIState::Settings) {
        drawSettingsMenu();
        settingsEasyBtn->draw(window);
        settingsNormalBtn->draw(window);
        settingsHardBtn->draw(window);
    } else if (uiState == UIState::Playing || uiState == UIState::Pause) {
        renderPlaying();
        if (uiState == UIState::Pause) {
            drawPauseMenu();
            pauseResumeBtn->draw(window);
            pauseRestartBtn->draw(window);
            pauseSettingsBtn->draw(window);
            pauseMenuBtn->draw(window);
        }
    } else if (uiState == UIState::LevelFail) {
        renderPlaying();
        drawLevelFail();
        failRetryBtn->draw(window);
        failMenuBtn->draw(window);
    } else if (uiState == UIState::LevelComplete) {
        renderPlaying();
        drawLevelComplete();
        // draw Next, Retry, Main Menu
        completeNextBtn->draw(window);
        completeRetryBtn->draw(window);
        completeMenuBtn->draw(window);
    } else if (uiState == UIState::GameComplete) {
        // draw final game-complete screen
        sf::Text title(font, "Game Completed !", 44u);
        title.setFillColor(sf::Color(120,220,120));
        title.setStyle(sf::Text::Style::Bold);
        {
            sf::FloatRect tb = title.getLocalBounds();
            title.setOrigin({ tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f });
            title.setPosition({ WindowWidth/2.f, 120.f });
        }
        window.draw(title);

        sf::Text stats(font, "You cleared all levels, Nice work !", 20u);
        stats.setFillColor(sf::Color::White);
        stats.setStyle(sf::Text::Style::Bold);
        {
            sf::FloatRect sb = stats.getLocalBounds();
            stats.setOrigin({ sb.position.x + sb.size.x/2.f, sb.position.y + sb.size.y/2.f });
            stats.setPosition({ WindowWidth/2.f, 180.f });
        }
        window.draw(stats);

        // Replay (start level 0), Retry (same level), Main Menu
        // completeNextBtn will be used as "Replay" here (its callback starts next by default;
        // we override behavior for GameComplete below)
        completeNextBtn->draw(window);   // acts as Replay
        completeRetryBtn->draw(window);  // acts as Retry (same level)
        completeMenuBtn->draw(window);
    }

    window.display();
}

void Game::renderPlaying()
{
    // draw grid & hazards
    grid.draw(window);

    // draw planned moves ghost if in planning
    if (phase == GamePhase::Planning) drawPlannedMoves();

    // draw player
    window.draw(player.getSprite());

    // HUD
    window.draw(*timerText);
    window.draw(*blocksLeftText);
    window.draw(*turnsText);
    window.draw(*levelTitleText);
    window.draw(*tooltipText);

    // toast
    if (toastText->getString() != "") {
        window.draw(*toastText);
    }
}

// ---------------- UI screens drawing ----------------

void Game::drawMainMenu()
{
    // Title (centered + bold)
    sf::Text title(font, "10 Seconds Ahead", 48u);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin({ tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f });
        title.setPosition({ WindowWidth/2.f, 80.f });
    }
    window.draw(title);

    // draw buttons (they already handle shadow and animation)
    mainPlayBtn->draw(window);
    mainSettingsBtn->draw(window);
    mainQuitBtn->draw(window);

    // Info text below buttons (centered + bold-ish)
    sf::Text info(font, ("Difficulty : " + settings.difficultyName()), 18u);
    info.setFillColor(sf::Color::White);
    info.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect ib = info.getLocalBounds();
        info.setOrigin({ ib.position.x + ib.size.x/2.f, ib.position.y + ib.size.y/2.f });
        info.setPosition({ WindowWidth/2.f, 460.f });
    }
    window.draw(info);
}


void Game::drawSettingsMenu()
{
    // Title (centered + bold)
    sf::Text title(font, "Settings", 40u);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin({ tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f });
        title.setPosition({ WindowWidth/2.f, 80.f });
    }
    window.draw(title);

    // Draw option buttons (they already have positions)
    settingsEasyBtn->draw(window);
    settingsNormalBtn->draw(window);
    settingsHardBtn->draw(window);

    // Back hint (centered)
    sf::Text backHint(font, "Press ESC to go back", 16u);
    backHint.setFillColor(sf::Color::White);
    backHint.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect bb = backHint.getLocalBounds();
        backHint.setOrigin({ bb.position.x + bb.size.x/2.f, bb.position.y + bb.size.y/2.f });
        backHint.setPosition({ WindowWidth/2.f, (float)WindowHeight - 80.f });
    }
    window.draw(backHint);
}


void Game::drawPauseMenu()
{
    sf::Text title(font, "Paused", 40u);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin({ tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f });
        title.setPosition({ WindowWidth/2.f, 120.f });
    }
    window.draw(title);

    // draw pause buttons (already positioned)
    pauseResumeBtn->draw(window);
    pauseRestartBtn->draw(window);
    pauseSettingsBtn->draw(window);
    pauseMenuBtn->draw(window);
}

void Game::drawLevelFail()
{
    sf::Text title(font, "Level Failed !", 44u);
    title.setFillColor(sf::Color(220,60,60));
    title.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin({ tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f });
        title.setPosition({ WindowWidth/2.f, 130.f });
    }
    window.draw(title);

    sf::Text msg(font, "You exhausted all turns", 20u);
    msg.setFillColor(sf::Color::White);
    msg.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect mb = msg.getLocalBounds();
        msg.setOrigin({ mb.position.x + mb.size.x/2.f, mb.position.y + mb.size.y/2.f });
        msg.setPosition({ WindowWidth/2.f, 190.f });
    }
    window.draw(msg);

    // draw button widgets
    failRetryBtn->draw(window);
    failMenuBtn->draw(window);
}


void Game::drawLevelComplete()
{
    sf::Text title(font, "Level Complete !", 44u);
    title.setFillColor(sf::Color(120,220,120));
    title.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin({ tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f });
        title.setPosition({ WindowWidth/2.f, 120.f });
    }
    window.draw(title);

    sf::Text stats(font, "Great job! Choose Next or Retry", 20u);
    stats.setFillColor(sf::Color::White);
    stats.setStyle(sf::Text::Style::Bold);
    {
        sf::FloatRect sb = stats.getLocalBounds();
        stats.setOrigin({ sb.position.x + sb.size.x/2.f, sb.position.y + sb.size.y/2.f });
        stats.setPosition({ WindowWidth/2.f, 180.f });
    }
    window.draw(stats);

    // Buttons are drawn in render() but positions were set in ctor.
}


// ---------------- Ghost preview ----------------

void Game::drawPlannedMoves()
{
    auto q = player.moves;            // copy queue
    sf::Vector2i p = player.gridPos;  // simulate position

    sf::RectangleShape ghost({(float)CellSize, (float)CellSize});
    ghost.setFillColor(sf::Color(255,255,0,120));
    ghost.setOutlineColor(sf::Color::Black);
    ghost.setOutlineThickness(1);

    float originX = (WindowWidth  - GridSize * CellSize) / 2.f;
    float originY = (WindowHeight - GridSize * CellSize) / 2.f;

    while (!q.empty()) {
        Direction d = q.front(); q.pop();

        if (d == Direction::Up && p.y > 0) p.y--;
        else if (d == Direction::Down && p.y < GridSize - 1) p.y++;
        else if (d == Direction::Left && p.x > 0) p.x--;
        else if (d == Direction::Right && p.x < GridSize - 1) p.x++;

        if (grid.isBlocked(p)) {
            ghost.setFillColor(sf::Color(255,0,0,150));
            ghost.setPosition({originX + p.x * CellSize, originY + p.y * CellSize});
            window.draw(ghost);
            break;
        }

        ghost.setPosition({originX + p.x * CellSize, originY + p.y * CellSize});
        window.draw(ghost);
    }
}

// ---------------- Level lifecycle helpers ----------------

void Game::applyDifficulty()
{
    blocksLeft = settings.blocksPerTurn();
    if (settings.turnLimit() < 0) {
        levelState.initialTurns = -1;
        levelState.turnsRemaining = -1;
    } else {
        levelState.initialTurns = settings.turnLimit();
        levelState.turnsRemaining = settings.turnLimit();
    }
}

void Game::startLevel(int index)
{
    if (index < 0) index = 0;
    if (index >= (int)levels.size()) index = 0;

    currentLevel = index;
    grid.loadLevel(levels[index]);
    player.resetPosition();

    // reset runtime pieces
    grid.clearBlocks();
    grid.clearProjectiles();
    grid.resetItemsToOriginal();
    placedBlocks.clear();
    actionHistory.clear();

    // set difficulty-based numbers
    blocksLeft = settings.blocksPerTurn();
    if (settings.turnLimit() < 0) {
        levelState.initialTurns = -1;
        levelState.turnsRemaining = -1;
    } else {
        levelState.initialTurns = settings.turnLimit();
        levelState.turnsRemaining = settings.turnLimit();
    }

    phase = GamePhase::Planning;
    phaseClock.restart();

    // update UI
    levelTitleText->setString("Level " + std::to_string(currentLevel + 1));
    toastText->setString("");
}

void Game::completeLevel()
{
    // if this was the last built-in level, show full-game completion screen
    if (currentLevel >= (int)levels.size() - 1) {
        uiState = UIState::GameComplete;
        toastText->setString("All levels cleared !");
        toastClock.restart();

        // ensure Replay button starts level 0
        completeNextBtn->setCallback([this](){
            startLevel(0);    // Replay from level 1 (index 0)
            uiState = UIState::Playing;
        });

        // Retry should play same (last) level again
        completeRetryBtn->setCallback([this](){
            startLevel(currentLevel);
            uiState = UIState::Playing;
        });

        // Menu already set
    } else {
        uiState = UIState::LevelComplete;
        toastText->setString("Level Complete !");
        toastClock.restart();

        // restore next behavior (next level)
        completeNextBtn->setCallback([this](){
            startLevel(currentLevel + 1);
            uiState = UIState::Playing;
        });

        completeRetryBtn->setCallback([this](){
            startLevel(currentLevel);
            uiState = UIState::Playing;
        });

        // Menu already set
    }
}

void Game::failLevel()
{
    uiState = UIState::LevelFail;
    toastText->setString("Level Failed");
    toastClock.restart();
}

// ---------------- Letterbox / view ----------------

void Game::updateLetterboxView(unsigned int newWidth, unsigned int newHeight)
{
    float windowRatio = (float)newWidth / (float)newHeight;
    float viewRatio   = (float)WindowWidth / (float)WindowHeight;

    float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;

    if (windowRatio > viewRatio) {
        sizeX = viewRatio / windowRatio;
        posX  = (1.f - sizeX) * 0.5f;
    } else {
        sizeY = windowRatio / viewRatio;
        posY  = (1.f - sizeY) * 0.5f;
    }

    view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
    window.setView(view);
}


