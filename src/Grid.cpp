#include "Grid.h"
#include <iostream>
#include <algorithm>

void Grid::load()
{
    if (!textureGrass.loadFromFile("assets/Grass.png"))
        std::cerr << "Error loading Grass.png\n";
    if (!textureChest.loadFromFile("assets/Chest.png"))
        std::cerr << "Error loading Chest.png\n";
    if (!textureTree.loadFromFile("assets/Tree.png"))
        std::cerr << "Error loading Tree.png\n";
    if (!textureWater.loadFromFile("assets/Water.png"))
        std::cerr << "Error loading Water.png\n";
    if (!textureBlock.loadFromFile("assets/Block.png"))
        std::cerr << "Error loading Block.png\n";

    // hazard textures
    if (!textureCannonRight.loadFromFile("assets/Cannon_Right.png"))
        std::cerr << "Error loading Cannon_Right.png\n";
    if (!textureCannonLeft.loadFromFile("assets/Cannon_Left.png"))
        std::cerr << "Error loading Cannon_Left.png\n";
    if (!textureLaserUp.loadFromFile("assets/Laser_UP.png"))
        std::cerr << "Error loading Laser_UP.png\n";
    if (!textureLaserDown.loadFromFile("assets/Laser_Down.png"))
        std::cerr << "Error loading Laser_Down.png\n";
    if (!textureLaserBeam.loadFromFile("assets/Laser_Vertical.png"))
        std::cerr << "Error loading Laser_Vertical.png\n";
    if (!textureCannonBall.loadFromFile("assets/Cannon_Ball.png"))
        std::cerr << "Error loading Cannon_Ball.png\n";

    textureGrass.setSmooth(false);
    textureChest.setSmooth(true);
    textureTree.setSmooth(true);
    textureWater.setSmooth(true);
    textureBlock.setSmooth(true);
    textureLaserBeam.setSmooth(true);
    textureCannonBall.setSmooth(true);
}

void Grid::loadLevel(const std::vector<std::string>& layout)
{
    // Reset
    items.clear();
    blockPositions.clear();
    hazards.clear();
    activeBeamCells.clear();
    mapLayout.clear();
    projectiles.clear();

    // store the layout rows, handling multi-char hazard tokens
    for (int y = 0; y < (int)layout.size(); ++y) {
        std::string row = layout[y];
        if ((int)row.size() < GridSize) row += std::string(GridSize - row.size(), '.');
        if ((int)row.size() > GridSize) row = row.substr(0, GridSize);

        for (int x = 0; x < GridSize; ++x)
        {
            char c = row[x];

            // Cannons: 'C>' or 'C<'
            if (c == 'C' && x + 1 < (int)layout[y].size())
            {
                char dir = layout[y][x+1];
                if (dir == '>') {
                    hazards.push_back({{x, y}, HazardType::CannonRight, 0});
                    row[x] = '.'; if (x+1 < GridSize) row[x+1] = '.';
                    ++x;
                    continue;
                } else if (dir == '<') {
                    hazards.push_back({{x, y}, HazardType::CannonLeft, 0});
                    row[x] = '.'; if (x+1 < GridSize) row[x+1] = '.';
                    ++x;
                    continue;
                }
            }

            // Lasers: 'Lv' (down) or 'L^' (up)
            if (c == 'L' && x + 1 < (int)layout[y].size())
            {
                char dir = layout[y][x+1];
                if (dir == 'v') {
                    hazards.push_back({{x, y}, HazardType::LaserDown, 0});
                    row[x] = '.'; if (x+1 < GridSize) row[x+1] = '.';
                    ++x;
                    continue;
                } else if (dir == '^') {
                    hazards.push_back({{x, y}, HazardType::LaserUp, 0});
                    row[x] = '.'; if (x+1 < GridSize) row[x+1] = '.';
                    ++x;
                    continue;
                }
            }

            row[x] = c;
        }

        mapLayout.push_back(row);
    }

    // collect items (I) into items vector
    for (int y = 0; y < (int)mapLayout.size(); ++y)
        for (int x = 0; x < (int)mapLayout[y].size(); ++x)
            if (mapLayout[y][x] == 'I')
                items.push_back({{x, y}, false});

    // compute initial beams (lasers)
    for (auto &h : hazards) h.beamProgress = 0;
    computeBeams();

    // projectiles empty at start
    projectiles.clear();
}

