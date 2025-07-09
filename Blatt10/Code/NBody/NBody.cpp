#include <cmath>
#include "include/Body.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random> // um Bodies zu initialisieren


constexpr float G = 1.f;
constexpr float dt = .1f;
constexpr float eps = 1e-1f;
constexpr size_t n_bodies = 1000; /// hier ändern
constexpr float center_mass = 1000.f;


constexpr float TARGET_FPS = 165.f;
const sf::Time FRAME_DURATION = sf::seconds(1.f / TARGET_FPS);

// Fenstergröße
const int WIDTH = 2560, HEIGHT = 1440;
// Fenster erstellen
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation");



sf::Color mass_to_color(float m) {
    float norm = std::min(1.0f, m / 10.0f);
    return sf::Color(255 * norm, 50, 255 * (1 - norm));
}

float orbital_velocity_scalar(float M, float r) {
    return std::sqrt(1.0f * M / r); // G = 1.0 assumed
}


void initialize_bodies(std::vector<Body>& bodies, size_t n_bodies, float center_mass, int width, int height) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radius_dist(50.0f, std::min(width, height) / 2.f - 20.f);
    std::uniform_real_distribution<float> mass_dist(0.5f, 10.f);

    // Schwerer Körper im Zentrum
    bodies.push_back(Body{0.f, 0.f, 0.f, 0.f, center_mass});

    // Zufällige Körper
    for (size_t i = 0; i < n_bodies; ++i) {
        float angle = angle_dist(rng);
        float r = radius_dist(rng);
        float mass = mass_dist(rng);

        float x = r * std::cos(angle);
        float y = r * std::sin(angle);

        // Kreisbahn-Geschwindigkeit
        float v = orbital_velocity_scalar(center_mass, r);
        float vx = -v * std::sin(angle);
        float vy = v * std::cos(angle);

        bodies.push_back(Body{x, y, vx, vy, mass});
    }
}

int main() {
    // Schriftart laden
    sf::Font font;
    if (!font.loadFromFile("../OpenSans-Bold.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // FPS Text vorbereiten
    sf::Text fpsText("", font, 18);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 5);

    // Körper initialisieren
    std::vector<Body> bodies;
    initialize_bodies(bodies, n_bodies, center_mass, WIDTH, HEIGHT);

    // Uhr zur FPS-Berechnung
    sf::Clock frameClock;
    sf::Clock fpsClock;
    float lastFPS = 0.f;

    // Haupt-Loop
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();

        compute_forces(bodies, G, eps);
        integrate_bodies(bodies, dt);

        window.clear(sf::Color::Black);


        for (const Body& b : bodies) {
            sf::CircleShape circle(b.mass > 50.0f ? 6 : 2);
            circle.setFillColor(mass_to_color(b.mass));
            circle.setPosition(WIDTH / 2 + b.posX, HEIGHT / 2 + b.posY);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            window.draw(circle);
        }

        // FPS calculation and display
        float fpsElapsed = fpsClock.restart().asSeconds();
        lastFPS = 1.0f / fpsElapsed;
        fpsText.setString("FPS: " + std::to_string((int)lastFPS));
        window.draw(fpsText);
    
        window.display();
    
        sf::Time frameElapsed = frameClock.getElapsedTime();
        if (frameElapsed < FRAME_DURATION) sf::sleep(FRAME_DURATION - frameElapsed);
        frameClock.restart();
    }

    return 0;
}
