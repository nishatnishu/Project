#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
const int INITIAL_LENGTH = 5;
const int NUM_OBSTACLES = 5;
const int BONUS_FOOD_INTERVAL = 5;
const int BONUS_FOOD_DURATION = 5000; // milliseconds

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

struct Snake {
    vector<pair<int, int>> body;
    char direction;
};

struct Food {
    int x, y;
};

struct Obstacle {
    int x, y;
    int speedX, speedY;
};

struct BonusFood {
    int x, y;
    bool active;
    Uint32 spendtime; // timer bonusfood
};

Snake snake;
Food food;
BonusFood bonusFood;
vector<Obstacle> obstacles;
int score = 0;
int foodsEaten = 0;

// 5. circle shape
void filledCircleRGBA(SDL_Renderer* renderer, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("nishat.ttf", 24);

    snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.direction = 'R';

    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;

    bonusFood.active = false;
    // 1. obstacles
    // Reduced the number of obstacles to 4
    for (int i = 0; i < NUM_OBSTACLES; ++i) {
        Obstacle obstacle;
        obstacle.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
        obstacle.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
        obstacle.speedX = 2 + rand() % 5; // Random speed between 2 and 6
        obstacle.speedY = 2 + rand() % 5; // Random speed between 2 and 6
        obstacles.push_back(obstacle);
    }
}

void close() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void generateFood() {
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
}

// 3. bonus food
void generateBonusFood() {
    bonusFood.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    bonusFood.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
    bonusFood.active = true;
    bonusFood.spendtime = SDL_GetTicks();
}

// 1. collision with obstacle
bool checkCollision(int x, int y) {
    for (const auto& segment : snake.body) {
        if (segment.first == x && segment.second == y) {
            return true;
        }
    }
    for (const auto& obstacle : obstacles) {
        if (obstacle.x == x && obstacle.y == y) {
            return true;
        }
    }

    return false;
}

void update() {
    pair<int, int> newHead = snake.body.front();

    switch (snake.direction) {
        case 'U':
            newHead.second -= GRID_SIZE;
            break;
        case 'D':
            newHead.second += GRID_SIZE;
            break;
        case 'L':
            newHead.first -= GRID_SIZE;
            break;
        case 'R':
            newHead.first += GRID_SIZE;
            break;
    }

    // 7. snake back
    if (newHead.first < 0) {
        newHead.first = SCREEN_WIDTH - GRID_SIZE;
    } else if (newHead.first >= SCREEN_WIDTH) {
        newHead.first = 0;
    }

    if (newHead.second < 0) {
        newHead.second = SCREEN_HEIGHT - GRID_SIZE;
    } else if (newHead.second >= SCREEN_HEIGHT) {
        newHead.second = 0;
    }

    // collision with obstacles or boundaries
    if (checkCollision(newHead.first, newHead.second)) {
        close();
        exit(1);
    }
    // Check collision with obstacles
    for (const auto& obstacle : obstacles) {
        if (newHead.first < obstacle.x + GRID_SIZE && newHead.first + GRID_SIZE > obstacle.x &&
            newHead.second < obstacle.y + GRID_SIZE && newHead.second + GRID_SIZE > obstacle.y) {
            close();
            exit(1);
        }
    }

    snake.body.insert(snake.body.begin(), newHead);

    if (newHead.first == food.x && newHead.second == food.y) {
        generateFood();
        score += 10;
        foodsEaten++;
        if (foodsEaten % BONUS_FOOD_INTERVAL == 0) {
            generateBonusFood();
        }
    } else if (newHead.first == bonusFood.x && newHead.second == bonusFood.y && bonusFood.active) {
        generateFood();
        score += 20;
        bonusFood.active = false;
    } else {
        snake.body.pop_back();
    }

    // 4. bonus food timer
    if (bonusFood.active && SDL_GetTicks() - bonusFood.spendtime > BONUS_FOOD_DURATION) {
        bonusFood.active = false;
    }

    // Update obstacle positions
    for (auto& obstacle : obstacles) {
        obstacle.x -= obstacle.speedX;
        obstacle.y -= obstacle.speedY;

        if (obstacle.x + GRID_SIZE < 0 || obstacle.x > SCREEN_WIDTH || obstacle.y + GRID_SIZE < 0 || obstacle.y > SCREEN_HEIGHT) {
            obstacle.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
            obstacle.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
            obstacle.speedX = 2 + rand() % 5;
            obstacle.speedY = 2 + rand() % 5;
        }
    }
}

void render() {
    // SDL_SetRenderDrawColor(renderer, 10, 100, 200, 255);//blue
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // whitee snake

    // changing the snake color and shape(pinkishh)
    for (int i = 0; i < snake.body.size(); ++i) {
        int colorValue = 255 - i * 10;
        SDL_SetRenderDrawColor(renderer, 255, colorValue, colorValue, 255);

        if (i == 0) // head
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
        }

        int radius = GRID_SIZE / 2;
        int centerX = snake.body[i].first + radius;
        int centerY = snake.body[i].second + radius;
        filledCircleRGBA(renderer, centerX, centerY, radius, 255, colorValue, colorValue, 255);
    }

    // food
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    // 3. bonus food
    if (bonusFood.active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
        SDL_Rect bonusFoodRect = {bonusFood.x, bonusFood.y, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &bonusFoodRect);
    }

    // 1. Obstacle
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//black
    SDL_SetRenderDrawColor(renderer, 10, 100, 200, 255);
    int obstacleSizex = GRID_SIZE * 12; // obstacle size will increase
    int obstacleSizey = GRID_SIZE * 1.5; // y coordinate e

    for (const auto& obstacle : obstacles) {
        // SDL_Rect obstacleRect = {obstacle.x, obstacle.y, GRID_SIZE, GRID_SIZE};
        SDL_Rect obstacleRect = {obstacle.x, obstacle.y, obstacleSizex, obstacleSizey}; // for increasing obs,. size
        SDL_RenderFillRect(renderer, &obstacleRect);
    }

    // 2. score
    SDL_Color textColor = {255, 255, 255, 255}; // white
    string scoreText = "Score: " + to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect scoreRect = {5, 5, 100, 30};
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(renderer);
}

// 5. after changing the snake color and shape
void filledCircleRGBA(SDL_Renderer* renderer, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    for (int dy = 0; dy <= radius; dy++) {
        for (int dx = 0; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                SDL_RenderDrawPoint(renderer, cx - dx, cy + dy);
                SDL_RenderDrawPoint(renderer, cx + dx, cy - dy);
                SDL_RenderDrawPoint(renderer, cx - dx, cy - dy);
            }
        }
    }
}

#ifdef _WIN32
int SDL_main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    initialize();

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.direction != 'D')
                            snake.direction = 'U';
                        break;
                    case SDLK_DOWN:
                        if (snake.direction != 'U')
                            snake.direction = 'D';
                        break;
                    case SDLK_LEFT:
                        if (snake.direction != 'R')
                            snake.direction = 'L';
                        break;
                    case SDLK_RIGHT:
                        if (snake.direction != 'L')
                            snake.direction = 'R';
                        break;
                }
            }
        }

        update();
        render();

        SDL_Delay(200);
    }

    close();
    return 0;
}