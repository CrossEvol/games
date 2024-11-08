#include <iostream>
#include <memory>
#include <random>
#include <SFML/Graphics.hpp>

constexpr int BLOCK_SIZE = 64;
constexpr int MATRIX_SIZE = 4;
constexpr int MODE_WIDTH = BLOCK_SIZE * MATRIX_SIZE;
constexpr int MODE_HEIGHT = BLOCK_SIZE * MATRIX_SIZE;

struct Point {
    int x;
    int y;
};

enum class Block:int {
    One = 1, Two, Three, Four, Five, Six, Seven, Eight, Night, Ten, Eleven, Twelve, Thirteen, Fourteen, Fifteen, Empty
};


class FifteenPuzzle {
    sf::Texture texture;
    std::vector<sf::Sprite> sprites;
    std::vector<std::vector<Block> > matrix;
    sf::RenderWindow &window;

    void setup() {
        std::vector<int> numbers(16);
        for (int i = 0; i < 16; ++i) {
            numbers[i] = i + 1;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(numbers.begin(), numbers.end(), gen);

        auto index = 0;
        for (int i = 1; i <= MATRIX_SIZE; ++i) {
            for (int j = 1; j <= MATRIX_SIZE; ++j) {
                matrix[i][j] = static_cast<Block>(numbers[index]);
                ++index;
            }
        }
    }

public:
    void reset() {
        setup();
    }

    explicit FifteenPuzzle(sf::RenderWindow &window): window(window) {
        texture.loadFromFile("images/15.png");
        auto count = 0;

        sprites.resize(16);
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                sprites[count].setTexture(texture);
                sprites[count].setTextureRect(sf::IntRect(i * BLOCK_SIZE, j * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
                ++count;
            }
        }

        matrix.resize(MATRIX_SIZE + 2);
        for (int i = 0; i < MATRIX_SIZE + 2; ++i) {
            matrix[i].resize(MATRIX_SIZE + 2);
        }
        setup();
    }

    void draw() {
        for (int i = 1; i <= MATRIX_SIZE; ++i) {
            for (int j = 1; j <= MATRIX_SIZE; ++j) {
                const auto index = static_cast<size_t>(matrix[i][j]) - 1;
                sprites[index].setPosition(static_cast<float>(BLOCK_SIZE * (i - 1)),
                                           static_cast<float>(BLOCK_SIZE * (j - 1)));
                const auto &sprite = sprites[index];
                window.draw(sprite);
            }
        }
    }

    bool check() const {
        auto index = 0;
        for (int i = 1; i <= BLOCK_SIZE; ++i) {
            for (int j = 1; j <= BLOCK_SIZE; ++j) {
                ++index;
                if (matrix[i][j] != static_cast<Block>(index)) {
                    return false;
                }
            }
        }
        return true;
    }

    void move(const Point point) {
        const auto x = point.x / BLOCK_SIZE + 1;
        const auto y = point.y / BLOCK_SIZE + 1;

        int dx = 0;
        int dy = 0;

        if (matrix[x][y - 1] == Block::Empty) {
            dx = 0;
            dy = -1;
        } else if (matrix[x][y + 1] == Block::Empty) {
            dx = 0;
            dy = 1;
        } else if (matrix[x - 1][y] == Block::Empty) {
            dx = -1;
            dy = 0;
        } else if (matrix[x + 1][y] == Block::Empty) {
            dx = 1;
            dy = 0;
        }

        auto block = matrix[x][y];
        matrix[x][y] = Block::Empty;
        matrix[x + dx][y + dy] = block;

        const auto source = static_cast<size_t>(block) - 1;
        constexpr auto target = static_cast<size_t>(Block::Empty) - 1;
        sprites[target].move(static_cast<float>(-dx * BLOCK_SIZE), static_cast<float>(-dy * BLOCK_SIZE));

        for (int i = 0, speed = 4; i < BLOCK_SIZE; i += speed) {
            sprites[source].move(static_cast<float>(dx * speed), static_cast<float>(dy * speed));
            window.draw(sprites[target]);
            window.draw(sprites[source]);
            window.display();
        }

        if (check()) {
            std::cout << "you have succeed." << std::endl;
            window.clear(sf::Color::White);
            this->draw();
            window.display();
        }
    }
};

int main() {
    auto window = sf::RenderWindow(sf::VideoMode{MODE_WIDTH, MODE_HEIGHT}, "15-Puzzle!");
    window.setFramerateLimit(60);

    FifteenPuzzle puzzle{window};

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto a = sf::Mouse::getPosition(window);
                    puzzle.move({a.x, a.y});
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    puzzle.reset();
                }
            }
        }

        window.clear(sf::Color::White);
        puzzle.draw();
        window.display();
    }
}
