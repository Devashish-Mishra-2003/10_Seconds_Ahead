#include "Player.h"
#include <iostream>

Player::Player()
{
    loadTextures();
    mSprite = std::make_unique<sf::Sprite>(texUp); // SFML 3: no default ctor
    resetPosition();
}

void Player::loadTextures()
{
    if (!texUp.loadFromFile("assets/Player_Up.png"))    std::cerr << "Error: Player_Up.png\n";
    if (!texDown.loadFromFile("assets/Player_Down.png")) std::cerr << "Error: Player_Down.png\n";
    if (!texLeft.loadFromFile("assets/Player_Right.png")) std::cerr << "Error: Player_Left.png\n";
    if (!texRight.loadFromFile("assets/Player_Left.png")) std::cerr << "Error: Player_Right.png\n";
}

void Player::resetPosition()
{
    gridPos = {0, GridSize - 1};

    float originX = (WindowWidth  - GridSize * CellSize) / 2.f;
    float originY = (WindowHeight - GridSize * CellSize) / 2.f;

    mSprite->setPosition({
        originX + gridPos.x * CellSize,
        originY + gridPos.y * CellSize
    });

    auto s = mSprite->getTexture().getSize(); // SFML 3: reference, not pointer
    mSprite->setScale({
        (float)CellSize / s.x,
        (float)CellSize / s.y
    });
}

void Player::enqueueMove(Direction dir)
{
    moves.push(dir);
}

void Player::executeNextMove()
{
    if (moves.empty()) return;

    Direction d = moves.front(); moves.pop();

    if (d == Direction::Up    && gridPos.y > 0)               gridPos.y--;
    else if (d == Direction::Down  && gridPos.y < GridSize-1) gridPos.y++;
    else if (d == Direction::Left  && gridPos.x > 0)          gridPos.x--;
    else if (d == Direction::Right && gridPos.x < GridSize-1) gridPos.x++;

    updateSpriteTexture(d);

    float originX = (WindowWidth  - GridSize * CellSize) / 2.f;
    float originY = (WindowHeight - GridSize * CellSize) / 2.f;

    mSprite->setPosition({
        originX + gridPos.x * CellSize,
        originY + gridPos.y * CellSize
    });
}

void Player::updateSpriteTexture(Direction dir)
{
    switch (dir) {
        case Direction::Up:    mSprite->setTexture(texUp); break;
        case Direction::Down:  mSprite->setTexture(texDown); break;
        case Direction::Left:  mSprite->setTexture(texLeft); break;
        case Direction::Right: mSprite->setTexture(texRight); break;
    }

    auto s = mSprite->getTexture().getSize();
    mSprite->setScale({
        (float)CellSize / s.x,
        (float)CellSize / s.y
    });
}

sf::Vector2i Player::peekNextMove() const
{
    if (moves.empty()) return gridPos;
    Direction d = moves.front();
    sf::Vector2i p = gridPos;

    if (d == Direction::Up    && p.y > 0)            p.y--;
    else if (d == Direction::Down  && p.y < GridSize-1) p.y++;
    else if (d == Direction::Left  && p.x > 0)       p.x--;
    else if (d == Direction::Right && p.x < GridSize-1) p.x++;
    return p;
}

void Player::undoLastMove()
{
    if (moves.empty()) return;

    std::queue<Direction> temp;
    std::vector<Direction> allMoves;

    while (!moves.empty())
    {
        allMoves.push_back(moves.front());
        moves.pop();
    }

    allMoves.pop_back(); // remove last

    for (auto& m : allMoves)
        moves.push(m);
}





