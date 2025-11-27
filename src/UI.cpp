#include "UI.h"
#include <cmath>

ElevatedButton::ElevatedButton(const sf::Font& font, const std::string& label, const sf::Vector2f& size)
: labelText(font, label, 20u), fontRef(&font)
{
    // configure card
    card.setSize(size);
    card.setFillColor(baseColor);
    card.setOutlineColor(outlineColor);
    card.setOutlineThickness(2.f);
    card.setOrigin(sf::Vector2f(0.f, 0.f));

    // labelText was constructed with font already
    labelText.setFillColor(sf::Color::White);

    // center label inside card
    // compute local bounds and set origin to center
    sf::FloatRect tb = labelText.getLocalBounds();
    sf::Vector2f origin{ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f };
    labelText.setOrigin(origin);

    // position label at center of card (local)
    labelText.setPosition(card.getPosition() + sf::Vector2f(size.x / 2.f, size.y / 2.f));
}

void ElevatedButton::setPosition(const sf::Vector2f& pos) {
    card.setPosition(pos);
    // label position relative to card's position & current scale
    sf::Vector2f size = card.getSize();
    labelText.setPosition(pos + sf::Vector2f(size.x/2.f, size.y/2.f));
}

void ElevatedButton::setLabel(const std::string& s) {
    if (!fontRef) return;
    labelText.setString(s);
    sf::FloatRect tb = labelText.getLocalBounds();
    sf::Vector2f origin{ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f };
    labelText.setOrigin(origin);

    // keep centered on card
    sf::Vector2f pos = card.getPosition();
    sf::Vector2f size = card.getSize();
    labelText.setPosition(pos + sf::Vector2f(size.x/2.f, size.y/2.f));
}

void ElevatedButton::setCallback(std::function<void()> cb) {
    callback = std::move(cb);
}

bool ElevatedButton::contains(const sf::Vector2f& p) const {
    return card.getGlobalBounds().contains(p);
}

void ElevatedButton::handleMouse(const sf::Vector2f& mouseWorldPos, bool mousePressedNow) {
    bool wasHovered = hovered;
    hovered = contains(mouseWorldPos);

    // pressed detection: set pressed when mouseDown while hovered
    if (hovered && mousePressedNow && !pressed) {
        pressed = true;
    } else if (pressed && !mousePressedNow) {
        // mouse released after being pressed
        if (hovered && callback) callback();
        pressed = false;
    }

    // when mouse hovers, targetScale increases slightly
    targetScale = hovered ? 1.03f : 1.0f;
}

void ElevatedButton::activate() {
    if (callback) callback();
}

void ElevatedButton::update(sf::Time dt) {
    // smooth scale animation (simple ease)
    float speed = 8.f;
    float t = std::min(1.f, speed * dt.asSeconds());
    scaleAnim += (targetScale - scaleAnim) * t;

    // set scale on card
    card.setScale({scaleAnim, scaleAnim});

    // shading / outline based on hover
    if (hovered) {
        card.setFillColor(hoverColor);
        card.setOutlineThickness(3.f);
    } else {
        card.setFillColor(baseColor);
        card.setOutlineThickness(2.f);
    }

    // pressed color handled visually on draw (no permanent move)
    if (pressed) {
        card.setFillColor(pressColor);
    }
}

void ElevatedButton::draw(sf::RenderWindow& window) {
    // compute pressed offset (visual only)
    float pressedOffset = pressed ? 1.5f : 0.f;

    // draw shadow (copy of card)
    sf::RectangleShape shadow = card;
    shadow.setFillColor(sf::Color(0,0,0,90));
    shadow.setOutlineThickness(0.f);

    // shadow should be drawn behind card, offset a bit (scaled same as card)
    shadow.setScale(card.getScale());
    shadow.setPosition(card.getPosition() + sf::Vector2f(3.f + pressedOffset, 6.f + pressedOffset));
    window.draw(shadow);

    // draw card
    // when drawing card, place it at scaled position (card already has scale set)
    sf::Vector2f drawPos = card.getPosition() + sf::Vector2f(0.f, pressedOffset);
    // we need to draw with the card at drawPos — temporarily set position
    sf::Vector2f prevPos = card.getPosition();
    card.setPosition(drawPos);
    window.draw(card);
    // restore position so subsequent logic relying on card.getPosition() stays consistent
    card.setPosition(prevPos);

    // update label position so it stays centered even when card scales
    sf::Vector2f size = card.getSize();
    sf::Vector2f scaledSize = sf::Vector2f(size.x * scaleAnim, size.y * scaleAnim);
    sf::Vector2f cardPos = card.getPosition();
    // center in scaled card
    labelText.setPosition(cardPos + sf::Vector2f(scaledSize.x / 2.f, scaledSize.y / 2.f) + sf::Vector2f(0.f, pressedOffset));
    // draw label
    window.draw(labelText);
}



