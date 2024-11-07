#include <memory>
#include <random>
#include <SFML/Graphics.hpp>

constexpr int MODE_WIDTH = 400;
constexpr int MODE_HEIGHT = 400;
constexpr int MATRIX_SIZE = 10;
constexpr int SAFE_MATRIX_SIZE = MATRIX_SIZE + 2;
constexpr int BLOCK_SIZE = 32;

namespace IRandom {
    auto nextInt(const int min = 0, const int max = INT_MAX) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> distrib(min, max);

        return distrib(gen);
    }
};

struct Point {
    int x;
    int y;
};

enum class BlockType:int {
    Empty, One, Two, Three, Four, Five, Six, Seven, Eight, Bomb, Mask, Flag
};

class Minesweeper {
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<std::vector<BlockType> > matrix;
    std::vector<std::vector<BlockType> > mask;
    Point point;

public:
    void setup() {
        matrix.resize(SAFE_MATRIX_SIZE, std::vector<BlockType>(SAFE_MATRIX_SIZE));
        for (int i = 0; i < SAFE_MATRIX_SIZE; ++i) {
            for (int j = 0; j < SAFE_MATRIX_SIZE; ++j) {
                if (IRandom::nextInt() % 5 == 0) {
                    matrix[i][j] = BlockType::Bomb;
                } else {
                    matrix[i][j] = BlockType::Empty;
                }
            }
        }

        for (auto i = 1; i <= MATRIX_SIZE; ++i) {
            for (auto j = 1; j <= MATRIX_SIZE; ++j) {
                if (matrix[i][j] == BlockType::Bomb) continue;
                auto count = 0;
                if (matrix[i - 1][j - 1] == BlockType::Bomb) ++count;
                if (matrix[i - 1][j] == BlockType::Bomb) ++count;
                if (matrix[i - 1][j + 1] == BlockType::Bomb) ++count;
                if (matrix[i][j - 1] == BlockType::Bomb) ++count;
                if (matrix[i][j + 1] == BlockType::Bomb) ++count;
                if (matrix[i + 1][j - 1] == BlockType::Bomb) ++count;
                if (matrix[i + 1][j] == BlockType::Bomb) ++count;
                if (matrix[i + 1][j + 1] == BlockType::Bomb) ++count;
                matrix[i][j] = static_cast<BlockType>(count);
            }
        }

        mask.resize(SAFE_MATRIX_SIZE, std::vector<BlockType>(SAFE_MATRIX_SIZE));
        for (int i = 0; i < SAFE_MATRIX_SIZE; ++i) {
            for (int j = 0; j < SAFE_MATRIX_SIZE; ++j) {
                mask[i][j] = BlockType::Mask;
            }
        }
    }

    Minesweeper(): point{0, 0} {
        texture.loadFromFile("images/tiles.jpg");
        sprite = sf::Sprite(texture);

        setup();
    };

    void draw(sf::RenderWindow &window) {
        for (auto i = 1; i <= MATRIX_SIZE; ++i) {
            for (auto j = 1; j <= MATRIX_SIZE; ++j) {
                sprite.setTextureRect(sf::IntRect(static_cast<int>(mask[i][j]) * BLOCK_SIZE, 0, BLOCK_SIZE,
                                                  BLOCK_SIZE));
                sprite.setPosition(static_cast<float>(i * BLOCK_SIZE), static_cast<float>(j * BLOCK_SIZE));
                window.draw(sprite);
            }
        }
    }

    void click(const sf::Vector2i vector, const bool isFlag = true) {
        const auto x = vector.x / BLOCK_SIZE;
        const auto y = vector.y / BLOCK_SIZE;
        point.x = x;
        point.y = y;
        if (isFlag) {
            mask[x][y] = BlockType::Flag;
            return;
        }

        if (matrix[x][y] != BlockType::Bomb) {
            mask[x][y] = matrix[x][y];
            return;
        }

        for (auto i = 1; i <= MATRIX_SIZE; ++i) {
            for (auto j = 1; j <= MATRIX_SIZE; ++j) {
                mask[i][j] = matrix[i][j];
            }
        }
    }

    void reset() {
        setup();
    }
};

int main() {
    auto window = sf::RenderWindow(sf::VideoMode{MODE_WIDTH, MODE_HEIGHT}, "Minesweeper!");
    window.setFramerateLimit(144);

    Minesweeper minesweeper{};

    while (window.isOpen()) {
        auto &&position = sf::Mouse::getPosition(window);

        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    minesweeper.click(position, false);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    minesweeper.click(position);
                } else if (event.mouseButton.button == sf::Mouse::Middle) {
                    minesweeper.reset();
                }
            }
        }

        window.clear(sf::Color::White);
        minesweeper.draw(window);
        window.display();
    }
}
