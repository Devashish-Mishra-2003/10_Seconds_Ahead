#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include <memory>
#include "Config.h"

enum class Direction { Up, Down, Left, Right };

class Player {
public:
    Player();
    void loadTextures();
    void resetPosition();
    void enqueueMove(Direction dir);
    void executeNextMove();
    void updateSpriteTexture(Direction dir);
    void undoLastMove();


    sf::Sprite& getSprite() { return *mSprite; }

public: // exposed for preview
    std::queue<Direction> moves;
    sf::Vector2i gridPos = {0, GridSize - 1}; // start bottom-left
    sf::Vector2i peekNextMove() const;


private:
    sf::Texture texUp, texDown, texLeft, texRight;
    std::unique_ptr<sf::Sprite> mSprite;
};

