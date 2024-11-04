#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <random>

class Paddle;
class Ball;
class Arkanoid;
class Random;

class Random {
public:
    static int nextInt(const int min = 0, const int max = INT_MAX) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> distrib(min, max);

        return distrib(gen);
    }
};

constexpr int MODE_WIDTH = 520;
constexpr int MODE_HEIGHT = 450;

enum class FloatRectType {
    withBlock,
    withPaddle,
};

class Ball {
    int x{300};
    int y{300};
    int dx{6};
    int dy{5};
    sf::Texture texture;
    sf::Sprite sBall;

    sf::FloatRect FloatRect(const FloatRectType type) const {
        switch (type) {
            case FloatRectType::withBlock:
                return {static_cast<float>(x + 3), static_cast<float>(y + 3), 6, 6};
            case FloatRectType::withPaddle:
                return {static_cast<float>(x), static_cast<float>(y), 12, 12};
        }
        throw std::runtime_error("Unknown FloatRectType");
    }

    void reverseDx() {
        dx = -dx;
    }

    void reverseDy() {
        dy = -dy;
    }

public:
    explicit Ball() {
        texture.loadFromFile("images/ball.png");
        sBall.setTexture(texture);
    }

    ~Ball() = default;

    void draw(sf::RenderWindow &window) {
        sBall.setPosition(static_cast<float>(x), static_cast<float>(y));
        window.draw(sBall);
    }

    void move(std::vector<sf::Sprite> &blocks) {
        x += dx;
        for (auto &block: blocks) {
            if (FloatRect(FloatRectType::withBlock).intersects(block.getGlobalBounds())) {
                block.setPosition(static_cast<float>(-100), static_cast<float>(0));
                reverseDx();
            }
        }
        y += dy;
        for (auto &block: blocks) {
            if (FloatRect(FloatRectType::withBlock).intersects(block.getGlobalBounds())) {
                block.setPosition(static_cast<float>(-100), static_cast<float>(0));
                reverseDy();
            }
        }
        if (x < 0 || x > MODE_WIDTH) reverseDx();
        if (y < 0 || y > MODE_HEIGHT) reverseDy();
    }

    void detectCollisionWithPaddle(sf::FloatRect bound) {
        if (FloatRect(FloatRectType::withPaddle).intersects(bound)) {
            dy = -(Random::nextInt() % 5 + 2);
        }
    }
};

enum class Direction {
    left,
    right
};

class Paddle {
    int x{300};
    int y{440};
    sf::Texture texture;
    sf::Sprite sPaddle;

public:
    explicit Paddle() {
        texture.loadFromFile("images/paddle.png");
        sPaddle.setTexture(texture);
    }

    ~Paddle() = default;

    sf::FloatRect paddleBound() {
        return sPaddle.getGlobalBounds();
    }

    void draw(sf::RenderWindow &window) {
        sPaddle.setPosition(static_cast<float>(x), static_cast<float>(y));
        window.draw(sPaddle);
    }

    void move(const Direction direction) {
        switch (direction) {
            case Direction::left:
                x -= 6;
                sPaddle.setPosition(static_cast<float>(x), static_cast<float>(y));
                break;
            case Direction::right:
                x += 6;
                sPaddle.setPosition(static_cast<float>(x), static_cast<float>(y));
                break;
        }
    }
};

class Arkanoid {
    constexpr static int BLOCK_WIDTH = 43;
    constexpr static int BLOCK_HEIGHT = 20;
    std::vector<sf::Sprite> blocks{100};
    std::shared_ptr<Ball> ball;
    std::shared_ptr<Paddle> paddle;
    sf::Sprite sBackground;
    sf::Sprite sBlock;
    sf::Texture blockTexture;
    sf::Texture backgroundTexture;

public:
    Arkanoid(const std::shared_ptr<Ball> &ball,
             const std::shared_ptr<Paddle> &paddle)
        : ball(ball),
          paddle(paddle) {
        backgroundTexture.loadFromFile("images/background.jpg");
        sBackground.setTexture(backgroundTexture);

        blockTexture.loadFromFile("images/block01.png");
        sBlock.setTexture(blockTexture);

        auto n = 0;
        for (int i = 1; i <= 10; ++i) {
            for (int j = 1; j <= 10; ++j) {
                blocks[n].setTexture(blockTexture);
                blocks[n].setPosition(static_cast<float>(i * BLOCK_WIDTH), static_cast<float>(j * BLOCK_HEIGHT));
                n++;
            }
        }
    }

    ~Arkanoid() = default;

    void detectCollision() {
        ball->detectCollisionWithPaddle(paddle->paddleBound());
    }

    void draw(sf::RenderWindow &window) {
        window.draw(sBackground);
        ball->draw(window);
        paddle->draw(window);
        for (const auto &block: blocks) {
            window.draw(block);
        }
    }

    void movePaddle(const Direction direction) {
        paddle->move(direction);
    }

    void moveBall() {
        ball->move(blocks);
    }

    void watchKeyboard() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) this->movePaddle(Direction::right);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) this->movePaddle(Direction::left);
    }
};

int main() {
    auto window = sf::RenderWindow(sf::VideoMode(MODE_WIDTH, MODE_HEIGHT), "Arkanoid!");
    window.setFramerateLimit(60);

    auto ball = std::make_shared<Ball>();
    auto paddle = std::make_shared<Paddle>();
    Arkanoid arkanoid(ball, paddle);

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        arkanoid.moveBall();

        arkanoid.watchKeyboard();

        arkanoid.detectCollision();

        window.clear();
        arkanoid.draw(window);

        window.display();
    }
}
