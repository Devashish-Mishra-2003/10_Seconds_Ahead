#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Config.h"

struct Item {
    sf::Vector2i gridPos;
    bool collected = false;
};

enum class HazardType {
    CannonRight,
    CannonLeft,
    LaserUp,
    LaserDown
};

struct Hazard {
    sf::Vector2i pos;
    HazardType type;
    int beamProgress = 0; // for lasers: how many cells currently visible
};

struct Projectile {
    sf::Vector2i pos;   // grid cell
    sf::Vector2i dir;   // direction vector (e.g. {1,0})
    bool alive = true;
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

    // Hazards / Beams / Projectiles
    void computeBeams();               // build activeBeamCells from hazard beamProgress
    void stepBeams();                  // advance beamProgress for lasers (animate appearance)
    bool cellHasBeam(const sf::Vector2i& pos) const;

    // Cannon projectile system
    void stepProjectiles();            // move existing projectiles and spawn new ones
    bool cellHasProjectile(const sf::Vector2i& pos) const;
    void clearProjectiles();
    void resetItemsToOriginal();        // reset collected state back to false

private:
    sf::Texture textureGrass;
    sf::Texture textureChest;
    sf::Texture textureTree;
    sf::Texture textureWater;
    sf::Texture textureBlock;

    // hazard textures
    sf::Texture textureCannonRight;
    sf::Texture textureCannonLeft;
    sf::Texture textureLaserUp;
    sf::Texture textureLaserDown;
    sf::Texture textureLaserBeam; // continuous beam tile (used per-cell)
    sf::Texture textureCannonBall; // for projectile (cannonball)

    std::vector<Item> items;
    std::vector<std::string> mapLayout; // (GridSize x GridSize), hazard origins replaced with '.'
    std::vector<sf::Vector2i> blockPositions;

    // hazards and active beam cells (cells that are dangerous)
    std::vector<Hazard> hazards;
    std::vector<sf::Vector2i> activeBeamCells;

    // projectiles for cannons
    std::vector<Projectile> projectiles;
};
