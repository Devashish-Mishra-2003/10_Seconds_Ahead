#include "Grid.h"
#include <iostream>

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

    textureGrass.setSmooth(false);
    textureChest.setSmooth(true);
    textureTree.setSmooth(true);
    textureWater.setSmooth(true);
    textureBlock.setSmooth(true);
}

void Grid::loadLevel(const std::vector<std::string>& layout)
{
    mapLayout = layout;
    items.clear();
    blockPositions.clear();

    for (int y = 0; y < (int)layout.size(); ++y)
        for (int x = 0; x < (int)layout[y].size(); ++x)
            if (layout[y][x] == 'I')
                items.push_back({{x, y}, false});
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

    // 🧱 Draw placed blocks
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
}

void Grid::removeBlock(const sf::Vector2i& pos)
{
    for (auto it = blockPositions.begin(); it != blockPositions.end(); ++it)
    {
        if (*it == pos)
        {
            blockPositions.erase(it);
            return;
        }
    }
}

void Grid::clearBlocks()
{
    blockPositions.clear();
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

    char c = mapLayout[pos.y][pos.x];
    return (c == 'T' || c == '~' || hasBlockAt(pos));
}

bool Grid::allItemsCollected() const
{
    for (auto& i : items)
        if (!i.collected) return false;
    return true;
}

