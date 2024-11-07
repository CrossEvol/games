#include <memory>
#include <random>
#include <SFML/Graphics.hpp>

constexpr int N = 30;
constexpr int M = 20;
constexpr int SIZE = 16;
constexpr int MODE_WIDTH = SIZE * N;
constexpr int MODE_HEIGHT = SIZE * M;

class Random {
public:
    static int nextInt(const int min = 0, const int max = INT_MAX) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> distrib(min, max);

        return distrib(gen);
    }
};

struct  Point {
    int x;
    int y;
};

using Block = Point;
using Snake = Point;
using Fruct = Point;

enum class Direction {
    Up, Down, Left, Right
};

class SnakeGame {
    std::vector<Snake> snakes;
    int snake_size;
    std::vector<Block> blocks;
    sf::Texture blockTexture;
    sf::Sprite sBlock;
    sf::Texture snakeTexture;
    sf::Sprite sSnake;
    Fruct fruct;

private:
    void drawBlocks(sf::RenderWindow &window) {
        for (const auto &[x, y]: blocks) {
            sBlock.setPosition(static_cast<float>(x * SIZE), static_cast<float>(y * SIZE));
            window.draw(sBlock);
        }
    }

    void drawSnakes(sf::RenderWindow &window) {
        for (int i = 0; i < snake_size; ++i) {
            const auto &[x, y] = snakes[i];
            sSnake.setPosition(static_cast<float>(x * SIZE), static_cast<float>(y * SIZE));
            window.draw(sSnake);
        }
    }

    void drawFruct(sf::RenderWindow &window) {
        sSnake.setPosition(static_cast<float>(fruct.x * SIZE), static_cast<float>(fruct.y * SIZE));
        window.draw(sSnake);
    }

public:
    Direction direction;

    SnakeGame(): snake_size{4}, fruct{10, 10}, direction(Direction::Down) {
        blockTexture.loadFromFile("images/white.png");
        sBlock.setTexture(blockTexture);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                blocks.emplace_back(Block{i, j});
            }
        }

        snakeTexture.loadFromFile("images/red.png");
        sSnake.setTexture(snakeTexture);
        for (int i = 0; i <= 100; ++i) {
            snakes.emplace_back(Snake{0, 0});
        }
    }

    ~SnakeGame() = default;

    void draw(sf::RenderWindow &window) {
        drawBlocks(window);
        drawSnakes(window);
        drawFruct(window);
    }

    void moveHead() {
        switch (direction) {
            case Direction::Up:
                snakes[0].y -= 1;
                break;
            case Direction::Down:
                snakes[0].y += 1;
                break;
            case Direction::Left:
                snakes[0].x -= 1;
                break;
            case Direction::Right:
                snakes[0].x += 1;
                break;
        }

        if (snakes[0].x < 0) {
            snakes[0].x = N;
        } else if (snakes[0].x > N) {
            snakes[0].x = 0;
        }

        if (snakes[0].y < 0) {
            snakes[0].y = M;
        } else if (snakes[0].y > M) {
            snakes[0].y = 0;
        }
    }

    void eatFruct() {
        if (snakes[0].x == fruct.x && snakes[0].y == fruct.y) {
            ++snake_size;
            fruct.x = Random::nextInt() % N;
            fruct.y = Random::nextInt() % M;
        }
    }

    void moveBody() {
        for (auto i = snake_size; i > 0; i--) {
            snakes[i].x = snakes[i - 1].x;
            snakes[i].y = snakes[i - 1].y;
        }
    }

    void selfBite() {
        for (int i = 1; i < snake_size; ++i) {
            if (const auto &[x, y] = snakes[i]; snakes[0].x == x && snakes[0].y == y) {
                snake_size = i;
                break;
            }
        }
    }

    void Tick() {
        moveBody();
        moveHead();
        eatFruct();
        selfBite();
    }
};

int main() {
    auto window = sf::RenderWindow(sf::VideoMode(MODE_WIDTH, MODE_HEIGHT), "Snake Game!");
    window.setFramerateLimit(144);

    float timer = 0;

    SnakeGame snake{};
    sf::Clock clock;

    while (window.isOpen()) {
        auto time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            snake.direction = Direction::Left;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            snake.direction = Direction::Right;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            snake.direction = Direction::Up;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            snake.direction = Direction::Down;
        }

        if (float delay = 0.1; timer > delay) {
            timer = 0;
            snake.Tick();
        }

        window.clear();
        snake.draw(window);
        window.display();
    }
}
