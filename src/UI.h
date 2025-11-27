#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

// ElevatedButton: simple reusable elevated card button with hover/press animation (mouse only)
class ElevatedButton {
public:
    ElevatedButton(const sf::Font& font,
                   const std::string& label,
                   const sf::Vector2f& size = {220.f, 48.f});

    void setPosition(const sf::Vector2f& pos);
    void setLabel(const std::string& s);
    void setCallback(std::function<void()> cb);

    // input handling: mouse pos and mouse pressed (left)
    void handleMouse(const sf::Vector2f& mouseWorldPos, bool mousePressed);

    // simulate activation (for keyboard or programmatic use)
    void activate();

    // update animation (dt seconds)
    void update(sf::Time dt);

    // draw
    void draw(sf::RenderWindow& window);

    // check if point inside bounds
    bool contains(const sf::Vector2f& p) const;

private:
    sf::RectangleShape card;
    sf::Text labelText;
    const sf::Font* fontRef;                // pointer to font (non-owning)

    std::function<void()> callback;

    // animation state
    bool hovered = false;
    bool pressed = false;
    float scaleAnim = 1.0f;
    float targetScale = 1.0f;

    // colors
    sf::Color baseColor    = sf::Color(50,50,60);
    sf::Color hoverColor   = sf::Color(96,120,200);
    sf::Color pressColor   = sf::Color(40,40,50);
    sf::Color outlineColor = sf::Color::White;
};

