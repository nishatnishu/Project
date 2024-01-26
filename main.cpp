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
bool gameOver= false;//6.gameoverr

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
    for (int i = 0; i < NUM_OBSTACLES; ++i) {
        Obstacle obstacle;
        obstacle.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
        obstacle.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
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
//6.GameOver: 
void displayGameOver()
{
    //6.gameover
    SDL_Color textColor ={255,0,0,255};//reD
    string gameOverText="GAME OVER";
    SDL_Surface* gameOverSurface =TTF_RenderText_Solid(font,gameOverText.c_str(),textColor);
    SDL_Texture* gameOverTexture=SDL_CreateTextureFromSurface(renderer,gameOverSurface);
    SDL_FreeSurface(gameOverSurface);
    SDL_Rect gameOverRect={SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2-20,200,40};
    SDL_RenderCopy(renderer,gameOverTexture,nullptr, &gameOverRect);
    SDL_DestroyTexture(gameOverTexture);
    //6.diplaying score: 
    SDL_Color scoreColor ={255,255,255,255};//white
    string scoreText="Score: "+ to_string(score);
    SDL_Surface* scoreSurface =TTF_RenderText_Solid(font,scoreText.c_str(),scoreColor);
    SDL_Texture* scoreTexture= SDL_CreateTextureFromSurface(renderer,scoreSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_Rect scoreRect ={SCREEN_WIDTH/2-50,SCREEN_HEIGHT/2+20,100,30};
    SDL_RenderCopy(renderer,scoreTexture,nullptr, &scoreRect);
    SDL_RenderPresent(renderer);


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

    // collision with obstacles or boundaries
    if (newHead.first < 0 || newHead.first >= SCREEN_WIDTH || newHead.second < 0 || newHead.second >= SCREEN_HEIGHT || checkCollision(newHead.first, newHead.second)) {
        close();
        cout<<"GAME OVER. YOUr Score: "<<score<<endl;
        gameOver=true;
        //exit(1);
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
}

void render() {
   // SDL_SetRenderDrawColor(renderer, 10, 100, 200, 255);//blue
    SDL_SetRenderDrawColor(renderer,0,0,0,255);//black backgound
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//whitee snake

    // changing the snake color and shape(pinkish hobe)
   for (int i = 0; i < snake.body.size(); ++i) {
   // SDL_SetRenderDrawColor(renderer,255,255,255,255);//white snake
        int colorValue = 255 - i * 10;//white+pinkk..
        SDL_SetRenderDrawColor(renderer, 255, colorValue, colorValue, 255);

        if (i == 0) // head
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
        }

        int radius = GRID_SIZE / 2;
        int centerX = snake.body[i].first + radius;
        int centerY = snake.body[i].second + radius;
       // filledCircleRGBA(renderer, centerX, centerY, radius, 255, 255,255, 255);
      filledCircleRGBA(renderer, centerX, centerY, radius, 255, colorValue, colorValue, 255);//pinshhh
    }

    // food
    SDL_SetRenderDrawColor(renderer, 0,255, 0, 255);
    SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    // 3. bonus food
    if (bonusFood.active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);//yellow
        SDL_Rect bonusFoodRect = {bonusFood.x, bonusFood.y, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &bonusFoodRect);
    }

    // 1. Obstacle
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//black
    SDL_SetRenderDrawColor(renderer,10,100,200,255);
    int obstacleSizex= GRID_SIZE*5;//obstacle size wiil increse
    int obstacleSizey=GRID_SIZE*1.5;//y cordinate e 


    for (const auto& obstacle : obstacles) {
        //SDL_Rect obstacleRect = {obstacle.x, obstacle.y, GRID_SIZE, GRID_SIZE};
        SDL_Rect obstacleRect={obstacle.x,obstacle.y,obstacleSizex,obstacleSizey};//for increasing obs,. size
        SDL_RenderFillRect(renderer, &obstacleRect);
    }

    // 2. score
    SDL_Color textColor = {255, 255, 255, 255};//white
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
  // while (!quit){
  while(!quit && !gameOver) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.direction != 'D') snake.direction = 'U';
                        break;
                    case SDLK_DOWN:
                        if (snake.direction != 'U') snake.direction = 'D';
                        break;
                    case SDLK_LEFT:
                        if (snake.direction != 'R') snake.direction = 'L';
                        break;
                    case SDLK_RIGHT:
                        if (snake.direction != 'L') snake.direction = 'R';
                        break;
                }
            }
        }

        update();
        render();
     //6.gameover....
     if(gameOver){
        displayGameOver();
        SDL_Delay(2000);
     }else { 
        SDL_Delay(200);
     }
       // SDL_Delay(200);
    }

    close();
    return 0;
}