void Grid::draw(sf::RenderWindow& win)
{
    sf::Sprite tile(textureGrass);
    tile.setScale({
        (float)CellSize / textureGrass.getSize().x,
        (float)CellSize / textureGrass.getSize().y
    });

    float originX = (WindowWidth  - GridSize * CellSize) / 2.f;
    float originY = (WindowHeight - GridSize * CellSize) / 2.f;

    for (int y = 0; y < GridSize; ++y)
    {
        for (int x = 0; x < GridSize; ++x)
        {
            char c = mapLayout[y][x];
            tile.setPosition({originX + x * CellSize, originY + y * CellSize});
            win.draw(tile);

            const sf::Texture* tex = nullptr;
            if (c == 'T') tex = &textureTree;
            else if (c == '~') tex = &textureWater;
            else if (c == 'I') tex = &textureChest;

            if (tex)
            {
                bool skip = false;
                if (c == 'I')
                    for (auto& i : items)
                        if (i.gridPos == sf::Vector2i(x, y) && i.collected)
                            skip = true;

                if (!skip)
                {
                    sf::Sprite obj(*tex);
                    obj.setScale({
                        (float)CellSize / tex->getSize().x,
                        (float)CellSize / tex->getSize().y
                    });
                    obj.setPosition({originX + x * CellSize, originY + y * CellSize});
                    win.draw(obj);
                }
            }
        }
    }

    // Draw active laser beams (beam cells)
    sf::Sprite beamSprite(textureLaserBeam);
    beamSprite.setScale({
        (float)CellSize / textureLaserBeam.getSize().x,
        (float)CellSize / textureLaserBeam.getSize().y
    });

    for (auto& bc : activeBeamCells)
    {
        beamSprite.setRotation(sf::degrees(0.f));
        beamSprite.setPosition({originX + bc.x * CellSize, originY + bc.y * CellSize});
        win.draw(beamSprite);
    }

    // Draw hazards (laser/cannon bases) on top of beams
    for (auto& h : hazards)
    {
        const sf::Texture* tex = nullptr;
        float rotation = 0.f;

        switch (h.type) {
            case HazardType::CannonRight:
                tex = &textureCannonRight;
                rotation = 0.f;
                break;
            case HazardType::CannonLeft:
                tex = &textureCannonLeft;
                rotation = 0.f;
                break;
            case HazardType::LaserDown:
                tex = &textureLaserDown;
                rotation = 0.f;
                break;
            case HazardType::LaserUp:
                tex = &textureLaserUp;
                rotation = 0.f;
                break;
        }

        if (tex) {
            sf::Sprite obj(*tex);
            obj.setScale({
                (float)CellSize / tex->getSize().x,
                (float)CellSize / tex->getSize().y
            });
            obj.setPosition({originX + h.pos.x * CellSize, originY + h.pos.y * CellSize});
            obj.setRotation(sf::degrees(rotation));
            win.draw(obj);
        }
    }

    // Draw projectiles (cannon balls)
    sf::Sprite ballSprite(textureCannonBall);
    ballSprite.setScale({
        (float)CellSize / textureCannonBall.getSize().x,
        (float)CellSize / textureCannonBall.getSize().y
    });

    for (auto& p : projectiles)
    {
        if (!p.alive) continue;
        ballSprite.setPosition({originX + p.pos.x * CellSize, originY + p.pos.y * CellSize});
        win.draw(ballSprite);
    }

    // Draw placed blocks
    for (auto& b : blockPositions)
    {
        sf::Sprite obj(textureBlock);
        obj.setScale({
            (float)CellSize / textureBlock.getSize().x,
            (float)CellSize / textureBlock.getSize().y
        });
        obj.setPosition({originX + b.x * CellSize, originY + b.y * CellSize});
        win.draw(obj);
    }
}

void Grid::placeBlock(const sf::Vector2i& pos)
{
    if (isBlocked(pos) || hasBlockAt(pos)) return;
    blockPositions.push_back(pos);
    computeBeams();
}

void Grid::removeBlock(const sf::Vector2i& pos)
{
    for (auto it = blockPositions.begin(); it != blockPositions.end(); ++it)
    {
        if (*it == pos)
        {
            blockPositions.erase(it);
            computeBeams();
            return;
        }
    }
}

void Grid::clearBlocks()
{
    blockPositions.clear();
    computeBeams();
}

bool Grid::hasBlockAt(const sf::Vector2i& pos) const
{
    for (auto& b : blockPositions)
        if (b == pos) return true;
    return false;
}

bool Grid::checkItemAt(const sf::Vector2i& playerPos)
{
    for (auto& item : items)
    {
        if (!item.collected && item.gridPos == playerPos)
        {
            item.collected = true;
            computeBeams();
            return true;
        }
    }
    return false;
}

bool Grid::isBlocked(const sf::Vector2i& pos) const
{
    if (pos.y < 0 || pos.y >= (int)mapLayout.size() ||
        pos.x < 0 || pos.x >= (int)mapLayout[0].size())
        return true;

    // origin hazards are not walkable
    for (auto &h : hazards) {
        if (h.pos == pos) return true;
    }

    char c = mapLayout[pos.y][pos.x];

    // Items are walkable for the player
    return (c == 'T' || c == '~' || hasBlockAt(pos));
}

bool Grid::allItemsCollected() const
{
    for (auto& i : items)
        if (!i.collected) return false;
    return true;
}

// ---------------- Hazards / Beams ----------------

