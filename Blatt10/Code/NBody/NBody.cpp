#include <cmath>
#include "include/Body.h"
#include <iostream>
#include <SFML/Graphics.hpp>

constexpr float G = 1.f;
constexpr float dt = .1f;
constexpr float eps = 1e-1f;
constexpr size_t n_bodies = 5;
constexpr float center_mass = 1000.f;
constexpr float TARGET_FPS = 165.f;
const sf::Time FRAME_DURATION = sf^::seconds(1.f / TARGET_FPS);

// Fenstergröße
const int WIDTH = 2560, HEIGHT = 1440;
// Fenster erstellen
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation");

// Schriftart laden
sf::Font font;
if (!font.loadFromFile("OpenSans-Bold.ttf")) {
    std::cerr << "Failed to load font\n";
    return 1;
}

// FPS Text vorbereiten
sf::Text fpsText("", font, 18);
fpsText.setFillColor(sf::Color::White);
fpsText.setPosition(10, 5);

int main() {



    // Körper initialisieren
    std::vector<Body> bodies;

    return 0;
}