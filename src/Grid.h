#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Config.h"

struct Item {
    sf::Vector2i gridPos;
    bool collected = false;
};

class Grid {
public:
    void load();
    void loadLevel(const std::vector<std::string>& layout);
    void draw(sf::RenderWindow& win);

    bool checkItemAt(const sf::Vector2i& playerPos);
    bool isBlocked(const sf::Vector2i& pos) const;
    bool allItemsCollected() const;

    // 🧱 Block functions
    void placeBlock(const sf::Vector2i& pos);
    void removeBlock(const sf::Vector2i& pos);
    void clearBlocks();
    bool hasBlockAt(const sf::Vector2i& pos) const;
    int getBlockCount() const { return (int)blockPositions.size(); }

private:
    sf::Texture textureGrass;
    sf::Texture textureChest;
    sf::Texture textureTree;
    sf::Texture textureWater;
    sf::Texture textureBlock;

    std::vector<Item> items;
    std::vector<std::string> mapLayout;
    std::vector<sf::Vector2i> blockPositions;
};
