#include "Game.h"
#include <iostream>
#include <vector>
#include <string>
#include "Config.h"

// --- Level layout (20x20) ---
static const std::vector<std::string> Level_CrossfirePath = {
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

Game::Game()
: window(sf::VideoMode({WindowWidth, WindowHeight}), "10 Seconds Ahead")
{
    // letterbox view
    view.setCenter({WindowWidth / 2.f, WindowHeight / 2.f});
    view.setSize({(float)WindowWidth, (float)WindowHeight});
    window.setView(view);

    grid.load();
    grid.loadLevel(Level_CrossfirePath);

    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Missing font: assets/arial.ttf\n";
    }

    timerText = std::make_unique<sf::Text>(font, "10.0", 24);
    timerText->setFillColor(sf::Color::White);
    timerText->setPosition({10.f, 10.f});

    blocksLeftText = std::make_unique<sf::Text>(font, "Blocks Left: 3", 20);
    blocksLeftText->setFillColor(sf::Color::White);
    blocksLeftText->setPosition({10.f, 40.f});
    blocksLeft = 3;

    tooltipText = std::make_unique<sf::Text>(font, "WASD Move | B Block | K Undo", 18);
    tooltipText->setFillColor(sf::Color(230,230,230,200));
    tooltipText->setPosition({10.f, (float)WindowHeight - 32.f});

    phaseClock.restart();
    state = GameState::Planning;
}

void Game::run()
{
    while (window.isOpen())
    {
        while (auto ev = window.pollEvent())
        {
            const sf::Event& e = *ev;
            if (e.is<sf::Event::Closed>()) window.close();
            else if (auto* rs = e.getIf<sf::Event::Resized>()) updateLetterboxView(rs->size.x, rs->size.y);
            handleInput(e);
        }

        update(sf::Time());
        render();
    }
}

void Game::handleInput(const sf::Event& e)
{
    if (state != GameState::Planning) return;

    if (auto* k = e.getIf<sf::Event::KeyPressed>()) {
        using Key = sf::Keyboard::Key;

        // Movement planning: enqueue & record action
        if (k->code == Key::W) {
            player.enqueueMove(Direction::Up);
            actionHistory.push_back({false, Direction::Up, {}});
        }
        if (k->code == Key::S) {
            player.enqueueMove(Direction::Down);
            actionHistory.push_back({false, Direction::Down, {}});
        }
        if (k->code == Key::A) {
            player.enqueueMove(Direction::Left);
            actionHistory.push_back({false, Direction::Left, {}});
        }
        if (k->code == Key::D) {
            player.enqueueMove(Direction::Right);
            actionHistory.push_back({false, Direction::Right, {}});
        }

        // Undo last action (move or block)
        if (k->code == Key::K) {
            if (actionHistory.empty()) return;

            ActionRecord last = actionHistory.back();
            actionHistory.pop_back();

            if (last.isBlock) {
                // remove that block and refund a block
                grid.removeBlock(last.blockPos);
                // remove from placedBlocks vector if present (last occurrence)
                if (!placedBlocks.empty() && placedBlocks.back() == last.blockPos) {
                    placedBlocks.pop_back();
                } else {
                    // if not last, search and erase first matching occurrence
                    for (auto it = placedBlocks.begin(); it != placedBlocks.end(); ++it)
                        if (*it == last.blockPos) { placedBlocks.erase(it); break; }
                }
                blocksLeft++;
            } else {
                // undo a move
                player.undoLastMove();
            }
        }

        // Place a block at ghost position (last reachable ghost tile)
        if (k->code == Key::B && blocksLeft > 0) {
            // simulate ghost position from current planned moves
            sf::Vector2i ghostPos = player.gridPos;
            auto temp = player.moves;
            while (!temp.empty()) {
                Direction d = temp.front(); temp.pop();
                if (d == Direction::Up && ghostPos.y > 0) ghostPos.y--;
                else if (d == Direction::Down && ghostPos.y < GridSize - 1) ghostPos.y++;
                else if (d == Direction::Left && ghostPos.x > 0) ghostPos.x--;
                else if (d == Direction::Right && ghostPos.x < GridSize - 1) ghostPos.x++;

                if (grid.isBlocked(ghostPos)) break; // stop before blocked cell
            }

            // If target cell is valid and not blocked, place block
            if (!grid.isBlocked(ghostPos)) {
                grid.placeBlock(ghostPos);
                placedBlocks.push_back(ghostPos);

                // record block action
                actionHistory.push_back({true, Direction::Up, ghostPos});

                // consume a planned move leading into that cell (if any)
                // remove the last Move action record too, if it exists
                if (!actionHistory.empty()) {
                    // the block record is at back; check previous record
                    if (actionHistory.size() >= 2) {
                        ActionRecord prev = actionHistory[actionHistory.size()-2];
                        if (!prev.isBlock) {
                            // remove previous move record (it corresponds to the removed step)
                            // but careful: we already pushed block record; pop previous move record
                            // to do that, erase the element at size-2
                            actionHistory.erase(actionHistory.end()-2);
                            // also undo last move in player's queue
                            player.undoLastMove();
                        }
                    } else {
                        // no prior action to remove; nothing to undo in moves
                    }
                }

                blocksLeft--;
            }
        }
    }
}

void Game::update(sf::Time)
{
    if (state == GameState::Planning)
    {
        float remaining = planningTime - phaseClock.getElapsedTime().asSeconds();
        if (remaining < 0.f) remaining = 0.f;

        std::string t = std::to_string(remaining);
        if (t.size() > 4) t = t.substr(0,4);
        timerText->setString(t);

        blocksLeftText->setString("Blocks Left: " + std::to_string(blocksLeft));

        if (remaining <= 0.f) {
            state = GameState::Executing;
            phaseClock.restart();
        }
    }
    else // Executing
    {
        static sf::Clock moveClock;
        if (moveClock.getElapsedTime().asMilliseconds() > 250) {
            moveClock.restart();

            if (!player.moves.empty()) {
                sf::Vector2i nextPos = player.peekNextMove();

                if (!grid.isBlocked(nextPos)) {
                    player.executeNextMove();
                    grid.checkItemAt(player.gridPos);
                } else {
                    // discard blocked move
                    player.moves.pop();
                }
            } else {
                // End of execution:
                // blocks live only for this turn -> clear them
                grid.clearBlocks();
                placedBlocks.clear();
                actionHistory.clear(); // planning history reset
                blocksLeft = 3;
                phaseClock.restart();
                state = GameState::Planning;
            }
        }
    }
}

void Game::render()
{
    window.clear(sf::Color(167,216,255));
    grid.draw(window);

    if (state == GameState::Planning) drawPlannedMoves();

    window.draw(player.getSprite());
    window.draw(*timerText);
    window.draw(*blocksLeftText);
    window.draw(*tooltipText);

    window.display();
}

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
            ghost.setFillColor(sf::Color(255,0,0,120));
            ghost.setPosition({originX + p.x * CellSize, originY + p.y * CellSize});
            window.draw(ghost);
            break;
        }

        ghost.setPosition({originX + p.x * CellSize, originY + p.y * CellSize});
        window.draw(ghost);
    }
}

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
