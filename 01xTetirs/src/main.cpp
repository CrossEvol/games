#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>

// setup keyboard
constexpr int M = 20;
constexpr int N = 10;
constexpr int RECT_SIZE = 18;
constexpr int FRAME_X = 28;
constexpr int FRAME_Y = 31;
constexpr int STEP = 1;
int field[M][N]{};

// prepare the tetirs of types
constexpr int COLOR_NUM = 7;
constexpr int TETIRS_TYPE_NUM = 7;
constexpr int POINTS = 4;
constexpr int CENTER_INDEX = 1;
int figures[TETIRS_TYPE_NUM][POINTS] =
{
    1, 3, 5, 7, // I
    2, 4, 5, 7, // Z
    3, 5, 4, 6, // S
    3, 5, 4, 7, // T
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5, // O
};

// tetirs consist of four points
struct Point {
    int x;
    int y;
};

struct State {
    int dx = 0;
    bool rotate = false;
    int colorNum = 1;
    float timer = 0;
    float delay = 0.3;

public:
    void reset() {
        dx = 0;
        rotate = false;
        delay = 0.3;
    }
};

Point curr[POINTS];
Point prev[POINTS];

bool check() {
    // should be in boundary
    // should do simple conflicts detection with the existed tetirs
    return !std::any_of(std::begin(curr), std::end(curr),
                        [](const Point &p) {
                            return p.x < 0 || p.x >= N || p.y >= M || field[p.y][p.x];
                        });
}

int main() {
    // c++ 11 random generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> colorDist(1, COLOR_NUM);
    std::uniform_int_distribution<> typeDist(0, TETIRS_TYPE_NUM - 1);

    auto window = sf::RenderWindow(sf::VideoMode(320, 480), "The Game!");
    window.setFramerateLimit(144);

    // load assets;
    sf::Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/frame.png");
    sf::Sprite s(t1), background(t2), frame(t3);

    State state;
    sf::Clock clock;

    while (window.isOpen()) {
        auto time = clock.getElapsedTime().asSeconds();
        clock.restart();
        state.timer += time;

        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    state.rotate = true;
                } else if (event.key.code == sf::Keyboard::Down) {
                    state.delay = 0.05;
                } else if (event.key.code == sf::Keyboard::Left) {
                    state.dx = -1;
                } else if (event.key.code == sf::Keyboard::Right) {
                    state.dx = 1;
                }
            }
        }

        //// <- Move -> ///
        for (int i = 0; i < POINTS; ++i) {
            prev[i] = curr[i];
            curr[i].x += state.dx;
        }
        if (!check()) {
            for (int i = 0; i < POINTS; ++i) {
                curr[i] = prev[i];
            }
        }

        //////Rotate//////
        if (state.rotate) {
            auto &[cx, cy] = curr[CENTER_INDEX];
            for (auto &i: curr) {
                auto rx = i.y - cy;
                auto ry = i.x - cx;
                i.x = cx - rx;
                i.y = cy + ry;
            }
            if (!check()) {
                for (int i = 0; i < POINTS; ++i) {
                    curr[i] = prev[i];
                }
            }
        }

        ///////Tick//////
        if (state.timer > state.delay) {
            for (int i = 0; i < POINTS; ++i) {
                prev[i] = curr[i];
                curr[i].y += STEP;
            }
            if (!check()) {
                // fill the board
                for (auto &[x,y]: prev) {
                    field[y][x] = state.colorNum;
                }

                // prepare the next tetirs
                state.colorNum = colorDist(gen);
                int n = typeDist(gen);
                for (int i = 0; i < POINTS; ++i) {
                    curr[i].x = figures[n][i] % 2;
                    curr[i].y = figures[n][i] / 2;
                }
            }
            state.timer = 0;
        }

        ///////check lines//////////
        auto k = M - 1;
        for (auto i = M - 1; i > 0; --i) {
            auto count = 0;
            for (auto j = 0; j < N; ++j) {
                if (field[i][j]) {
                    ++count;
                }
                field[k][j] = field[i][j];
            }
            // if the current line is full, will be overwritten by the next line
            // the visual effect is that the full line has disappeared
            if (count < N) k--;
        }

        // reset state
        state.reset();

        /////////draw//////////
        window.clear(sf::Color::White);
        window.draw(background);

        // draw the board
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                if (field[i][j] == 0) { continue; }
                s.setTextureRect(sf::IntRect(field[i][j] * RECT_SIZE, 0, RECT_SIZE, RECT_SIZE));
                s.setPosition(static_cast<float>(j * RECT_SIZE), static_cast<float>(i * RECT_SIZE));
                s.move(FRAME_X, FRAME_Y);
                window.draw(s);
            }
        }

        // draw the next tetirs
        for (auto &[x, y]: curr) {
            s.setTextureRect(sf::IntRect(state.colorNum * RECT_SIZE, 0, RECT_SIZE, RECT_SIZE));
            s.setPosition(static_cast<float>(x * RECT_SIZE), static_cast<float>(y * RECT_SIZE));
            s.move(FRAME_X, FRAME_Y);
            window.draw(s);
        }

        window.draw(frame); // duplicate with the background
        window.display();
    }

    return 0;
}
