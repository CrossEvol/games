#include <memory>
#include <random>
#include <SFML/Graphics.hpp>

constexpr int POINTS_SIZE = 10;
constexpr int MODE_WIDTH = 400;
constexpr int MODE_HEIGHT = 533;
constexpr int PLATFORM_WIDTH = 68;
constexpr int PLATFORM_HEIGHT = 14;
constexpr int BEAK_LENGTH = 20;
constexpr int FOOT_LENGTH = 50;
constexpr int DOODLE_SIZE = 70;
constexpr float VELOCITY = 0.2;
constexpr int TOP = 200;

struct Point {
    int x{0};
    int y{0};
};


int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> randX(0, 400);
    std::uniform_int_distribution<> randY(0, 533);
    Point platforms[POINTS_SIZE];
    for (int i = 0; i < POINTS_SIZE; i++) {
        platforms[i].x = randX(gen) % MODE_WIDTH;
        platforms[i].y = randY(gen) % MODE_HEIGHT;
    }

    auto window = sf::RenderWindow(sf::VideoMode(MODE_WIDTH, MODE_HEIGHT), "Doodle Game!");
    window.setFramerateLimit(60);

    sf::Texture t1, t2, t3;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/platform.png");
    t3.loadFromFile("images/doodle.png");
    sf::Sprite sBackground(t1), sPlatform(t2), sDoodle(t3);

    int x = 100;
    int y = 100;
    float dy = 0;

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    x -= 3;
                } else if (event.key.code == sf::Keyboard::Right) {
                    x += 3;
                }
            }
        }

        dy += VELOCITY;
        y += dy;
        if (y > 500) dy = -10;
        if (y < TOP) {
            y = TOP;
            for (auto &platform: platforms) {
                platform.y = platform.y - dy;
                if (platform.y > MODE_HEIGHT) {
                    platform.y = 0;
                    platform.x = randX(gen) % MODE_WIDTH;
                }
            }
        }

        for (auto &platform: platforms) {
            if (x + FOOT_LENGTH > platform.x
                && x + BEAK_LENGTH < platform.x + PLATFORM_WIDTH
                && y + DOODLE_SIZE > platform.y
                && y + DOODLE_SIZE < platform.y + PLATFORM_HEIGHT) {
                dy = -10;
            }
        }


        window.clear();
        window.draw(sBackground);
        sDoodle.setPosition(static_cast<float>(x), static_cast<float>(y));
        window.draw(sDoodle);
        for (const auto &[px, py]: platforms) {
            sPlatform.setPosition(static_cast<float>(px), static_cast<float>(py));
            window.draw(sPlatform);
        }

        window.display();
    }

    return 0;
}
