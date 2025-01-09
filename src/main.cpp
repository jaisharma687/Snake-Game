#include <raylib.h>
#include <bits/stdc++.h>
#include <raymath.h>

using namespace std;

Color snakeColor = {24,49,79,255};
Color bgColor = {139,190,178,255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool elementInDeque(Vector2 element, deque<Vector2> deq){
    for(auto el:deq){
        if(Vector2Equals(el,element)){
            return true;
        }
    }
    return false;
}

class Snake{
    public:
        deque<Vector2> body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        Vector2 direction = {1,0};
        bool addSegment = false;
        void draw(){
            for(auto cell:body){
                Rectangle cellRect = {offset + cell.x*cellSize,offset + cell.y*cellSize,(float)cellSize,(float)cellSize};
                DrawRectangleRounded(cellRect,0.6,10,snakeColor);
            }
        }
        void update(){
            body.push_front(Vector2Add(body[0],direction));
            if(addSegment){
                addSegment = false;
            }
            else{
                body.pop_back();
            }
        }
        void reset(){
            body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
            direction = {1,0};
        }
};

class Food{
    public:
        Vector2 position;
        Texture2D foodTexture;
        Food(deque<Vector2> body){
            Image image = LoadImage("food.png");
            foodTexture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = generateRandomPosition(body);
        }
        ~Food(){
            UnloadTexture(foodTexture);
        }
        void draw(){
            DrawTexture(foodTexture,offset + position.x*cellSize,offset + position.y*cellSize,snakeColor);
        }

        Vector2 generateRandomCell(){
            Vector2 pos;
            pos.x = (rand()%cellCount);
            pos.y = (rand()%cellCount);
            return pos;
        }

        Vector2 generateRandomPosition(deque<Vector2> body){
            Vector2 pos = generateRandomCell();
            while(elementInDeque(pos,body)){
                pos = generateRandomCell();
            }
            return pos;
        }
};

class Game{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        Sound eatSound;
        Sound wallSound;
        int score = 0;

        Game(){
            InitAudioDevice();
            eatSound = LoadSound("eat.mp3");
            wallSound = LoadSound("wall.mp3");
        }

        ~Game(){
            UnloadSound(eatSound);
            UnloadSound(wallSound);
            CloseAudioDevice(); 
        }

        void draw(){
            food.draw();
            snake.draw();
        }
        void update(){
            if(running){
                snake.update();
                checkCollisionWithFood();
                checkCollisionWithWall();
                checkCollisionWithSelf();
            }
        }
        void checkCollisionWithFood(){
            if(Vector2Equals(snake.body[0],food.position)){
                snake.addSegment = true;
                food.position = food.generateRandomPosition(snake.body);
                score++;
                PlaySound(eatSound);
            }
        }
        void checkCollisionWithWall(){
            if(snake.body[0].x == -1 || snake.body[0].x == cellCount || snake.body[0].y == -1 || snake.body[0].y == cellCount){
                Gameover();
            }
        }
        void checkCollisionWithSelf(){
            deque<Vector2> headLessBody = snake.body;
            headLessBody.pop_front();
            if(elementInDeque(snake.body[0],headLessBody)){
                Gameover();
            }
        }
        void Gameover(){
            snake.reset();
            food.position = food.generateRandomPosition(snake.body);
            running = false;
            score = 0;
            PlaySound(wallSound);
        }
};

int main() {
    InitWindow(2*offset + cellSize*cellCount,2*offset + cellSize*cellCount,"Snake Game");
    SetTargetFPS(60);
    Game game = Game();
    while(WindowShouldClose() == false){
        BeginDrawing();
        if(eventTriggered(0.2)){
            game.update();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0,-1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0,1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1,0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1,0};
            game.running = true;
        }
        ClearBackground(bgColor);
        DrawRectangleLinesEx(Rectangle{(float)offset-10,(float)offset-10,(float)cellSize*cellCount+20,(float)cellSize*cellCount+20}, 5, snakeColor);
        DrawText("Snake Game",offset,offset - 50,40,snakeColor);
        DrawText(TextFormat("%i",game.score),offset,offset + cellSize*cellCount + 10,40,snakeColor);
        game.draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}