void Grid::computeBeams()
{
    activeBeamCells.clear();

    for (auto& h : hazards)
    {
        // Only lasers produce continuous beams. Cannons use projectiles.
        if (h.type == HazardType::CannonLeft || h.type == HazardType::CannonRight)
            continue;

        sf::Vector2i dir{0,0};
        if (h.type == HazardType::LaserDown) dir = {0, 1};
        else if (h.type == HazardType::LaserUp) dir = {0, -1};

        sf::Vector2i cur = h.pos;
        // add up to beamProgress cells
        for (int step = 0; step < h.beamProgress; ++step) {
            cur += dir;

            if (cur.x < 0 || cur.x >= GridSize || cur.y < 0 || cur.y >= GridSize) break;

            // double-check obstacle
            char c = mapLayout[cur.y][cur.x];
            bool stop = false;
            if (c == 'T' || c == '~') stop = true;
            else if (c == 'I') {
                for (auto &it : items) if (it.gridPos == cur && !it.collected) { stop = true; break; }
            } else if (hasBlockAt(cur)) stop = true;

            if (stop) break;

            activeBeamCells.push_back(cur);
        }
    }
}

bool Grid::cellHasBeam(const sf::Vector2i& pos) const
{
    for (auto& b : activeBeamCells)
        if (b == pos) return true;
    return false;
}

void Grid::stepBeams()
{
    for (auto &h : hazards) {
        if (h.type != HazardType::LaserDown && h.type != HazardType::LaserUp) continue;

        sf::Vector2i dir = (h.type == HazardType::LaserDown) ? sf::Vector2i{0,1} : sf::Vector2i{0,-1};

        sf::Vector2i cur = h.pos + dir;
        int maxLen = 0;
        while (cur.x >= 0 && cur.x < GridSize && cur.y >= 0 && cur.y < GridSize) {
            bool stop = false;
            char c = mapLayout[cur.y][cur.x];
            if (c == 'T' || c == '~') stop = true;
            else if (c == 'I') {
                for (auto &it : items) {
                    if (it.gridPos == cur && !it.collected) { stop = true; break; }
                }
            } else if (hasBlockAt(cur)) stop = true;

            if (stop) break;
            ++maxLen;
            cur += dir;
        }

        if (h.beamProgress < maxLen) ++h.beamProgress;
        if (h.beamProgress > maxLen) h.beamProgress = maxLen;
    }

    computeBeams();
}

// ---------------- Projectile logic for cannons ----------------

void Grid::stepProjectiles()
{
    // 1) Move existing projectiles first (so newly spawned ones don't move immediately)
    for (auto& p : projectiles)
    {
        if (!p.alive) continue;

        sf::Vector2i next = p.pos + p.dir;

        // bounds check
        if (next.x < 0 || next.x >= GridSize || next.y < 0 || next.y >= GridSize) {
            p.alive = false;
            continue;
        }

        // check obstacle at next pos: Tree, Water, uncollected chest, block -> projectile disappears
        char c = mapLayout[next.y][next.x];
        bool stops = false;
        if (c == 'T' || c == '~') stops = true;
        else if (c == 'I') {
            for (auto& it : items) if (it.gridPos == next && !it.collected) { stops = true; break; }
        } else if (hasBlockAt(next)) stops = true;

        if (stops) {
            p.alive = false;
            continue;
        }

        // move forward
        p.pos = next;
    }

    // 2) Spawn new cannonballs after moving existing ones
    for (auto& h : hazards)
    {
        if (h.type == HazardType::CannonLeft || h.type == HazardType::CannonRight)
        {
            sf::Vector2i dir = (h.type == HazardType::CannonRight) ? sf::Vector2i{1,0} : sf::Vector2i{-1,0};
            sf::Vector2i spawnPos = h.pos + dir;

            // spawn only if inside map and not immediately blocked
            if (spawnPos.x >= 0 && spawnPos.x < GridSize && spawnPos.y >= 0 && spawnPos.y < GridSize)
            {
                char c = mapLayout[spawnPos.y][spawnPos.x];
                bool blocked = false;
                if (c == 'T' || c == '~') blocked = true;
                else if (c == 'I') {
                    for (auto& it : items) if (it.gridPos == spawnPos && !it.collected) { blocked = true; break; }
                } else if (hasBlockAt(spawnPos)) blocked = true;

                if (!blocked) {
                    Projectile p;
                    p.pos = spawnPos;
                    p.dir = dir;
                    p.alive = true;
                    projectiles.push_back(p);
                }
            }
        }
    }

    // 3) Garbage-collect dead projectiles
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& pr){ return !pr.alive; }),
        projectiles.end()
    );
}

bool Grid::cellHasProjectile(const sf::Vector2i& pos) const
{
    for (auto& p : projectiles)
        if (p.alive && p.pos == pos) return true;
    return false;
}

void Grid::clearProjectiles()
{
    projectiles.clear();
}

void Grid::resetItemsToOriginal()
{
    for (auto& it : items) it.collected = false;
    for (auto &h : hazards) h.beamProgress = 0;
    computeBeams();
}
