/*
Compilation Commands (In Order):

    1) g++ -c main.cpp
    2) g++ main.o -o main -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

Running The Game:

    3) ./main

One line command for all :
    g++ -c main.cpp;
    g++ main.o -o main -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system;
    ./main;

g++ main.cpp -o main -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system;./main

*/

#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

using namespace std;
using namespace sf;

Clock frameClock;

const Vector2f screenResolution = {1000, 600};

Font font;
Text _text;

string playerName = " ";

// Class Declarations :
class Object
{
public:
    Texture texture;
    Sprite sprite;

    // Load Image -> Set Sprite -> SetScale -> SetPosition
    Object(string path = "fireBall.png", Vector2f position = {-1000, -1000}, Vector2f scale = {0, 0})
    {

        if (!texture.loadFromFile("images/" + path))
        {
            cout << "Error loading image Path : images/" << path << endl;
            exit(EXIT_FAILURE);
        }

        sprite.setTexture(texture);
        sprite.setScale(scale);
        sprite.setPosition(position);
    }

    // Render On Screen
    void draw(RenderWindow &window)
    {
        window.draw(sprite);
    }

    // Change Positon
    void changePosition(Vector2f newPosition)
    {
        sprite.setPosition(newPosition);
    }

    // Change Scale
    void changeScale(Vector2f newScale)
    {
        sprite.setScale(newScale);
    }

    // Check Collision click with Object
    bool checkCollision(Vector2i mousePosition)
    {
        return sprite.getGlobalBounds().contains(mousePosition.x, mousePosition.y);
    }

    FloatRect getGlobalBond() const
    {
        return sprite.getGlobalBounds();
    }

    void moveObject(sf::Vector2f velocity, float deltaTime, sf::Vector2f windowSize)
    {
        // Move the object
        sprite.move(velocity.x * deltaTime, velocity.y * deltaTime);

        // Ensure the object stays inside the window
        if (sprite.getPosition().x < 0)
        {
            sprite.setPosition(0, sprite.getPosition().y);
        }
        else if (sprite.getPosition().x + sprite.getGlobalBounds().width > windowSize.x)
        {
            sprite.setPosition(windowSize.x - sprite.getGlobalBounds().width, sprite.getPosition().y);
        }

        if (sprite.getPosition().y < 0)
        {
            sprite.setPosition(sprite.getPosition().x, 0);
        }
        else if (sprite.getPosition().y + sprite.getGlobalBounds().height > windowSize.y)
        {
            sprite.setPosition(sprite.getPosition().x, windowSize.y - sprite.getGlobalBounds().height);
        }
    }

    // Set IntRect for sprite
    void setIntRect(float left, float top, float width, float height)
    {
        sprite.setTextureRect(IntRect(left, top, width, height));
    }

    // Get the height of the object
    float getHeight() const
    {
        return sprite.getGlobalBounds().height;
    }
    // Get the left position of the object
    float getLeft() const
    {
        return sprite.getGlobalBounds().left;
    }

    // Get the right position of the object
    float getRight() const
    {
        return sprite.getGlobalBounds().left + sprite.getGlobalBounds().width;
    }

    // Get the top position of the object
    float getTop() const
    {
        return sprite.getGlobalBounds().top;
    }

    // Get the bottom position of the object
    float getBottom() const
    {
        return sprite.getGlobalBounds().top + sprite.getGlobalBounds().height;
    }
    // Get the width of the object
    float getWidth() const
    {
        return sprite.getGlobalBounds().width;
    }

    // Change the image of the object
    void changeImage(string newPath)
    {
        if (!texture.loadFromFile("images/" + newPath))
        {
            cout << "Error loading image Path : images/" << newPath << endl;
            exit(EXIT_FAILURE);
        }
        sprite.setTexture(texture);
    }
};

class fireBall : public Object
{
public:
    float velocity = 50.f;
    bool isDestroyed;
    Clock spawnClock;
    bool hasStartedFalling = false;
    Vector2f worldPosition; // add this public member to track actual world position

    fireBall(string path = "fireBall.png", Vector2f position = {-1000, -1000}, Vector2f scale = {.1f, .1f})
    {
        isDestroyed = true;
        hasStartedFalling = false;

        if (!texture.loadFromFile("images/" + path))
        {
            cout << "Error loading image Path : images/" << path << endl;
            exit(EXIT_FAILURE);
        }

        sprite.setTexture(texture);
        sprite.setScale(scale);
        sprite.setPosition(position);
        sprite.setTextureRect({0, 0, 300, 300});
    }

    void distroy()
    {
        isDestroyed = true;
        hasStartedFalling = false;
    }
};

class player : public Object
{
public:
    int hp;
    int ringsCollected;
    int score;
    string name;

    // Constructor
    player(string path, float startX, float startY, float scaleX, float scaleY)
        : Object(path, {startX, startY}, {scaleX, scaleY}),
          hp(140), ringsCollected(0), score(0)

    {
        sprite.setPosition(startX, startY);
        sprite.setScale(scaleX, scaleY);

        name = playerName;
    }

    void takeDamage(int amount)
    {
        hp -= amount;
        if (hp < 0)
            hp = 0;
    }

    void addRings(int amount = 1)
    {
        ringsCollected += amount;
        score += amount * 10;
    }
 


    int getRingsCollected() const
    {
        return ringsCollected;
    }

    int getScore() const
    {
        return score;
    }
};

class Sonic : public player
{

public:
    string state; // "left", "right", "jump", "fly_left", "fly_right"
    int frame;
    float sonicAnimTimer = 0.0f;
    const int maxFrames = 7;

    Sonic(float startX, float startY, float scaleX = 1.28f, float scaleY = 1.28f)
        : player("k_jump_right.png", startX, startY, scaleX, scaleY),
          state("right"), frame(0)
    {
        setIntRect(0 * 40, 0, 40, 40);
    }

    void update(float deltaTime, float &velocityX, bool &isJumping)
    {

        if (velocityX != 0 && !isJumping)
        {
            sonicAnimTimer += deltaTime;
            if (sonicAnimTimer >= 0.04f)
            {
                sonicAnimTimer = 0.0f;
                frame = (frame + 1) % maxFrames;

                // Run animation (row 0)
                changeScale({1.28f, 1.28f});
                setIntRect(frame * 40, 0, 40, 40);
            }
        }
        else if (velocityX != 0 && isJumping)
        {
            sonicAnimTimer += deltaTime;
            if (sonicAnimTimer >= 0.04)
            {
                sonicAnimTimer = 0.0f;
                frame = (frame + 1) % maxFrames;

                // Jumping animation (row 1 — assuming jump frames are in second row)
                changeScale({1.f, 1.f});

                setIntRect(frame * 40, 0, 40, 40);
            }
        }
        else if (isJumping)
        {
            // Not moving left/right, but in air → show first jump frame
            changeScale({1.f, 1.f});

            setIntRect(0, 0, 40, 40);
        }
        else
        {
            // Idle animation
            frame = 0;
            changeScale({1.28f, 1.28f});
            setIntRect(0, 0, 40, 40);
        }
    }
};

class Tales : public player
{
public:
    string state; // "left", "right", "jump", "fly_left", "fly_right"
    int frame;
    Clock animationClock;
    bool flyactivated = false;

    Tales(float startX, float startY, float scaleX = 1.0f, float scaleY = 1.0f)
        : player("tales_right.png", startX, startY, scaleX, scaleY),
          state("right"), frame(0)
    {
        setIntRect(0 * 45, 0, 40, 35);
    }


    void update(float deltaTime)
    {
        if (animationClock.getElapsedTime().asMilliseconds() > 80)
        {
            if (state == "left")
            {
                changeScale({1.5f, 1.5f});
                changeImage("tales_left.png");
                switch (frame)
                {
                case 8:
                    setIntRect(0 * 45, 0, 45, 35);
                    break;
                case 7:
                    setIntRect(1 * 45, 0, 45, 35);
                    break;
                case 6:
                    setIntRect(2 * 45, 0, 45, 35);
                    break;
                case 5:
                    setIntRect(3 * 45, 0, 45, 35);
                    break;
                case 4:
                    setIntRect(4 * 45 - 5, 0, 45, 35);
                    break;
                case 3:
                    setIntRect(5 * 45 - 5, 0, 45, 35);
                    break;
                case 2:
                    setIntRect(6 * 45 - 5, 0, 45, 35);
                    break;
                case 1:
                    setIntRect(7 * 45 - 5, 0, 45, 35);
                    break;
                case 0:
                    setIntRect(8 * 45 - 10, 0, 45, 35);
                    break;
                }
                frame++;
                if (frame >= 9)
                    frame = 0;
            }

            else if (state == "right")
            {
                changeScale({1.5f, 1.5f});
                changeImage("tales_right.png");
                switch (frame)
                {
                case 0:
                    setIntRect(0 * 45, 0, 45, 35);
                    break;
                case 1:
                    setIntRect(1 * 45, 0, 45, 35);
                    break;
                case 2:
                    setIntRect(2 * 45, 0, 45, 35);
                    break;
                case 3:
                    setIntRect(3 * 45 - 10, 0, 45, 35);
                    break;
                case 4:
                    setIntRect(4 * 45 - 5, 0, 45, 35);
                    break;
                case 5:
                    setIntRect(5 * 45 - 10, 0, 45, 35);
                    break;
                case 6:
                    setIntRect(6 * 45 - 10, 0, 45, 35);
                    break;
                case 7:
                    setIntRect(7 * 45 - 10, 0, 45, 35);
                    break;
                case 8:
                    setIntRect(8 * 45 - 13, 0, 45, 35);
                    break;
                }
                frame++;
                if (frame >= 9)
                    frame = 0;
            }

            else if (state == "jump")
            {
                changeImage("jumpTales.png");
                changeScale({1.2f, 1.2f});
                switch (frame)
                {
                case 0:
                    setIntRect(0 * 45, 0, 40, 35);
                    break;
                case 1:
                    setIntRect(1 * 45 - 2, 0, 40, 35);
                    break;
                case 2:
                    setIntRect(2 * 45 - 5, 0, 40, 35);
                    break;
                case 3:
                    setIntRect(3 * 45 - 5, 0, 40, 35);
                    break;
                case 4:
                    setIntRect(4 * 45 - 7, 0, 40, 35);
                    break;
                case 5:
                    setIntRect(5 * 45 - 10, 0, 40, 35);
                    break;
                }
                frame++;
                if (frame >= 6)
                    frame = 0;
            }
            else if (state == "fly_left")
            {
                changeImage("tales_fly_left.png");
                setIntRect(0, 0, 32, 32);
            }
            else if (state == "fly_right")
            {
                changeImage("tales_fly_right.png");
                setIntRect(0, 0, 32, 32);
            }

            animationClock.restart();
        }

        // moveObject({velocityX, velocityY}, deltaTime, {1000, 600});
    }
};

class Knuckles : public player
{
public:
    string state; // "right_run", "left_run", "jump_right", etc.
    int frame;
    Clock animationClock;
    float frameDuration;
    int totalFrames;

    Knuckles(float startX, float startY, float scaleX = 1.0f, float scaleY = 1.0f)
        : player("k_still_right.png", startX, startY, scaleX, scaleY),
          state("right_still"), frame(0), frameDuration(0.1f), totalFrames(9)
    {
        setIntRect(0, 0, 45, 35); // Initial frame
    }

    void update(float deltaTime)
    {

        if (animationClock.getElapsedTime().asSeconds() > frameDuration)
        {
            if (state == "right")
            {
                frame = (frame + 1) % 6;

                switch (frame)
                {
                case 0:
                    setIntRect(0, 5, 35, 30);
                    break;
                case 1:
                    setIntRect(36, 5, 30, 30);
                    break;
                case 2:
                    setIntRect(72, 5, 30, 30);
                    break;
                case 3:
                    setIntRect(100, 5, 35, 30);
                    break;
                case 4:
                    setIntRect(140, 5, 36, 30);
                    break;
                case 5:
                    setIntRect(174, 5, 36, 30);
                    break;
                }
            }
            else if (state == "left")
            {
                frame = (frame + 1) % 6;

                switch (frame)
                {
                case 5:
                    setIntRect(0, 5, 42, 30);
                    break;
                case 4:
                    setIntRect(36, 5, 42, 30);
                    break;
                case 3:
                    setIntRect(72, 5, 42, 30);
                    break;
                case 2:
                    setIntRect(100, 5, 42, 30);
                    break;
                case 1:
                    setIntRect(140, 5, 36, 30);
                    break;
                case 0:
                    setIntRect(174, 5, 36, 30);
                    break;
                }
            }
            else if (state == "jump_right")
            {
                frame = (frame + 1) % 7;

                switch (frame)
                {
                case 0:
                    setIntRect(0 * 60, 0, 60, 80);
                    break;
                case 1:
                    setIntRect(1 * 60, 0, 60, 80);
                    break;
                case 2:
                    setIntRect(2 * 60, 0, 60, 80);
                    break;
                case 3:
                    setIntRect(3 * 60, 0, 60, 80);
                    break;
                case 4:
                    setIntRect(4 * 60 + 4, 0, 60, 80);
                    break;
                case 5:
                    setIntRect(5 * 60 + 5, 0, 60, 80);
                    break;
                case 6:
                    setIntRect(2 * 60, 0, 60, 80);
                    break;
                }
            }
            else if (state == "jump_left")
            {
                frame = (frame + 1) % 7;

                switch (frame)
                {
                case 0:
                    setIntRect(0 * 60, 0, 60, 80);
                    break;
                case 1:
                    setIntRect(1 * 60, 0, 60, 80);
                    break;
                case 2:
                    setIntRect(2 * 60, 0, 60, 80);
                    break;
                case 3:
                    setIntRect(3 * 60, 0, 60, 80);
                    break;
                case 4:
                    setIntRect(4 * 60 + 4, 0, 60, 80);
                    break;
                case 5:
                    setIntRect(5 * 60 + 5, 0, 60, 80);
                    break;
                case 6:
                    setIntRect(2 * 60, 0, 60, 80);
                    break;
                }
            }
            else if (state == "punch_right")
            {
                changeScale({1.5f, 1.5f});
                changeImage("k_punch_right.png");

                frame = (frame + 1) % 4;

                switch (frame)
                {
                case 0:
                    setIntRect(0, 0, 70, 80);
                    break;
                case 1:
                    setIntRect(70, 0, 70, 80);
                    break;
                case 2:
                    setIntRect(140, 0, 70, 80);
                    break;
                case 3:
                    setIntRect(210, 0, 70, 80);
                    break;
                }
            }
            else if (state == "punch_left")
            {
                changeScale({1.5f, 1.5f});
                changeImage("k_punch_left.png");

                frame = (frame + 1) % 4;

                switch (frame)
                {
                case 3:
                    setIntRect(0, 0, 50, 80);
                    break;
                case 2:
                    setIntRect(60, 0, 55, 80);
                    break;
                case 1:
                    setIntRect(120, 0, 65, 80);
                    break;
                case 0:
                    setIntRect(210, 0, 70, 80);
                    break;
                }
            }

            animationClock.restart();
        }
    }
};

class Ring : public Object
{
public:
    bool isCollected;

    Ring(string path, float startX, float startY, float scaleX, float scaleY)
        : Object(path, {startX, startY}, {scaleX, scaleY})
    {
        isCollected = false;
    }

    void collect()
    {
        isCollected = true;
    }

};

class Obstacle : public Object
{
public:
    bool isActive;
    Obstacle(string path, float startX, float startY, float scaleX, float scaleY) : Object(path, {startX, startY}, {scaleX, scaleY})
    {
        isActive = true;
    }
};

class Button
{
public:
    RectangleShape rect;
    Text text;

    Button(string str, Vector2f position, Color color = Color::Blue, int characterSize = 30)
    {
        // Set up the text
        text.setString(str);
        text.setFont(font);
        text.setCharacterSize(characterSize);
        text.setFillColor(Color::Yellow);

        // Set up the rectangle
        rect.setSize(Vector2f(text.getGlobalBounds().width + 20, 50)); // Add some padding
        rect.setFillColor(color);
        rect.setPosition(position);

        // Center the text on the rectangle
        text.setPosition(position.x + (rect.getSize().x - text.getGlobalBounds().width) / 2, position.y + (rect.getSize().y - text.getGlobalBounds().height) / 2 - 15);
    }

    void draw(RenderWindow &window)
    {
        window.draw(rect);
        window.draw(text);
    }

    bool isClicked(Vector2i mousePosition)
    {
        return rect.getGlobalBounds().contains(mousePosition.x, mousePosition.y);
    }

    void ChangeColor()
    {

        if (rect.getFillColor() == Color::Blue)
        {
            rect.setFillColor(Color(255, 69, 0));
            text.setFillColor(Color::Black);
        }
        else
        {
            rect.setFillColor(Color::Blue);
            text.setFillColor(Color::Yellow);
        }
    }

    Color getColor() const
    {
        return rect.getFillColor();
    }

    void setPosition(Vector2f position)
    {
        rect.setPosition(position);
        text.setPosition(position.x + (rect.getSize().x - text.getGlobalBounds().width) / 2, position.y + (rect.getSize().y - text.getGlobalBounds().height) / 2 - 15);
    }

    void setRandom()
    {
        setPosition({static_cast<float>(rand() % int(screenResolution.x - rect.getSize().x)), static_cast<float>(rand() % int(screenResolution.y - rect.getSize().y))});
    }
};

class enemy : public Object
{
public:
    int hp;
    bool isAlive;
    int frameCount;

    int currentFrame;
    float frameTime;
    float elapsed;

    float x, y;

    bool moveRight = true;

    float movedDistance = 0;

    // Constructor with frameCount and animation setup
    enemy(
        string path = "motoBug.png",
        float startX = 0.0f,
        float startY = 0.0f,
        float scaleX = 1.0f,
        float scaleY = 1.0f,
        int frameCount = 1,
        int frameWidth = 50,
        int frameHeight = 50,
        float frameDuration = 0.15f)
        : Object(path, {startX, startY}, {scaleX, scaleY}),
          hp(100), isAlive(true),
          frameCount(frameCount), currentFrame(0),
          frameTime(frameDuration), elapsed(0.0f)
    {
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
    }
};

class e4_type : public enemy
{
public:
    Clock fireballCooldown;
    bool canFire = true;

    // Constructor for e4_type
    e4_type(
        string path = "motoBug.png",
        float startX = 0.0f,
        float startY = 0.0f,
        float scaleX = .5f,
        float scaleY = .5f,
        int frameCount = 1,
        int frameWidth = 50,
        int frameHeight = 50,
        float frameDuration = 0.15f)
        : enemy(path, startX, startY, scaleX, scaleY,
                frameCount, frameWidth, frameHeight, frameDuration)
    {
        x = startX;
        y = startY;
        movedDistance = 0;
        moveRight = true;
        canFire = true;
    }
};

class Hitbox : public Object
{
public:
    Hitbox(string path, float startX, float startY, float scaleX, float scaleY)
        : Object(path, {startX, startY}, {scaleX, scaleY}) {}

    void copyCordinates(const Sonic &player)
    {
        float newX = player.getLeft() + player.getWidth() * 0.1f;
        float newY = player.getTop() + player.getHeight() * 0.1f;

        float newScaleX = 1.28f;
        float newScaleY = 1.28f;

        this->changePosition({newX + 4 + 4, newY});
        this->changeScale({newScaleX, newScaleY});
    }
    void copyCordinates(const Sonic &player, int a)
    {
        float newX = player.getLeft() + player.getWidth() * 0.1f;
        float newY = player.getTop() + player.getHeight() * 0.1f;

        float newScaleX = 1.7f;
        float newScaleY = 3.5f;

        this->changePosition({newX - 20, newY});
        this->changeScale({newScaleX, newScaleY});
    }
};

class LeaderBoards
{
public:
    int position;
    string name;
    int score;
    LeaderBoards()
        : position(-1), name("unknown"), score(-1) {}
    // Constructor
    LeaderBoards(int pos, const string &playerName, int playerScore)
        : position(pos), name(playerName), score(playerScore) {}

    // Method to display the leaderboard entry
    void display() const
    {
        cout << "Position: " << position << ", Name: " << name << ", Score: " << score << endl;
    }

    // Method to update the score
    void updateScore(int newScore)
    {
        score = newScore;
    }

    // Method to compare scores (useful for sorting)
    bool operator<(const LeaderBoards &other) const
    {
        return score < other.score;
    }

    // Method to reset the leaderboard entry
    void reset()
    {
        position = 0;
        name = "";
        score = 0;
    }
};

// Global Varibales :
bool menu = true;
bool showLeaderBoard = false;
bool gotoOptions = false;
bool continueGame = false;
bool askingName = false;
bool levelSelection = true;

// Static Functions :
static void setFont(string path);

// Functions :
void displayText(string text, Vector2f position, RenderWindow &window, int characterSize = 30, Color color = Color::Black);
void fileRead(LeaderBoards *&ranking, int &totalRanks);
void LeaderBoardWorkingScreen(RenderWindow &window, Event &event, Object &leaderBoardBackground, Button &Back, LeaderBoards *ranking, int &ranks);
void menuWorking(RenderWindow &window, Event &event, Button &NewGame, Button &Options, Button &Continue, Button &LeaderBoard, Button &exitButton, Object &sonicLogo, Object &menuBackground, LeaderBoards *&ranking);
void optionsWorking(RenderWindow &window, Event &event, Object &leaderBoardBackground, Button &volumeSlow, Button &volumeMedium, Button &volumeFast, Button &Back, Music &);
void continueWorking(RenderWindow &window, Event &event, Object &leaderBoardBackground, Button &Back);
void LoadSound(Music &backgroundMusic, string path, bool loops = true);
void changeVolume(Music &backgroundMusic, float volume);
void askingUsername(RenderWindow &window, Event &event, string &username, LeaderBoards *ranking, int &ranks);
void appendToFile(const string &fileName, const string &username, int score, int &totalRanks);
void levelSelectionScreen(RenderWindow &window, Event &event, Object &background, Button &Back);
void levelOne(RenderWindow &window, Event &event);
void levelTwo(RenderWindow &window, Event &event);
void levelThree(RenderWindow &window, Event &event);

int main()
{
    // Planting a seed for Randomization
    srand(static_cast<unsigned int>(time(0)));

    // Load Font For (All Dispay Button Use) ;
    setFont("Jersey.ttf");

    // Creating a Window of the Resolution (x,y) --- Name of RendeBlue Window
    RenderWindow window(VideoMode(screenResolution.x, screenResolution.y), " CS PROJECT (TALHA SAMI) ");

    // For handling the Keyboard and Mouse Input function and Mechanics of the Game
    Event event;

    // Movement SetUp
    Clock clock;
    Vector2f velocity = Vector2f(0, 0);
    float speed = 10.0f;

    // Music Setup
    Music backgroundMusic;
    LoadSound(backgroundMusic, "labrynth.ogg");

    // Game Setup
    Button NewGame("NEW GAME", Vector2f(445, 260));
    NewGame.ChangeColor();

    Button Options("OPTIONS", Vector2f(460, 315));
    Button Continue("CONTINUE", Vector2f(455, 370));

    Continue.ChangeColor();
    Button LeaderBoard("LEADERBOARD", Vector2f(435, 425));
    Button exitButton(" EXIT ", Vector2f(470, 480));
    exitButton.ChangeColor();

    Button Back(" <- BACK  ", Vector2f(200, 500));
    Button volumeSlow(" SLOW ", Vector2f(675, 170));
    Button volumeMedium(" MEDIUM ", Vector2f(665, 230));
    Button volumeFast(" FAST ", Vector2f(675, 290));

    Object sonicLogo("sonicLogo.png", Vector2f(360, 90), Vector2f(0.5f, 0.5f));
    Object menuBackground("menuBackground.jpg", Vector2f(0, 0), Vector2f(0.75f, 0.78f));
    Object rankBackground("pixelBackground.jpg", Vector2f(-300, 0), Vector2f(0.97f, 0.78f));

    LeaderBoards *ranking = nullptr;
    int ranks = 0;

    string newUserName = "";

    fileRead(ranking, ranks);
    volumeMedium.ChangeColor();

    // Every thing inside this While Loop would work Till the Rendering Window isn't closed
    while (window.isOpen())
    {
        if (menu)
        {
            menuWorking(window, event, NewGame, Options, Continue, LeaderBoard, exitButton, sonicLogo, menuBackground, ranking);
        }
        else if (askingName)
        {
            askingUsername(window, event, newUserName, ranking, ranks);
        }
        else if (showLeaderBoard)
        {
            LeaderBoardWorkingScreen(window, event, rankBackground, Back, ranking, ranks);
        }
        else if (gotoOptions)
        {
            optionsWorking(window, event, rankBackground, volumeSlow, volumeMedium, volumeFast, Back, backgroundMusic);
        }
        else if (continueGame)
        {
            continueWorking(window, event, rankBackground, Back);
        }
        else if (levelSelection)
        {
            levelSelectionScreen(window, event, rankBackground, Back);
        }
    }

    return 0;
}

// Load Font Once for whole
static void setFont(string path)
{

    if (!font.loadFromFile("fonts/" + path))
    {
        cout << "Error loading Font file Path : fonts/" << path << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << " Fonts Loaded Successfully Start Working" << endl;
    }

    return;
}

// Function to Display String On Screen on Runtime
void displayText(string text, Vector2f position, RenderWindow &window, int characterSize, Color color)
{

    // Initalize _text -> setFont -> setCharacterSize -> setFillColor -> setPosition

    _text.setString(text);
    _text.setFont(font);
    _text.setCharacterSize(characterSize);
    _text.setFillColor(color);
    _text.setPosition(position);

    // Render On Screen
    window.draw(_text);
}

void LeaderBoardWorkingScreen(RenderWindow &window, Event &event, Object &leaderBoardBackground, Button &Back, LeaderBoards *ranking, int &ranks)
{
    // This While loop handle all the Inputs of ( Keyboard --- Mouse --- Screen Buttons )

    while (window.pollEvent(event))
    {
        // Close Button on Screen Handling
        if (event.type == Event::Closed)
            window.close();

        // Mechanics What to Happen when Mouse Pressed
        if (event.type == Event::MouseButtonPressed)
        {
            // Mouse Pixels Input Where Clicked
            Vector2i mousePosition = Mouse::getPosition(window);
            cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

            if (event.mouseButton.button == Mouse::Left)
            {
                cout << " Left Mouse Button Pressed " << endl;
                if (Back.isClicked(mousePosition))
                {
                    // Back.ChangeColor();
                    showLeaderBoard = false;
                    menu = true;
                }
            }
        }
    }

    window.clear(Color::White);
    leaderBoardBackground.draw(window);
    Back.draw(window);

    displayText("NOTE : Would update when game restarts", Vector2f(100, 40), window, 30, Color::Black);
    // Display "LeaderBoard" title in a different color
    displayText("LeaderBoard", Vector2f(635, 100), window, 50, Color::Red);

    // Display the top 7 ranks or a message if no ranks are available
    if (ranking != nullptr && ranks > 0)
    {
        for (int i = 0; i < min(ranks, 7); ++i)
        {
            string rankInfo = to_string(i + 1) + ". " + ranking[i].name + " - " + to_string(ranking[i].score);
            displayText(rankInfo, Vector2f(635, 170 + i * 40), window, 40);
        }
    }
    else
    {
        displayText("No ranks available", Vector2f(635, 200), window, 40);
    }
    /*
    Display Starts making things On Screen in Backhand
    and when window.display() is called all the Textures
    comes up in the order they are setted
    */
    window.display();
}

void menuWorking(RenderWindow &window, Event &event, Button &NewGame, Button &Options, Button &Continue, Button &LeaderBoard, Button &exitButton, Object &sonicLogo, Object &menuBackground, LeaderBoards *&ranking)
{
    // This While loop handle all the Inputs of ( Keyboard --- Mouse --- Screen Buttons )
    while (window.pollEvent(event))
    {
        // Close Button on Screen Handling
        if (event.type == Event::Closed)
            window.close();

        // Mechanics What to Happen when Mouse Pressed
        if (event.type == Event::MouseButtonPressed)
        {
            // Mouse Pixels Input Where Clicked
            Vector2i mousePosition = Mouse::getPosition(window);
            cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

            if (exitButton.isClicked(mousePosition))
            {
                exit(EXIT_SUCCESS);
            }

            if (event.mouseButton.button == Mouse::Left)
            {
                cout << " Left Mouse Button Pressed " << endl;

                if (NewGame.isClicked(mousePosition))
                {
                    // NewGame.ChangeColor();
                    cout << " New Game Clicked " << endl;
                    menu = false;
                    askingName = true;
                }
                else if (Options.isClicked(mousePosition))
                {
                    // Options.ChangeColor();
                    cout << " Options Clicked " << endl;
                    menu = false;
                    gotoOptions = true;
                }
                else if (Continue.isClicked(mousePosition))
                {
                    // Continue.ChangeColor();
                    cout << " Continue Clicked " << endl;
                    menu = false;
                    continueGame = true;
                }
                else if (LeaderBoard.isClicked(mousePosition))
                {
                    // LeaderBoard.ChangeColor();
                    cout << " Leader Board Clicked " << endl;
                    menu = false;
                    showLeaderBoard = true;
                }
            }
        }
    }

    window.clear(Color::Blue);
    // Clears all the Previous Screen and Render the Screen
    menuBackground.draw(window);

    sonicLogo.draw(window);
    NewGame.draw(window);
    Options.draw(window);
    Continue.draw(window);
    LeaderBoard.draw(window);
    exitButton.draw(window);

    /*
    Display Starts making things On Screen in Backhand
    and when window.display() is called all the Textures
    comes up in the order they are setted
    */
    window.display();
}

void optionsWorking(RenderWindow &window, Event &event, Object &leaderBoardBackground, Button &volumeSlow, Button &volumeMedium, Button &volumeFast, Button &Back, Music &musicBackground)
{
    // This While loop handle all the Inputs of ( Keyboard --- Mouse --- Screen Buttons )

    while (window.pollEvent(event))
    {
        // Close Button on Screen Handling
        if (event.type == Event::Closed)
            window.close();

        // Mechanics What to Happen when Mouse Pressed
        if (event.type == Event::MouseButtonPressed)
        {
            // Mouse Pixels Input Where Clicked
            Vector2i mousePosition = Mouse::getPosition(window);
            cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

            if (event.mouseButton.button == Mouse::Left)
            {
                cout << " Left Mouse Button Pressed " << endl;
                if (Back.isClicked(mousePosition))
                {
                    // Back.ChangeColor();
                    menu = true;
                    gotoOptions = false;
                }
                else if (volumeSlow.isClicked(mousePosition))
                {
                    changeVolume(musicBackground, 15.0f);
                    if (volumeSlow.getColor() == Color::Blue)
                    {
                        volumeSlow.ChangeColor();
                    }
                    if (volumeMedium.getColor() != Color::Blue)
                    {
                        volumeMedium.ChangeColor();
                    }
                    if (volumeFast.getColor() != Color::Blue)
                    {
                        volumeFast.ChangeColor();
                    }
                }
                else if (volumeMedium.isClicked(mousePosition))
                {
                    changeVolume(musicBackground, 30.0f);
                    if (volumeMedium.getColor() == Color::Blue)
                    {
                        volumeMedium.ChangeColor();
                    }
                    if (volumeSlow.getColor() != Color::Blue)
                    {
                        volumeSlow.ChangeColor();
                    }
                    if (volumeFast.getColor() != Color::Blue)
                    {
                        volumeFast.ChangeColor();
                    }
                }
                else if (volumeFast.isClicked(mousePosition))
                {
                    changeVolume(musicBackground, 60.0f);
                    if (volumeFast.getColor() == Color::Blue)
                    {
                        volumeFast.ChangeColor();
                    }
                    if (volumeSlow.getColor() != Color::Blue)
                    {
                        volumeSlow.ChangeColor();
                    }
                    if (volumeMedium.getColor() != Color::Blue)
                    {
                        volumeMedium.ChangeColor();
                    }
                }
            }
        }
    }

    window.clear(Color::White);
    leaderBoardBackground.draw(window);
    Back.draw(window);

    // Display "LeaderBoard" title in a different color
    displayText("SOUND VOLUME", Vector2f(605, 100), window, 50, Color::Blue);

    volumeSlow.draw(window);
    volumeFast.draw(window);
    volumeMedium.draw(window);

    /*
    Display Starts making things On Screen in Backhand
    and when window.display() is called all the Textures
    comes up in the order they are setted
    */
    window.display();
}

void continueWorking(RenderWindow &window, Event &event, Object &leaderBoardBackground, Button &Back)
{
    while (window.isOpen() && continueGame)
    {
        while (window.pollEvent(event))
        {
            // Close Button on Screen Handling
            if (event.type == Event::Closed)
                window.close();

            // Mechanics What to Happen when Mouse Pressed
            if (event.type == Event::MouseButtonPressed)
            {
                // Mouse Pixels Input Where Clicked
                Vector2i mousePosition = Mouse::getPosition(window);
                cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

                if (event.mouseButton.button == Mouse::Left)
                {
                    cout << " Left Mouse Button Pressed " << endl;
                    if (Back.isClicked(mousePosition))
                    {
                        // Back.ChangeColor();
                        menu = true;
                        continueGame = false;
                    }
                }
            }
        }

        window.clear(Color::White);
        leaderBoardBackground.draw(window);
        Back.draw(window);

        displayText("   NOTE : ", Vector2f(600, 200), window, 45, Color::Red);
        displayText("   Currently Not", Vector2f(600, 245), window, 45, Color::Red);
        displayText("    Working", Vector2f(600, 290), window, 45, Color::Red);
        /*
        Display Starts making things On Screen in Backhand
        and when window.display() is called all the Textures
        comes up in the order they are setted
        */
        window.display();
    }
}

void LoadSound(Music &backgroundMusic, string path, bool loops)
{
    // Load background music

    if (!backgroundMusic.openFromFile("audio/" + path))
    {
        cout << "Error loading background music file." << endl;
        exit(EXIT_FAILURE);
    }
    backgroundMusic.setVolume(30.0f);
    backgroundMusic.setLoop(loops);
    if (loops)
    {
        backgroundMusic.play();
    }
}

void changeVolume(Music &backgroundMusic, float volume)
{
    backgroundMusic.setVolume(volume);
}

void askingUsername(RenderWindow &window, Event &event, string &username, LeaderBoards *ranking, int &ranks)
{
    username = ""; // Initialize username
    Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(30);
    inputText.setFillColor(Color::Yellow);
    inputText.setPosition(400 + 200 + 50, 300);

    Text promptText;
    promptText.setFont(font);
    promptText.setCharacterSize(40);
    promptText.setFillColor(Color::Blue);
    promptText.setString("   Enter your username :");
    promptText.setPosition(200 + 200 + 100, 200);

    Object backgroundPhoto("playerUser.png", {-170, 10}, {1.5f, 1.5f});

    while (window.isOpen() && askingName)
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::TextEntered)
            {
                if (event.text.unicode == '\b')
                { // Handle backspace
                    if (!username.empty())
                    {
                        username.pop_back();
                    }
                }
                else if (event.text.unicode == '\r')
                { // Handle enter
                    if (!username.empty())
                    {
                        cout << "Entered Username: " << username << endl;
                        appendToFile("leaderBoard.txt", username, 0, ranks); // Append username with initial score 0
                        askingName = false;
                        levelSelection = true;
                        playerName = username;
                    }
                }
                else if (username.size() < 20 && event.text.unicode < 128)
                { // Limit to 20 characters
                    username += static_cast<char>(event.text.unicode);
                }
            }
        }

        inputText.setString(username + "_"); // Add cursor effect
        window.clear(Color::Black);
        backgroundPhoto.draw(window);
        displayText("NOTE: Username will be used for leaderboard tracking.", Vector2f(200 + 200 + 100 + 50, 150), window, 15, Color::Red);
        window.draw(promptText);
        window.draw(inputText);
        window.display();
    }
}

void levelSelectionScreen(RenderWindow &window, Event &event, Object &background, Button &Back)
{
    Button level1("LEVEL 1", Vector2f(675, 170));
    Button level2("LEVEL 2", Vector2f(675, 230));
    Button level3("LEVEL 3", Vector2f(675, 290));
    Button bossLevel("BOSS LEVEL", Vector2f(650, 350));
    bossLevel.ChangeColor();

    while (window.isOpen() && levelSelection)
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mousePosition = Mouse::getPosition(window);

                if (event.mouseButton.button == Mouse::Left)
                {
                    if (Back.isClicked(mousePosition))
                    {
                        menu = true;
                        levelSelection = false;
                    }
                    else if (level1.isClicked(mousePosition))
                    {
                        cout << "Level 1 Selected" << endl;

                        levelOne(window, event);
                    }
                    else if (level2.isClicked(mousePosition))
                    {
                        cout << "Level 2 Selected" << endl;
                        levelTwo(window, event);
                    }
                    else if (level3.isClicked(mousePosition))
                    {
                        cout << "Level 3 Selected" << endl;
                        levelThree(window, event);
                        // Add logic for Level 3
                    }
                    else if (bossLevel.isClicked(mousePosition))
                    {
                        cout << "Boss Level Selected" << endl;
                    }
                }
            }
        }

        window.clear(Color::White);
        background.draw(window);
        Back.draw(window);

        displayText("SELECT LEVEL", Vector2f(605, 100), window, 50, Color::Black);

        level1.draw(window);
        level2.draw(window);
        level3.draw(window);
        bossLevel.draw(window);

        window.display();
    }
}

void fileRead(LeaderBoards *&ranking, int &totalRanks)
{
    ifstream file("leaderBoard.txt");
    if (!file)
    {
        cerr << "Error: Could not open file." << endl;
        return;
    }

    LeaderBoards temp[100]; // Max 100 entries
    int totalInFile = 0;
    int pos, score;
    string name;

    while (file >> pos)
    {
        file.ignore(); // Ignore leftover newline
        getline(file, name);
        file >> score;
        file.ignore(); // Ignore leftover newline

        temp[totalInFile] = LeaderBoards(pos, name, score);
        totalInFile++;
    }

    // Sort descending by score
    for (int i = 0; i < totalInFile - 1; ++i)
    {
        for (int j = 0; j < totalInFile - i - 1; ++j)
        {
            if (temp[j].score < temp[j + 1].score)
            {
                LeaderBoards tempObj = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = tempObj;
            }
        }
    }

    totalRanks = (totalInFile < 7) ? totalInFile : 7;
    ranking = new LeaderBoards[totalRanks];

    for (int i = 0; i < totalRanks; ++i)
    {
        ranking[i] = temp[i];
        ranking[i].position = i + 1;
    }

    cout << "Total ranks loaded: " << totalRanks << endl;
    for (int i = 0; i < totalRanks; ++i)
    {
        cout << "Rank " << ranking[i].position << ": "
             << ranking[i].name << " with score "
             << ranking[i].score << endl;
    }
}

void appendToFile(const string &fileName, const string &username, int score, int &totalRanks)
{
    ifstream inFile(fileName);

    string line;
    while (getline(inFile, line))
    {
        if (line.empty())
            continue;
    }
    inFile.close();

    ofstream file(fileName, ios::app);
    if (!file)
    {
        cerr << "Error: Could not open file for appending." << endl;
        return;
    }

    file << ++totalRanks << endl;
    file << username << endl;
    file << score << endl;

    file.close();
    cout << "Appended to file: " << username << " with score " << score << endl;
}

/*
1 = left_Still
2 = left_run
3 = left_jump
4 = right_Still
5 = right_run
6 = left_jump
*/

void animateRings(float &ringsElapsedTime, float animationSpeed, int &ringFrame, Ring &ring1)
{
    if (ringsElapsedTime >= animationSpeed)
    {
        ringFrame = (ringFrame + 1) % 4;
        ringsElapsedTime = 0.0f;
    }

    switch (ringFrame)
    {
    case 0:
        ring1.setIntRect(0, 0, 19, 18);

        break;
    case 1:
        ring1.setIntRect(17, 0, 19, 18);

        break;
    case 2:
        ring1.setIntRect(30, 0, 19, 18);

        break;
    case 3:
        ring1.setIntRect(45, 0, 19, 18);

        break;
    }
}

int calculateRings(const char lvl[12][200])
{
    int rings = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            if (lvl[i][j] == 'R')
            {
                // cout << "Rings Found at " << i << " " << j << endl;
                rings++;
            }
        }
    }

    return rings;
}

void animateMotoBug(float &elapsedTime, enemy &e)
{
    e.elapsed += elapsedTime;

    if (e.elapsed >= e.frameTime)
    {
        e.currentFrame = (e.currentFrame + 1) % 4; // Assuming 4 frames
        e.elapsed = 0.0f;
    }

    switch (e.currentFrame)
    {
    case 0:
        e.setIntRect(0, 0, 43.5f, 41);
        break;
    case 1:
        e.setIntRect(43.5f, 0, 43.5f, 41);
        break;
    case 2:
        e.setIntRect(87.0f, 0, 43.5f, 41);
        break;
    case 3:
        e.setIntRect(130.5f, 0, 43.5f, 41);
        break;
    }
}

void animateCrabMeat(float &elapsedTime, enemy &e)
{
    e.elapsed += elapsedTime;

    if (e.elapsed >= e.frameTime)
    {
        e.currentFrame = (e.currentFrame + 1) % 9; // 9 frames
        e.elapsed = 0.0f;
    }

    switch (e.currentFrame)
    {
    case 0:
        e.setIntRect(0.f, 0, 54.f, 41);
        break;
    case 1:
        e.setIntRect(54.f, 0, 40.f, 41);
        break;
    case 2:
        e.setIntRect(94.5f, 0, 40.5f, 41);
        break;
    case 3:
        e.setIntRect(129.f, 0, 45.f, 41);
        break;
    case 4:
        e.setIntRect(172.f, 0, 41.f, 41);
        break;
    case 5:
        e.setIntRect(213.f, 0, 33.f, 41);
        break;
    case 6:
        e.setIntRect(246.f, 0, 40.f, 41);
        break;
    case 7:
        e.setIntRect(246.f + 40, 0, 43.5f, 41);
        break;
    case 8:
        e.setIntRect(246.f + 40, 0, 43.5f, 41);
        break;
    }
}

void animateBeeBot(float &elapsedTime, enemy &e)
{
    e.elapsed += elapsedTime;

    if (e.elapsed >= e.frameTime)
    {
        e.currentFrame = (e.currentFrame + 1) % 5; // 5 frames
        e.elapsed = 0.0f;
    }

    switch (e.currentFrame)
    {
    case 0:
        e.setIntRect(5.f, 0, 29.f, 40);
        break;
    case 1:
        e.setIntRect(33, 0, 26.f, 40);
        break;
    case 2:
        e.setIntRect(65.5, 0, 28.f, 40);
        break;
    case 3:
        e.setIntRect(93.5f, 0, 27.f, 40);
        break;
    case 4:
        e.setIntRect(93.5f + 27.f, 0, 27.f, 40);
        break;
    }
}

void animateBatBrain(float &elapsedTime, enemy &e)
{
    e.elapsed += elapsedTime;

    if (e.elapsed >= e.frameTime)
    {
        e.currentFrame = (e.currentFrame + 1) % 5; // 5 frames
        e.elapsed = 0.0f;
    }

    switch (e.currentFrame)
    {
    case 0:
        e.setIntRect(5.f, 0, 65.f, 40);
        break;
    case 1:
        e.setIntRect(77, 0, 65.f, 40);
        break;
    case 2:
        // e.changePosition({e.getLeft() + 10, e.getTop() });
        e.setIntRect(142, 0, 45.f, 40);
        break;
    case 3:
        // e.changePosition({e.getLeft() - 10, e.getTop() });
        e.setIntRect(187.f, 0, 65.f, 40.f);
        break;
    case 4:
        e.setIntRect(187 + 65, 0, 65.f, 40);
        break;
    }
}

void countEnemies(const char screenLayout[][200], int rows, int &e1, int &e2, int &e3, int &e4)
{
    e1 = e2 = e3 = e4 = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            switch (screenLayout[i][j])
            {
            case '1':
                e1++;
                break;
            case '2':
                e2++;
                break;
            case '3':
                e3++;
                break;
            case '4':
                e4++;
                break;
            }
        }
    }
}

void spawnEnemies(char screenLayout[][200], int rows, int cols,
                  enemy testEnemy1[], int e1, int &idx1,
                  enemy testEnemy2[], int e2, int &idx2,
                  enemy testEnemy3[], int e3, int &idx3,
                  e4_type testEnemy4[], int e4, int &idx4)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vector2f position(j * 50, i * 50);
            char tile = screenLayout[i][j];

            if (tile == '1' && idx1 < e1)
            {
                testEnemy1[idx1].changeImage("CrabMeat.png");
                testEnemy1[idx1].changeScale({1.45f, 1.3f});
                testEnemy1[idx1].changePosition(position);
                testEnemy1[idx1].frameCount = 8;
                testEnemy1[idx1].frameTime = 0.15f;
                testEnemy1[idx1].x = position.x;
                testEnemy1[idx1].y = position.y;
                idx1++;
            }
            else if (tile == '2' && idx2 < e2)
            {
                testEnemy2[idx2].changeImage("motoBug.png");
                testEnemy2[idx2].changeScale({1.12f, 1.12f});
                testEnemy2[idx2].changePosition(position);
                testEnemy2[idx2].frameCount = 4;
                testEnemy2[idx2].frameTime = 0.15f;
                testEnemy2[idx2].x = position.x;
                testEnemy2[idx2].y = position.y;
                idx2++;
            }
            else if (tile == '3' && idx3 < e3)
            {
                testEnemy3[idx3].changeImage("beeBot.png");
                testEnemy3[idx3].changeScale({2.f, 2.f});
                testEnemy3[idx3].changePosition(position);
                testEnemy3[idx3].frameCount = 5;
                testEnemy3[idx3].frameTime = 0.15f;
                testEnemy3[idx3].x = position.x;
                testEnemy3[idx3].y = position.y;
                idx3++;
            }
            else if (tile == '4' && idx4 < e4)
            {
                testEnemy4[idx4].changeImage("batBrain.png");
                testEnemy4[idx4].changeScale({1.5f, 1.5f});
                testEnemy4[idx4].changePosition(position);
                testEnemy4[idx4].frameCount = 8;
                testEnemy4[idx4].frameTime = 0.15f;
                testEnemy4[idx4].x = position.x;
                testEnemy4[idx4].y = position.y;
                idx4++;
            }

            if (tile >= '1' && tile <= '4')
            {
                screenLayout[i][j] = ' ';
            }
        }
    }
}


void showTimeOverScreen(RenderWindow &window, Sonic &sonic, float duration)
{
    Clock clock;

    while (window.isOpen() && clock.getElapsedTime().asSeconds() < duration)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();

        displayText("TIME OVER", Vector2f(screenResolution.x / 2.f - 70.f, screenResolution.y / 2.f - 140.f), window, 60, Color::Yellow);
        displayText("Rings: " + to_string(sonic.getRingsCollected()), Vector2f(screenResolution.x / 2.f - 70.f, screenResolution.y / 2.f - 60.f), window, 30, Color::White);
        displayText("Score: " + to_string(sonic.getScore()), Vector2f(screenResolution.x / 2.f - 70.f, screenResolution.y / 2.f - 20.f), window, 30, Color::White);
        displayText("HP: " + to_string(sonic.hp) + " / 100", Vector2f(screenResolution.x / 2.f - 70.f, screenResolution.y / 2.f + 20.f), window, 30, Color::White);

        window.display();
    }
}

void showGameOverScreen(RenderWindow &window, Sonic &sonic, float duration)
{
    Clock clock;

    while (window.isOpen() && clock.getElapsedTime().asSeconds() < duration)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();

        displayText("GAME OVER", Vector2f(screenResolution.x / 2.f - 120.f, screenResolution.y / 2.f - 140.f), window, 60, Color::Red);
        displayText("Rings: " + to_string(sonic.getRingsCollected()), Vector2f(screenResolution.x / 2.f - 40.f, screenResolution.y / 2.f - 60.f), window, 30, Color::White);
        displayText("Score: " + to_string(sonic.getScore()), Vector2f(screenResolution.x / 2.f - 40.f, screenResolution.y / 2.f - 20.f), window, 30, Color::White);

        window.display();
    }
}

void showLevelUpScreen(RenderWindow &window, float duration)
{
    Clock clock;

    while (window.isOpen() && clock.getElapsedTime().asSeconds() < duration)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();

        displayText("LEVEL UP", Vector2f(screenResolution.x / 2.f - 80.f, screenResolution.y / 2.f - 100.f), window, 60, Color::Green);

        window.display();
    }
}

void levelOne(RenderWindow &window, Event &event)
{

    Object background("bg1.png", Vector2f(0, 0), Vector2f(0.64f, 0.67f));
    const int rows = 12;
    const int cols = 200;
    // enemy testEnemy1("CrabMeat.png", 270, 101, 1.45f, 1.3f, 8, 54.f, 41);
    // enemy testEnemy2("motoBug.png", 101, 100, 1.12f, 1.12f, 4, 43.5f, 41);
    // enemy testEnemy3("beeBot.png", 101, 400, 2.f, 2.f, 5, 34.f, 20);
    // enemy testEnemy4("batBrain.png", 551, 100, 1.5f, 1.5f, 5, 65.f, 40);

    char screenLayout[rows][cols] = {

        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'},
        {'B', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B'},
        {'B', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', 'B', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U'},
        {'B', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', 'E'},
        {'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B'},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', 'E', 'U', 'E', 'U', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B'},
        {'B', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B'},
        {'E', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', 'R', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'E', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', 'B', 'B', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B'},
        {'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'S', 'U', ' ', ' ', ' ', ' ', 'B', 'S', 'B', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'E', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', 'B', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B'},
        {'B', ' ', ' ', 'R', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '2', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'B', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', 'B', 'B', '2', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B'},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'S', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B'},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B'}};
    bool ringCollected[rows][cols] = {false}; // Initially, all rings are uncollected

    int e1 = 0, e2 = 0, e3 = 0, e4 = 0;
    countEnemies(screenLayout, 12, e1, e2, e3, e4);

    // cout << e1 << " " << e2 << " " << e3 << " " << e4 << endl;

    enemy *testEnemy1 = new enemy[e1];
    enemy *testEnemy2 = new enemy[e2];
    enemy *testEnemy3 = new enemy[e3];
    e4_type *testEnemy4 = new e4_type[e4];

    int idx1 = 0, idx2 = 0, idx3 = 0, idx4 = 0;

    spawnEnemies(screenLayout, rows, cols, testEnemy1, e1, idx1, testEnemy2, e2, idx2, testEnemy3, e3, idx3, testEnemy4, e4, idx4);

    int TotalRingsToBeCollected = calculateRings(screenLayout);

    cout << "Total Rings to be collected: " << TotalRingsToBeCollected << endl;

    float scrollX = 0.0f;
    float speed = 250.0f;
    int currentFrame = 4;
    string prev = "right";

    Obstacle brick1("brick2.png", 0, 0, 0.8f, 0.8f);
    Obstacle brick2("brick1.png", 0, 0, 0.8f, 0.8f);
    Obstacle brick3("brick3.png", 0, 0, 0.8f, 0.8f);
    Obstacle spike("spike.png", 0, 0, 0.75f, 0.75f);

    Ring ring1("ring.png", 0, 0, 1.25f, 1.25f);

    Sonic sonic(100, 100, 1.28f, 1.28f);
    Tales tales(100, 100, 1.5f, 1.5f);
    Knuckles knuckles(100, 100, 1.28f, 1.28f);

    Hitbox hitbox("0hit_box.png", 0, 0, 1.1f, 1.1f);

    // Test loading multiple enemy instances

    Clock deltaClock;
    Clock ScreenClockTimer;
    int ringFrame = 0;
    float animationSpeed = 0.15f;
    float ringsElapsedTime = 0.0f;
    float velocityX = 0;

    float sonicSpeed = 0.4f;
    float runSpeedMultiplier = 0.9f;

    ring1.setIntRect(0, 0, 19, 19);

    static bool isJumping = false;
    static float jumpVelocity = 0.0f;

    Button Back(" BACK ", Vector2f(920, 0), Color::White, 30);
    Back.ChangeColor();

    SoundBuffer jumpBuffer;
    Sound jumpSound;

    if (!jumpBuffer.loadFromFile("audio/Jump.wav"))
    {
        cerr << "Failed to load jump sound!" << std::endl;
        exit(EXIT_FAILURE);
    }
    jumpSound.setBuffer(jumpBuffer);
    jumpSound.setLoop(false); // Play only once per press
    jumpSound.setVolume(50);  // Set volume to 60% (default is 100%)

    SoundBuffer ringBuffer;
    Sound ringSound;

    // Load the ring sound
    if (!ringBuffer.loadFromFile("audio/Ring.wav"))
    {
        cerr << "Failed to load ring sound!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set up the sound
    ringSound.setBuffer(ringBuffer);
    ringSound.setLoop(false); // Play only once
    ringSound.setVolume(50);  // Set volume to 50% (default is 100%)

    float gameOverTimer = 2;
    bool gameOver = false;

    bool isImmune = false;
    float immunityTimer = 0.0f;

    bool fallToGround = false;

    // Initialize HP bar
    RectangleShape hpBarBackground(Vector2f(280.f, 20.f));
    hpBarBackground.setFillColor(Color::Red);
    hpBarBackground.setPosition(400.f, 15.f);

    RectangleShape hpBar(Vector2f(200.f, 20.f));
    hpBar.setFillColor(Color::Green);
    hpBar.setPosition(400.f, 15.f);

    fireBall fireBallsUnlimited[100];

    int ballsLaunched = 0;

    int mode = 1;             // Starting value
    bool pKeyPressed = false; // To avoid multiple increments on one press

    float switchTimerKnucles = 4;
    float k_time = 0;
    float switchTimerTales = 7;
    float t_time = 0;

    bool first_fly_activation = false;

    while (window.isOpen())
    {
        // cout << isJumping << " " << fallToGround<< endl;

        hpBar.setSize(Vector2f(200.f * (sonic.hp / 100.f), 20.f));

        if (tales.flyactivated)
        {
            hitbox.copyCordinates(sonic, 10);
        }
        else
        {
            hitbox.copyCordinates(sonic);
        }

        float deltaTime = deltaClock.restart().asSeconds();
        int totalSecondsPassed = static_cast<int>(ScreenClockTimer.getElapsedTime().asSeconds());
        int minutes = totalSecondsPassed / 60;
        int seconds = totalSecondsPassed % 60;
        for (int i = 0; i < e1; i++)
        {
            // Move left
            if (!testEnemy1[i].moveRight)
            {
                testEnemy1[i].movedDistance -= 50 * deltaTime;

                if (testEnemy1[i].movedDistance <= 0)
                {
                    testEnemy1[i].movedDistance = 0;
                    testEnemy1[i].moveRight = true;
                }
                testEnemy1[i].sprite.setScale(1.28f, 1.28f); // Face left
            }
            else // Move right
            {
                testEnemy1[i].movedDistance += 50 * deltaTime;

                if (testEnemy1[i].movedDistance >= 190)
                {
                    testEnemy1[i].movedDistance = 190;
                    testEnemy1[i].moveRight = false;
                }

                testEnemy1[i].sprite.setScale(-1.28f, 1.28f); // Face right (flipped)
            }

            // World position
            float worldX = testEnemy1[i].x + testEnemy1[i].movedDistance;
            float worldY = testEnemy1[i].y;

            // Screen position
            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Check if off-screen and hide
            if (screenX + testEnemy1[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                // Hide the enemy by moving it off-screen
                testEnemy1[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            // Adjust for flipped sprite anchor
            if (testEnemy1[i].sprite.getScale().x < 0)
                screenX += testEnemy1[i].getWidth();

            // Update actual visible position
            testEnemy1[i].changePosition(Vector2f(screenX, screenY));
        }

        for (int i = 0; i < e2; i++)
        {
            float worldX = testEnemy2[i].x;
            float worldY = testEnemy2[i].y;

            // Convert world coordinates to screen coordinates
            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Check if the enemy is off-screen and move it out of view if so
            if (screenX + testEnemy2[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy2[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            bool canMoveRight = true;
            bool canMoveLeft = true;

            // Check if there's a block on the right side of the enemy
            int colRight = static_cast<int>((worldX + testEnemy2[i].getWidth()) / 50); // Right side of the enemy
            int rowBelow = static_cast<int>((worldY + hitbox.getHeight()) / 50);       // Tile just below the enemy

            // Check the tile to the right of the enemy
            if (colRight >= 0 && colRight < cols && rowBelow >= 0 && rowBelow < rows)
            {
                char tileRight = screenLayout[rowBelow][colRight - 1];
                if (tileRight == 'B' || tileRight == 'U' || tileRight == 'E')
                {
                    canMoveRight = false; // Block movement to the right
                }
            }

            // Check if there's a block on the left side of the enemy
            int colLeft = static_cast<int>((worldX) / 50); // Left side of the enemy
            if (colLeft >= 0 && colLeft < cols && rowBelow >= 0 && rowBelow < rows)
            {
                char tileLeft = screenLayout[rowBelow][colLeft];
                if (tileLeft == 'B' || tileLeft == 'U' || tileLeft == 'E')
                {
                    canMoveLeft = false; // Block movement to the left
                }
            }

            // Check if Sonic and enemy are both on the screen
            if (sonic.getLeft() >= 0 && sonic.getRight() <= screenResolution.x &&
                screenX >= 0 && screenX <= screenResolution.x)
            {
                // Move enemy towards Sonic on the x-axis if no obstacle
                float e_S = 70.0f;
                if (canMoveRight && screenX < sonic.getLeft())
                {
                    testEnemy2[i].x += e_S * deltaTime;           // Move right
                    testEnemy2[i].sprite.setScale(-1.28f, 1.28f); // Flip sprite to face right
                }
                else if (canMoveLeft && screenX > sonic.getLeft())
                {
                    testEnemy2[i].x -= e_S * deltaTime; // Move left
                    testEnemy2[i].sprite.setOrigin(testEnemy2[i].getWidth() / 2, testEnemy2[i].getHeight() / 2);

                    if (canMoveRight && screenX < sonic.getLeft())
                    {
                        testEnemy2[i].x += e_S * deltaTime;
                        testEnemy2[i].sprite.setScale(-1.28f, 1.28f);
                    }
                    else if (canMoveLeft && screenX > sonic.getLeft())
                    {
                        testEnemy2[i].x -= e_S * deltaTime;
                        testEnemy2[i].sprite.setScale(1.28f, 1.28f);
                    }

                    testEnemy2[i].sprite.setScale(1.28f, 1.28f); // Flip sprite to face left
                }
            }
            // Update the enemy position on screen
            testEnemy2[i].changePosition(Vector2f(screenX + (canMoveLeft ? -20 : 20), screenY + 30));
        }

        for (int i = 0; i < e3; i++)
        {
            float worldX = testEnemy3[i].x;
            float worldY = testEnemy3[i].y;

            float screenX = worldX - scrollX;
            float screenY = worldY;

            if (screenX + testEnemy3[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy3[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            float targetX = sonic.getLeft() + scrollX;
            float targetY = sonic.getTop();

            float speed = 80.0f;

            float dx = targetX - worldX;
            float dy = targetY - worldY;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > 1.0f)
            {
                float moveX = (dx / dist) * speed * deltaTime;
                float moveY = (dy / dist) * speed * deltaTime;

                testEnemy3[i].x += moveX;
                testEnemy3[i].y += moveY;

                // Flip sprite
                if (moveX > 0)
                    testEnemy3[i].sprite.setScale(-2.f, 2.f); // Facing right
                else if (moveX < 0)
                    testEnemy3[i].sprite.setScale(2.f, 2.f); // Facing left
            }

            screenX = testEnemy3[i].x - scrollX;
            screenY = testEnemy3[i].y;

            testEnemy3[i].changePosition(Vector2f(screenX, screenY)); // You can manually tweak here if needed
        }
        for (int i = 0; i < e4; i++)
        {
            // Handle patrol movement
            if (!testEnemy4[i].moveRight)
            {
                testEnemy4[i].movedDistance -= 50 * deltaTime;

                if (testEnemy4[i].movedDistance <= 0)
                {
                    testEnemy4[i].movedDistance = 0;
                    testEnemy4[i].moveRight = true;
                }
                testEnemy4[i].sprite.setScale(1.28f, 1.28f); // Face left
            }
            else
            {
                testEnemy4[i].movedDistance += 50 * deltaTime;

                if (testEnemy4[i].movedDistance >= 190)
                {
                    testEnemy4[i].movedDistance = 190;
                    testEnemy4[i].moveRight = false;
                }
                testEnemy4[i].sprite.setScale(-1.28f, 1.28f); // Face right (flipped)
            }

            // World and screen positions
            float worldX = testEnemy4[i].x + testEnemy4[i].movedDistance;
            float worldY = testEnemy4[i].y;

            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Skip if off-screen
            if (screenX + testEnemy4[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy4[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            // Adjust position for flipped sprite
            if (testEnemy4[i].sprite.getScale().x < 0)
                screenX += testEnemy4[i].getWidth();

            // Check if Sonic is below the enemy within horizontal range
            float sonicX = sonic.getLeft() + scrollX;
            float sonicY = sonic.getTop();
            float xDistance = abs(sonicX - worldX);

            if (xDistance <= 400 && sonicY > worldY && testEnemy4[i].canFire)
            {
                for (int j = 0; j < 100; j++)
                {
                    if (fireBallsUnlimited[j].isDestroyed)
                    {

                        fireBallsUnlimited[j].worldPosition = Vector2f(worldX, worldY);
                        fireBallsUnlimited[j].sprite.setPosition(worldX - scrollX, worldY + 10);
                        fireBallsUnlimited[j].isDestroyed = false;
                        fireBallsUnlimited[j].hasStartedFalling = false;
                        fireBallsUnlimited[j].spawnClock.restart();

                        testEnemy4[i].canFire = false;
                        testEnemy4[i].fireballCooldown.restart(); // Start cooldown
                        break;
                    }
                }
            }

            if (!testEnemy4[i].canFire && testEnemy4[i].fireballCooldown.getElapsedTime().asSeconds() >= 4.0f)
            {
                testEnemy4[i].canFire = true;
            }

            // Apply final screen position
            testEnemy4[i].changePosition(Vector2f(screenX, screenY));
        }

        if (isImmune)
        {
            immunityTimer += deltaTime;
            if (immunityTimer >= 3.f)
            {
                isImmune = false;
                immunityTimer = 0 ;
            }
        }

        string timeString = (minutes < 10 ? "0" : "") + std::to_string(minutes) +
                            "::" +
                            (seconds < 10 ? "0" : "") + std::to_string(seconds);
        ringsElapsedTime += deltaTime;

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed)
                window.close();

            // Mechanics What to Happen when Mouse Pressed
            if (event.type == Event::MouseButtonPressed)
            {
                // Mouse Pixels Input Where Clicked
                Vector2i mousePosition = Mouse::getPosition(window);
                cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

                if (event.mouseButton.button == Mouse::Left)
                {
                    cout << " Left Mouse Button Pressed " << endl;
                    if (Back.isClicked(mousePosition))
                    {
                        // Back.ChangeColor();
                        cout << "Back is clicked" << endl;
                        showLeaderBoard = false;
                        menu = true;
                        return;
                    }
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::P))
            {
                if (!pKeyPressed)
                {
                    mode++;
                    if (mode > 3)
                    {

                        mode = 1;
                    }
                    if (mode != 2)
                    {
                        first_fly_activation = false;
                        tales.flyactivated = false;
                    }
                    pKeyPressed = true;
                }
            }
            else
            {
                pKeyPressed = false; // Reset when key is released
            }
        }
        k_time += deltaTime;
        if (k_time >= switchTimerKnucles && mode == 3)
        {
            mode = 1;
            k_time = 0; // correct: reset k_time
        }

        t_time += deltaTime;
        if (t_time >= switchTimerTales && mode == 2)
        {
            mode = 1;
            t_time = 0; // correct: reset t_time
            tales.flyactivated = false;
            first_fly_activation = false;
            tales.changeImage("jumpTales.png");
            tales.changeScale({1.2f, 1.2f});
            tales.setIntRect(0 * 45, 0, 40, 35);
        }

        float currentSpeed = sonicSpeed;

        // Movement Input
        bool movingLeft = Keyboard::isKeyPressed(Keyboard::A);
        bool movingRight = Keyboard::isKeyPressed(Keyboard::D);

        if (mode == 2)
        {
            if (!first_fly_activation)
            {
                sonic.changePosition({sonic.getLeft(), sonic.getTop() - 100});
                first_fly_activation = true;
            }
            tales.flyactivated = true;
            tales.changeImage("fly_l.png");
        }
        // Handle movement
        if (movingLeft && sonic.getLeft() > 50)
        {
            velocityX = -currentSpeed; // Use currentSpeed for movement
            if (isJumping)
            {
                currentFrame = 3;
            }
            else
            {
                currentFrame = 2;
            }

            sonic.changePosition(Vector2f(sonic.getLeft() - currentSpeed, sonic.getTop()));
            prev = "left";
        }
        else if (movingRight && sonic.getRight() < screenResolution.x - 50)
        {
            velocityX = currentSpeed; // Use currentSpeed for movement
            if (isJumping)
            {
                currentFrame = 6;
            }
            else
            {
                currentFrame = 5;
            }
            sonic.changePosition(Vector2f(sonic.getLeft() + currentSpeed, sonic.getTop()));

            prev = "right";
        }
        else
        {
            velocityX = 0.0f;
            if (isJumping && prev == "left")
            {
                currentFrame = 3;
            }
            else if (isJumping && prev == "right")
            {
                currentFrame = 6;
            }
            else
            {
                currentFrame = (prev == "left" ? 1 : 4);
            }
        }

        if (!tales.flyactivated)
        {
            if (Keyboard::isKeyPressed(Keyboard::Space) && (!isJumping) && fallToGround == false)
            {
                jumpSound.play();

                isJumping = true;
                jumpVelocity = 497.0f; // Increased initial jump velocity for faster jump

                if (strcmp(prev.c_str(), "left") == 0)
                {
                    currentFrame = 6;
                }
                else
                {
                    currentFrame = 3;
                }
            }

            if (isJumping)
            {

                jumpVelocity -= 1100.0f * deltaTime; // Increased gravity effect for higher jump
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() - jumpVelocity * deltaTime));
            }
            // If Sonic is not jumping and no collision with ground or platform, then fall
            if (!isJumping)
            {
                bool isOnGround = false;

                // Compute center-bottom of Sonic
                float centerX = sonic.getLeft() + (hitbox.getWidth() / 2.0f);
                float bottomY = sonic.getTop() + hitbox.getHeight();

                // Convert to tile indices
                int rowBelow = static_cast<int>((bottomY) / 50) + 1; // +1 to check tile *just* below the bottom
                int colCenter = static_cast<int>((centerX + scrollX) / 50);

                if (rowBelow >= 0 && rowBelow < rows &&
                    colCenter >= 0 && colCenter < cols)
                {
                    char tileBelow = screenLayout[rowBelow][colCenter];

                    if (tileBelow == 'B' || tileBelow == 'U' || tileBelow == 'E')
                    {
                        // Scout << "Standing at [" << rowBelow << "][" << colCenter << "] on tile '" << tileBelow << "'\n";
                        isOnGround = true;

                        // Change sprite back to idle/run based on direction & speed
                        if (prev == "left")
                        {
                            currentFrame = 2;
                        }
                        else
                        {
                            currentFrame = 5;
                        }
                    }
                }

                // If not on ground, start falling
                if (!isOnGround)
                {
                    isJumping = true;
                    jumpVelocity = 0.0f; // Begin fall
                }
            }

            if (sonic.getTop() >= 497 + 80)
            { // Adjusted ground level for Sonic to come back to 497

                sonic.changePosition(Vector2f(sonic.getLeft(), 497 + 80));
                isJumping = false;
                jumpVelocity = 0.0f;

                // Reset to previous image after jump
                if (prev == "left")
                {
                    currentFrame = 1;
                }
                else
                {
                    currentFrame = 4;
                }
            }

            float sonicX = sonic.getLeft();

            if (sonicX < 250 && velocityX < 0)
            { // Sonic is trying to move left past x = 250
                if (scrollX > 0)
                {
                    scrollX -= speed * deltaTime;

                    currentFrame = (isJumping) ? 3 : 2;

                    sonic.changePosition(Vector2f(250, sonic.getTop()));
                }
                else
                {

                    currentFrame = (isJumping) ? 3 : 2;

                    sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                }
            }
            else if (sonicX > 800 && velocityX > 0)
            { // Sonic is trying to move right past x = 800
                if (scrollX < cols * 50 - screenResolution.x)
                {
                    scrollX += speed * deltaTime;
                    if (isJumping)
                    {
                        currentFrame = 6;
                    }
                    else
                    {
                        currentFrame = 5;
                    }

                    sonic.changePosition(Vector2f(800, sonic.getTop()));
                }
                else
                {
                    if (isJumping)
                    {
                        currentFrame = 6;
                    }
                    else
                    {
                        currentFrame = 5;
                    }

                    sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                }

                currentFrame = (isJumping) ? 6 : 5;
            }
            else
            {

                sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                if (velocityX > 0)
                {
                    if (isJumping)
                    {

                        currentFrame = 6;
                    }
                    else
                    {

                        currentFrame = 5;
                    }
                }
                else if (velocityX < 0)
                {
                    if (isJumping)
                    {
                        currentFrame = 3;
                    }
                    else
                    {
                        currentFrame = 2;
                    }
                }
                else
                {
                    if (isJumping)
                    {
                        currentFrame = prev == "left" ? 3 : 6;
                    }
                    else
                    {
                        currentFrame = prev == "left" ? 1 : 4;
                    }
                }
            }
        }
        else
        {
            bool movingUp = Keyboard::isKeyPressed(Keyboard::W);
            bool movingDown = Keyboard::isKeyPressed(Keyboard::S);

            float flySpeed = 200.0f; // Flying speed

            float s_fly_x = sonic.getLeft();

            if (s_fly_x < 250)
            { // Sonic is trying to move left past x = 250
                mode = 1;
                t_time = 0; // correct: reset t_time
                tales.flyactivated = false;
                first_fly_activation = false;
                tales.changeImage("jumpTales.png");
                tales.changeScale({1.2f, 1.2f});
                tales.setIntRect(0 * 45, 0, 40, 35);
            }
            else if (s_fly_x > 800)
            { // Sonic is trying to move right past x = 800
                mode = 1;
                t_time = 0; // correct: reset t_time
                tales.flyactivated = false;
                first_fly_activation = false;
                tales.changeImage("jumpTales.png");
                tales.changeScale({1.2f, 1.2f});
                tales.setIntRect(0 * 45, 0, 40, 35);
            }

            if (movingUp)
            {
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() - flySpeed * deltaTime));
            }
            if (movingDown)
            {
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() + flySpeed * deltaTime));
            }
        }

        animateRings(ringsElapsedTime, animationSpeed, ringFrame, ring1);
        // Iterate through all the enemies and animate them
        for (int i = 0; i < e1; i++)
        {
            animateCrabMeat(deltaTime, testEnemy1[i]);
        }

        for (int i = 0; i < e2; i++)
        {
            animateMotoBug(deltaTime, testEnemy2[i]);
        }

        for (int i = 0; i < e3; i++)
        {
            animateBeeBot(deltaTime, testEnemy3[i]);
        }

        for (int i = 0; i < e4; i++)
        {
            animateBatBrain(deltaTime, testEnemy4[i]);
        }

        if (currentFrame == 1)
        {
            sonic.changeImage("0left_still.png");

            tales.state = "still";
            knuckles.state = "left_still";

            tales.changeImage("tales_stilll.png");
            tales.setIntRect(0, 0, 86, 86);
            tales.changeScale({0.8f, 0.8f});

            knuckles.changeImage("k_still_left.png");
            knuckles.setIntRect(0, 0, 100, 100);
            knuckles.changeScale({1.f, 1.f});

            tales.changePosition({sonic.getLeft() + 10, sonic.getTop() - 13});
            knuckles.changePosition({sonic.getLeft() + 37, sonic.getTop() - 13});

            tales.frame = 0;
            knuckles.frame = 0;
        }
        else if (currentFrame == 2)
        {
            sonic.changeImage("0jog_left.png");

            if (knuckles.state != "left")
            {
                tales.setIntRect(0 * 45, 0, 45, 35);
            }
            tales.state = "left";
            if (knuckles.state != "left")
            {
                knuckles.setIntRect(0, 5, 35, 30);
            }
            knuckles.state = "left";

            knuckles.changeImage("k_run_left.png");
            knuckles.changeScale({1.8f, 1.8f});

            tales.changePosition({sonic.getLeft() + 19, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() + 37, sonic.getTop() - 2});
        }
        else if (currentFrame == 3)
        {
            sonic.changeImage("0upL.png");

            tales.state = "jump";
            knuckles.state = "jump_left";

            knuckles.changeImage("k_jump_left.png");
            knuckles.changeScale({0.85f, 0.85f});

            tales.changePosition({sonic.getLeft() + 12, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() + 30, sonic.getTop() - 4});
        }
        else if (currentFrame == 4)
        {
            sonic.changeImage("0right_still.png");

            tales.state = "still";
            knuckles.state = "right_still";

            tales.changeImage("tales_still.png");
            tales.setIntRect(0, 0, 86, 86);
            tales.changeScale({0.8f, 0.8f});

            knuckles.changeImage("k_still_right.png");
            knuckles.setIntRect(0, 0, 100, 100);
            knuckles.changeScale({1.f, 1.f});

            tales.changePosition({sonic.getLeft() - 28, sonic.getTop() - 13});
            knuckles.changePosition({sonic.getLeft() - 50, sonic.getTop() - 13});

            tales.frame = 0;
            knuckles.frame = 0;
        }
        else if (currentFrame == 5)
        {
            sonic.changeImage("0jog_right.png");
            if (tales.state != "right")
            {
                tales.setIntRect(2 * 45, 0, 45, 35);
            }
            tales.state = "right";
            if (knuckles.state != "right")
            {
                knuckles.setIntRect(0, 5, 35, 30);
            }
            knuckles.state = "right";

            knuckles.changeImage("k_run_right.png");
            knuckles.changeScale({1.8f, 1.8f});
            // knuckles.setIntRect(0, 5, 35, 30);

            tales.changePosition({sonic.getLeft() - 37, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() - 55, sonic.getTop() - 5});
        }
        else if (currentFrame == 6)
        {
            sonic.changeImage("0upR.png");

            tales.state = "jump";
            knuckles.state = "jump_right";

            knuckles.changeImage("k_jump_right.png");
            knuckles.changeScale({0.85f, 0.85f});

            tales.changePosition({sonic.getLeft() - 20, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() - 45, sonic.getTop() - 5});
        }

        knuckles.update(deltaTime);
        tales.update(deltaTime);
        sonic.update(deltaTime, velocityX, isJumping);

        for (int i = 0; i < 100; i++)
        {
            if (!fireBallsUnlimited[i].isDestroyed)
            {
                // Move in world space (don't apply scrollX here)
                Vector2f worldPos = fireBallsUnlimited[i].worldPosition; // You'll need to store world position separately
                worldPos.y += 100.f * deltaTime;

                // Destroy if it's too far vertically in the world
                if (worldPos.y > 700.f) // or some max level height
                {
                    fireBallsUnlimited[i].isDestroyed = true;
                    fireBallsUnlimited[i].sprite.setPosition(-1000, -1000);
                }
                else
                {
                    fireBallsUnlimited[i].worldPosition = worldPos;
                    Vector2f screenPos(worldPos.x - scrollX, worldPos.y);
                    fireBallsUnlimited[i].sprite.setPosition(screenPos);
                }
            }
        }

        // ---- For testEnemy1 ----
        for (int i = 0; i < e1; i++)
        {
            if (!testEnemy1[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy1[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }

                    isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy2 ----
        for (int i = 0; i < e2; i++)
        {
            if (!testEnemy2[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy2[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy3 ----
        for (int i = 0; i < e3; i++)
        {
            if (!testEnemy3[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy3[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy4 ----
        for (int i = 0; i < e4; i++)
        {
            if (!testEnemy4[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy4[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Sonic hit enemy4 from side -> lost 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Tales hit enemy4 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {

                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy4 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        window.clear(Color::White);
        background.draw(window);

        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                float px = j * 50 - scrollX;
                float py = i * 50;

                if (px + 50 < 0 || px > screenResolution.x)
                    continue;

                switch (screenLayout[i][j])
                {
                case 'B': // Wall (Ground)
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (tales.flyactivated)
                    {
                        if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                            playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                        {
                            if (playerTop > obstacleBottom && playerBottom < obstacleBottom)
                            {
                                tales.changePosition({sonic.getLeft(), sonic.getTop() + 2});
                            }
                            // From above (player landing on wall)
                            else if (playerBottom > obstacleTop && playerTop < obstacleTop)
                            {
                                tales.changePosition({sonic.getLeft(), sonic.getTop() - 1});
                            }
                            // From left (player moving right into wall)
                            else if (playerRight > obstacleLeft && playerLeft < obstacleLeft)
                            {
                                tales.changePosition({sonic.getLeft() - 1, sonic.getTop()});
                            }
                            // From right (player moving left into wall)
                            else if (playerLeft < obstacleRight && playerRight > obstacleRight)
                            {
                                tales.changePosition({sonic.getLeft() + 1, sonic.getTop()});
                            }
                        }
                    }
                    else
                    {
                        if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                            playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                        {
                            if (playerTop > obstacleBottom && playerBottom < obstacleBottom)
                            {
                                fallToGround = false;
                                isJumping = false;
                                sonic.changePosition({sonic.getLeft(), sonic.getTop() + 2});
                            }
                            // From above (player landing on wall)
                            else if (playerBottom > obstacleTop && playerTop < obstacleTop)
                            {
                                fallToGround = false;
                                isJumping = false;
                                sonic.changePosition({sonic.getLeft(), sonic.getTop() - 1});
                            }
                            // From left (player moving right into wall)
                            else if (playerRight > obstacleLeft && playerLeft < obstacleLeft)
                            {

                                if (mode == 3)
                                {
                                    screenLayout[i][j] = ' ';
                                    continue;
                                }

                                isJumping = false;
                                sonic.changePosition({sonic.getLeft() - 1, sonic.getTop()});
                            }
                            // From right (player moving left into wall)
                            else if (playerLeft < obstacleRight && playerRight > obstacleRight)
                            {
                                if (mode == 3)
                                {

                                    screenLayout[i][j] = ' ';
                                    continue;
                                }

                                isJumping = false;
                                sonic.changePosition({sonic.getLeft() + 1, sonic.getTop()});
                            }
                        }
                    }

                    brick1.changePosition(Vector2f(px, py));
                    brick1.draw(window);
                    break;
                }
                case 'U': // Platform (Collision only from above)
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                        playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                    {
                        // From above (player landing on wall)
                        if (playerBottom > obstacleTop && playerTop < obstacleTop)
                        {

                            isJumping = false;
                            sonic.changePosition({sonic.getLeft(), sonic.getTop() - 5});
                        }
                    }

                    brick2.changePosition(Vector2f(px, py));
                    brick2.draw(window);
                    break;
                }

                case 'S':
                {
                    float spikeLeft = j * 50 - scrollX;
                    float spikeTop = i * 50;
                    float spikeRight = spikeLeft + 50;
                    float spikeBottom = spikeTop + 50;

                    FloatRect spikeBounds(spikeLeft, spikeTop, 50, 50);
                    FloatRect playerBounds = hitbox.getGlobalBond();

                    if (spikeBounds.intersects(playerBounds) && !isImmune)
                    {
                        sonic.takeDamage(10); // Reduce health by 10
                        isImmune = true;
                        immunityTimer = 1.0f; // 1 second of immunity
                    }
                    spike.changePosition(Vector2f(px, py));
                    spike.draw(window);
                    break;
                }
                case 'E': // Last and second last row treated as platforms
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                        playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                    {
                        // From above (player landing on wall)
                        if (playerBottom > obstacleTop && playerTop < obstacleTop)
                        {

                            isJumping = false;
                            sonic.changePosition({sonic.getLeft(), sonic.getTop() - 5});
                        }
                    }

                    brick2.changePosition(Vector2f(px, py));
                    brick2.draw(window);
                    break;
                }
                case 'R': // Ring
                    if (!ringCollected[i][j])
                    {
                        // Same as before, handle ring collection logic here
                        FloatRect ringBounds(px + 15, py + 20, 19, 19);  // Ring size
                        FloatRect playerBounds = hitbox.getGlobalBond(); // Or sonic.getSprite().getGlobalBounds()

                        if (ringBounds.intersects(playerBounds))
                        {
                            // To play the ring sound
                            ringSound.play();

                            ringCollected[i][j] = true; // Mark ring as collected
                            sonic.addRings();           // Increase score/rings if you have such a function
                        }
                        else
                        {
                            ring1.changePosition(Vector2f(px + 15, py + 20));
                            ring1.draw(window);
                        }
                    }
                    break;
                }
            }
        }


        if (isImmune)
        {
            displayText("IS IMMUNE", {screenResolution.x / 2 - 100, 50}, window, 40, Color::Magenta);
        }

        // hitbox.draw(window);
        if (!tales.flyactivated)
        {
            knuckles.draw(window);
            tales.draw(window);
            sonic.draw(window);
        }
        else
        {
            if (movingLeft)
            {
                tales.changeImage("fly_l.png");
                tales.changeScale({0.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (movingRight)
            {
                tales.changeImage("fly_r.png");
                tales.changeScale({.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (currentFrame == 1 || currentFrame == 3)
            {
                tales.changeImage("fly_l.png");
                tales.changeScale({0.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (currentFrame == 4 || currentFrame == 6)
            {
                tales.changeImage("fly_r.png");
                tales.changeScale({.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            tales.draw(window);
        }

        for (int i = 0; i < e1; i++)
        {
            if (testEnemy1[i].isAlive)
                testEnemy1[i].draw(window);
        }

        for (int i = 0; i < e2; i++)
        {
            if (testEnemy2[i].isAlive)
                testEnemy2[i].draw(window);
        }

        for (int i = 0; i < e3; i++)
        {
            if (testEnemy3[i].isAlive)
                testEnemy3[i].draw(window);
        }

        for (int i = 0; i < e4; i++)
        {
            if (testEnemy4[i].isAlive)
                testEnemy4[i].draw(window);
        }

        for (int i = 0; i < 100; i++)
        {
            if (!fireBallsUnlimited[i].isDestroyed)
            {
                window.draw(fireBallsUnlimited[i].sprite);
            }
        }

        if (minutes < gameOverTimer && !(TotalRingsToBeCollected == sonic.getRingsCollected()) && sonic.hp > 0)
        {
            displayText("Time: " + timeString, Vector2f(20.f, 10.f), window, 30, Color(16, 124, 43));
            displayText("Rings: " + to_string(sonic.getRingsCollected()), Vector2f(20.f, 50.f), window, 30, Color::Yellow);
            displayText(" / " + to_string(TotalRingsToBeCollected), Vector2f(120.f, 50.f), window, 30, Color::Green);
            displayText("Score: " + to_string(sonic.getScore()), Vector2f(20.f, 90.f), window, 30, Color::White);
        }
        else if (TotalRingsToBeCollected >= sonic.getRingsCollected())
        {
            showLevelUpScreen(window, 3.5f); // 5 seconds level-up screen

            return;
        }
        else if (sonic.hp <= 0)
        {
            showGameOverScreen(window, sonic, 3.5f); // 5 seconds game over screen

            return;
        }
        else // Time over but not game over
        {
            showTimeOverScreen(window, sonic, 3.5f); // show for 5 seconds
            return;
        }

        if (mode == 1)
            displayText("Sonic Control", {700, 5}, window, 30, Color::Blue);
        else if (mode == 2)
            displayText("Tails Control", {700, 5}, window, 30, Color::Yellow);
        else
            displayText("Knuckles Control", {700, 5}, window, 30, Color::Red);

        Back.draw(window);
        window.draw(hpBarBackground);
        window.draw(hpBar);

        window.display();
    }

    delete[] testEnemy1;
    delete[] testEnemy2;
    delete[] testEnemy3;
    delete[] testEnemy4;
}

void countEnemies_2(const char screenLayout[][250], int rows, int &e1, int &e2, int &e3, int &e4)
{
    e1 = e2 = e3 = e4 = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < 250; j++)
        {
            switch (screenLayout[i][j])
            {
            case '1':
                e1++;
                break;
            case '2':
                e2++;
                break;
            case '3':
                e3++;
                break;
            case '4':
                e4++;
                break;
            }
        }
    }
}

void spawnEnemies_2(char screenLayout[][250], int rows, int cols,
                  enemy testEnemy1[], int e1, int &idx1,
                  enemy testEnemy2[], int e2, int &idx2,
                  enemy testEnemy3[], int e3, int &idx3,
                  e4_type testEnemy4[], int e4, int &idx4)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vector2f position(j * 50, i * 50);
            char tile = screenLayout[i][j];

            if (tile == '1' && idx1 < e1)
            {
                testEnemy1[idx1].changeImage("CrabMeat.png");
                testEnemy1[idx1].changeScale({1.45f, 1.3f});
                testEnemy1[idx1].changePosition(position);
                testEnemy1[idx1].frameCount = 8;
                testEnemy1[idx1].frameTime = 0.15f;
                testEnemy1[idx1].x = position.x;
                testEnemy1[idx1].y = position.y;
                idx1++;
            }
            else if (tile == '2' && idx2 < e2)
            {
                testEnemy2[idx2].changeImage("motoBug.png");
                testEnemy2[idx2].changeScale({1.12f, 1.12f});
                testEnemy2[idx2].changePosition(position);
                testEnemy2[idx2].frameCount = 4;
                testEnemy2[idx2].frameTime = 0.15f;
                testEnemy2[idx2].x = position.x;
                testEnemy2[idx2].y = position.y;
                idx2++;
            }
            else if (tile == '3' && idx3 < e3)
            {
                testEnemy3[idx3].changeImage("beeBot.png");
                testEnemy3[idx3].changeScale({2.f, 2.f});
                testEnemy3[idx3].changePosition(position);
                testEnemy3[idx3].frameCount = 5;
                testEnemy3[idx3].frameTime = 0.15f;
                testEnemy3[idx3].x = position.x;
                testEnemy3[idx3].y = position.y;
                idx3++;
            }
            else if (tile == '4' && idx4 < e4)
            {
                testEnemy4[idx4].changeImage("batBrain.png");
                testEnemy4[idx4].changeScale({1.5f, 1.5f});
                testEnemy4[idx4].changePosition(position);
                testEnemy4[idx4].frameCount = 8;
                testEnemy4[idx4].frameTime = 0.15f;
                testEnemy4[idx4].x = position.x;
                testEnemy4[idx4].y = position.y;
                idx4++;
            }

            if (tile >= '1' && tile <= '4')
            {
                screenLayout[i][j] = ' ';
            }
        }
    }
}

int calculateRings_2(const char lvl[12][250])
{
    int rings = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            if (lvl[i][j] == 'R')
            {
                // cout << "Rings Found at " << i << " " << j << endl;
                rings++;
            }
        }
    }

    return rings;
}

void levelTwo(RenderWindow &window, Event &event)
{

    Object background("bg2.png", Vector2f(0, 0), Vector2f(0.64f, 0.67f));
    background.sprite.setColor(sf::Color::Blue);  // Icy Blue


    const int rows = 12;
    const int cols = 250;
    // enemy testEnemy1("CrabMeat.png", 270, 101, 1.45f, 1.3f, 8, 54.f, 41);
    // enemy testEnemy2("motoBug.png", 101, 100, 1.12f, 1.12f, 4, 43.5f, 41);
    // enemy testEnemy3("beeBot.png", 101, 400, 2.f, 2.f, 5, 34.f, 20);
    // enemy testEnemy4("batBrain.png", 551, 100, 1.5f, 1.5f, 5, 65.f, 40);

    char screenLayout[rows][cols] = {

        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B','B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B'},
        {'B', ' ', ' ', ' ', 'B', 'E', 'U', 'U', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', '3', ' ', ' ', ' ', '3', '4', ' ', 'B', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', 'E',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B',' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R'},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', 'E', 'U', 'E', 'U', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B','U', 'U', 'U', 'U', 'E', 'U', 'E', 'U', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R'},
        {'E', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'E', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', 'B', 'B', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B',' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', 'B', 'B', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U'},
        {'B', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', 'R', ' ', 'R', ' ', 'R', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'S', 'U', ' ', ' ', ' ', ' ', 'B', 'S', 'B', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'E', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', 'B', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B',' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', 'B', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', 'R', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '2', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'B', ' ', ' ', '2', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', 'B', 'B', '2', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B',' ', 'R', ' ', 'B', 'B', '2', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' '},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'S', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B','S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B'},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B','B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'}};
    bool ringCollected[rows][cols] = {false}; // Initially, all rings are uncollected

    int e1 = 0, e2 = 0, e3 = 0, e4 = 0;
    countEnemies_2(screenLayout, 12, e1, e2, e3, e4);

    // cout << e1 << " " << e2 << " " << e3 << " " << e4 << endl;

    enemy *testEnemy1 = new enemy[e1];
    enemy *testEnemy2 = new enemy[e2];
    enemy *testEnemy3 = new enemy[e3];
    e4_type *testEnemy4 = new e4_type[e4];

    int idx1 = 0, idx2 = 0, idx3 = 0, idx4 = 0;

    spawnEnemies_2(screenLayout, rows, cols, testEnemy1, e1, idx1, testEnemy2, e2, idx2, testEnemy3, e3, idx3, testEnemy4, e4, idx4);

    int TotalRingsToBeCollected = calculateRings_2(screenLayout);

    cout << "Total Rings to be collected: " << TotalRingsToBeCollected << endl;

    float scrollX = 0.0f;
    float speed = 300.0f;
    int currentFrame = 4;
    string prev = "right";

    Obstacle brick1("brick2.png", 0, 0, 0.8f, 0.8f);
    Obstacle brick2("brick1.png", 0, 0, 0.8f, 0.8f);
    Obstacle brick3("brick3.png", 0, 0, 0.8f, 0.8f);
    Obstacle spike("spike.png", 0, 0, 0.75f, 0.75f);

    Ring ring1("ring.png", 0, 0, 1.25f, 1.25f);

    Sonic sonic(100, 100, 1.28f, 1.28f);
    Tales tales(100, 100, 1.5f, 1.5f);
    Knuckles knuckles(100, 100, 1.28f, 1.28f);

    Hitbox hitbox("0hit_box.png", 0, 0, 1.1f, 1.1f);

    // Test loading multiple enemy instances

    Clock deltaClock;
    Clock ScreenClockTimer;
    int ringFrame = 0;
    float animationSpeed = 0.15f;
    float ringsElapsedTime = 0.0f;
    float velocityX = 0;

    float sonicSpeed = 0.6f;
    float runSpeedMultiplier = 0.9f;

    ring1.setIntRect(0, 0, 19, 19);

    static bool isJumping = false;
    static float jumpVelocity = 0.0f;

    Button Back(" BACK ", Vector2f(920, 0), Color::White, 30);
    Back.ChangeColor();

    SoundBuffer jumpBuffer;
    Sound jumpSound;

    if (!jumpBuffer.loadFromFile("audio/Jump.wav"))
    {
        cerr << "Failed to load jump sound!" << std::endl;
        exit(EXIT_FAILURE);
    }
    jumpSound.setBuffer(jumpBuffer);
    jumpSound.setLoop(false); // Play only once per press
    jumpSound.setVolume(50);  // Set volume to 60% (default is 100%)

    SoundBuffer ringBuffer;
    Sound ringSound;

    // Load the ring sound
    if (!ringBuffer.loadFromFile("audio/Ring.wav"))
    {
        cerr << "Failed to load ring sound!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set up the sound
    ringSound.setBuffer(ringBuffer);
    ringSound.setLoop(false); // Play only once
    ringSound.setVolume(50);  // Set volume to 50% (default is 100%)

    float gameOverTimer = 2;
    bool gameOver = false;

    bool isImmune = false;
    float immunityTimer = 0.0f;

    bool fallToGround = false;

    // Initialize HP bar
    RectangleShape hpBarBackground(Vector2f(280.f, 20.f));
    hpBarBackground.setFillColor(Color::Red);
    hpBarBackground.setPosition(400.f, 15.f);

    RectangleShape hpBar(Vector2f(200.f, 20.f));
    hpBar.setFillColor(Color::Green);
    hpBar.setPosition(400.f, 15.f);

    fireBall fireBallsUnlimited[100];

    int ballsLaunched = 0;

    int mode = 1;             // Starting value
    bool pKeyPressed = false; // To avoid multiple increments on one press

    float switchTimerKnucles = 4;
    float k_time = 0;
    float switchTimerTales = 7;
    float t_time = 0;

    bool first_fly_activation = false;

    while (window.isOpen())
    {
        // cout << isJumping << " " << fallToGround<< endl;

        hpBar.setSize(Vector2f(200.f * (sonic.hp / 100.f), 20.f));

        if (tales.flyactivated)
        {
            hitbox.copyCordinates(sonic, 10);
        }
        else
        {
            hitbox.copyCordinates(sonic);
        }

        float deltaTime = deltaClock.restart().asSeconds();
        int totalSecondsPassed = static_cast<int>(ScreenClockTimer.getElapsedTime().asSeconds());
        int minutes = totalSecondsPassed / 60;
        int seconds = totalSecondsPassed % 60;
        for (int i = 0; i < e1; i++)
        {
            // Move left
            if (!testEnemy1[i].moveRight)
            {
                testEnemy1[i].movedDistance -= 50 * deltaTime;

                if (testEnemy1[i].movedDistance <= 0)
                {
                    testEnemy1[i].movedDistance = 0;
                    testEnemy1[i].moveRight = true;
                }
                testEnemy1[i].sprite.setScale(1.28f, 1.28f); // Face left
            }
            else // Move right
            {
                testEnemy1[i].movedDistance += 50 * deltaTime;

                if (testEnemy1[i].movedDistance >= 190)
                {
                    testEnemy1[i].movedDistance = 190;
                    testEnemy1[i].moveRight = false;
                }

                testEnemy1[i].sprite.setScale(-1.28f, 1.28f); // Face right (flipped)
            }

            // World position
            float worldX = testEnemy1[i].x + testEnemy1[i].movedDistance;
            float worldY = testEnemy1[i].y;

            // Screen position
            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Check if off-screen and hide
            if (screenX + testEnemy1[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                // Hide the enemy by moving it off-screen
                testEnemy1[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            // Adjust for flipped sprite anchor
            if (testEnemy1[i].sprite.getScale().x < 0)
                screenX += testEnemy1[i].getWidth();

            // Update actual visible position
            testEnemy1[i].changePosition(Vector2f(screenX, screenY));
        }

        for (int i = 0; i < e2; i++)
        {
            float worldX = testEnemy2[i].x;
            float worldY = testEnemy2[i].y;

            // Convert world coordinates to screen coordinates
            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Check if the enemy is off-screen and move it out of view if so
            if (screenX + testEnemy2[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy2[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            bool canMoveRight = true;
            bool canMoveLeft = true;

            // Check if there's a block on the right side of the enemy
            int colRight = static_cast<int>((worldX + testEnemy2[i].getWidth()) / 50); // Right side of the enemy
            int rowBelow = static_cast<int>((worldY + hitbox.getHeight()) / 50);       // Tile just below the enemy

            // Check the tile to the right of the enemy
            if (colRight >= 0 && colRight < cols && rowBelow >= 0 && rowBelow < rows)
            {
                char tileRight = screenLayout[rowBelow][colRight - 1];
                if (tileRight == 'B' || tileRight == 'U' || tileRight == 'E')
                {
                    canMoveRight = false; // Block movement to the right
                }
            }

            // Check if there's a block on the left side of the enemy
            int colLeft = static_cast<int>((worldX) / 50); // Left side of the enemy
            if (colLeft >= 0 && colLeft < cols && rowBelow >= 0 && rowBelow < rows)
            {
                char tileLeft = screenLayout[rowBelow][colLeft];
                if (tileLeft == 'B' || tileLeft == 'U' || tileLeft == 'E')
                {
                    canMoveLeft = false; // Block movement to the left
                }
            }

            // Check if Sonic and enemy are both on the screen
            if (sonic.getLeft() >= 0 && sonic.getRight() <= screenResolution.x &&
                screenX >= 0 && screenX <= screenResolution.x)
            {
                // Move enemy towards Sonic on the x-axis if no obstacle
                float e_S = 70.0f;
                if (canMoveRight && screenX < sonic.getLeft())
                {
                    testEnemy2[i].x += e_S * deltaTime;           // Move right
                    testEnemy2[i].sprite.setScale(-1.28f, 1.28f); // Flip sprite to face right
                }
                else if (canMoveLeft && screenX > sonic.getLeft())
                {
                    testEnemy2[i].x -= e_S * deltaTime; // Move left
                    testEnemy2[i].sprite.setOrigin(testEnemy2[i].getWidth() / 2, testEnemy2[i].getHeight() / 2);

                    if (canMoveRight && screenX < sonic.getLeft())
                    {
                        testEnemy2[i].x += e_S * deltaTime;
                        testEnemy2[i].sprite.setScale(-1.28f, 1.28f);
                    }
                    else if (canMoveLeft && screenX > sonic.getLeft())
                    {
                        testEnemy2[i].x -= e_S * deltaTime;
                        testEnemy2[i].sprite.setScale(1.28f, 1.28f);
                    }

                    testEnemy2[i].sprite.setScale(1.28f, 1.28f); // Flip sprite to face left
                }
            }
            // Update the enemy position on screen
            testEnemy2[i].changePosition(Vector2f(screenX + (canMoveLeft ? -20 : 20), screenY + 30));
        }

        for (int i = 0; i < e3; i++)
        {
            float worldX = testEnemy3[i].x;
            float worldY = testEnemy3[i].y;

            float screenX = worldX - scrollX;
            float screenY = worldY;

            if (screenX + testEnemy3[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy3[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            float targetX = sonic.getLeft() + scrollX;
            float targetY = sonic.getTop();

            float speed = 80.0f;

            float dx = targetX - worldX;
            float dy = targetY - worldY;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > 1.0f)
            {
                float moveX = (dx / dist) * speed * deltaTime;
                float moveY = (dy / dist) * speed * deltaTime;

                testEnemy3[i].x += moveX;
                testEnemy3[i].y += moveY;

                // Flip sprite
                if (moveX > 0)
                    testEnemy3[i].sprite.setScale(-2.f, 2.f); // Facing right
                else if (moveX < 0)
                    testEnemy3[i].sprite.setScale(2.f, 2.f); // Facing left
            }

            screenX = testEnemy3[i].x - scrollX;
            screenY = testEnemy3[i].y;

            testEnemy3[i].changePosition(Vector2f(screenX, screenY)); // You can manually tweak here if needed
        }
        for (int i = 0; i < e4; i++)
        {
            // Handle patrol movement
            if (!testEnemy4[i].moveRight)
            {
                testEnemy4[i].movedDistance -= 50 * deltaTime;

                if (testEnemy4[i].movedDistance <= 0)
                {
                    testEnemy4[i].movedDistance = 0;
                    testEnemy4[i].moveRight = true;
                }
                testEnemy4[i].sprite.setScale(1.28f, 1.28f); // Face left
            }
            else
            {
                testEnemy4[i].movedDistance += 50 * deltaTime;

                if (testEnemy4[i].movedDistance >= 190)
                {
                    testEnemy4[i].movedDistance = 190;
                    testEnemy4[i].moveRight = false;
                }
                testEnemy4[i].sprite.setScale(-1.28f, 1.28f); // Face right (flipped)
            }

            // World and screen positions
            float worldX = testEnemy4[i].x + testEnemy4[i].movedDistance;
            float worldY = testEnemy4[i].y;

            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Skip if off-screen
            if (screenX + testEnemy4[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy4[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            // Adjust position for flipped sprite
            if (testEnemy4[i].sprite.getScale().x < 0)
                screenX += testEnemy4[i].getWidth();

            // Check if Sonic is below the enemy within horizontal range
            float sonicX = sonic.getLeft() + scrollX;
            float sonicY = sonic.getTop();
            float xDistance = abs(sonicX - worldX);

            if (xDistance <= 400 && sonicY > worldY && testEnemy4[i].canFire)
            {
                for (int j = 0; j < 100; j++)
                {
                    if (fireBallsUnlimited[j].isDestroyed)
                    {

                        fireBallsUnlimited[j].worldPosition = Vector2f(worldX, worldY);
                        fireBallsUnlimited[j].sprite.setPosition(worldX - scrollX, worldY + 10);
                        fireBallsUnlimited[j].isDestroyed = false;
                        fireBallsUnlimited[j].hasStartedFalling = false;
                        fireBallsUnlimited[j].spawnClock.restart();

                        testEnemy4[i].canFire = false;
                        testEnemy4[i].fireballCooldown.restart(); // Start cooldown
                        break;
                    }
                }
            }

            if (!testEnemy4[i].canFire && testEnemy4[i].fireballCooldown.getElapsedTime().asSeconds() >= 4.0f)
            {
                testEnemy4[i].canFire = true;
            }

            // Apply final screen position
            testEnemy4[i].changePosition(Vector2f(screenX, screenY));
        }

        if (isImmune)
        {
            immunityTimer += deltaTime;
            if (immunityTimer >= 3.f)
            {
                isImmune = false;
                immunityTimer = 0 ;
            }
        }

        string timeString = (minutes < 10 ? "0" : "") + std::to_string(minutes) +
                            "::" +
                            (seconds < 10 ? "0" : "") + std::to_string(seconds);
        ringsElapsedTime += deltaTime;

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed)
                window.close();

            // Mechanics What to Happen when Mouse Pressed
            if (event.type == Event::MouseButtonPressed)
            {
                // Mouse Pixels Input Where Clicked
                Vector2i mousePosition = Mouse::getPosition(window);
                cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

                if (event.mouseButton.button == Mouse::Left)
                {
                    cout << " Left Mouse Button Pressed " << endl;
                    if (Back.isClicked(mousePosition))
                    {
                        // Back.ChangeColor();
                        cout << "Back is clicked" << endl;
                        showLeaderBoard = false;
                        menu = true;
                        return;
                    }
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::P))
            {
                if (!pKeyPressed)
                {
                    mode++;
                    if (mode > 3)
                    {

                        mode = 1;
                    }
                    if (mode != 2)
                    {
                        first_fly_activation = false;
                        tales.flyactivated = false;
                    }
                    pKeyPressed = true;
                }
            }
            else
            {
                pKeyPressed = false; // Reset when key is released
            }
        }
        k_time += deltaTime;
        if (k_time >= switchTimerKnucles && mode == 3)
        {
            mode = 1;
            k_time = 0; // correct: reset k_time
        }

        t_time += deltaTime;
        if (t_time >= switchTimerTales && mode == 2)
        {
            mode = 1;
            t_time = 0; // correct: reset t_time
            tales.flyactivated = false;
            first_fly_activation = false;
            tales.changeImage("jumpTales.png");
            tales.changeScale({1.2f, 1.2f});
            tales.setIntRect(0 * 45, 0, 40, 35);
        }

        float currentSpeed = sonicSpeed;

        // Movement Input
        bool movingLeft = Keyboard::isKeyPressed(Keyboard::A);
        bool movingRight = Keyboard::isKeyPressed(Keyboard::D);

        if (mode == 2)
        {
            if (!first_fly_activation)
            {
                sonic.changePosition({sonic.getLeft(), sonic.getTop() - 100});
                first_fly_activation = true;
            }
            tales.flyactivated = true;
            tales.changeImage("fly_l.png");
        }
        // Handle movement
        if (movingLeft && sonic.getLeft() > 50)
        {
            velocityX = -currentSpeed; // Use currentSpeed for movement
            if (isJumping)
            {
                currentFrame = 3;
            }
            else
            {
                currentFrame = 2;
            }

            sonic.changePosition(Vector2f(sonic.getLeft() - currentSpeed, sonic.getTop()));
            prev = "left";
        }
        else if (movingRight && sonic.getRight() < screenResolution.x - 50)
        {
            velocityX = currentSpeed; // Use currentSpeed for movement
            if (isJumping)
            {
                currentFrame = 6;
            }
            else
            {
                currentFrame = 5;
            }
            sonic.changePosition(Vector2f(sonic.getLeft() + currentSpeed, sonic.getTop()));

            prev = "right";
        }
        else
        {
            velocityX = 0.0f;
            if (isJumping && prev == "left")
            {
                currentFrame = 3;
            }
            else if (isJumping && prev == "right")
            {
                currentFrame = 6;
            }
            else
            {
                currentFrame = (prev == "left" ? 1 : 4);
            }
        }

        if (!tales.flyactivated)
        {
            if (Keyboard::isKeyPressed(Keyboard::Space) && (!isJumping) && fallToGround == false)
            {
                jumpSound.play();

                isJumping = true;
                jumpVelocity = 497.0f; // Increased initial jump velocity for faster jump

                if (strcmp(prev.c_str(), "left") == 0)
                {
                    currentFrame = 6;
                }
                else
                {
                    currentFrame = 3;
                }
            }

            if (isJumping)
            {

                jumpVelocity -= 1100.0f * deltaTime; // Increased gravity effect for higher jump
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() - jumpVelocity * deltaTime));
            }
            // If Sonic is not jumping and no collision with ground or platform, then fall
            if (!isJumping)
            {
                bool isOnGround = false;

                // Compute center-bottom of Sonic
                float centerX = sonic.getLeft() + (hitbox.getWidth() / 2.0f);
                float bottomY = sonic.getTop() + hitbox.getHeight();

                // Convert to tile indices
                int rowBelow = static_cast<int>((bottomY) / 50) + 1; // +1 to check tile *just* below the bottom
                int colCenter = static_cast<int>((centerX + scrollX) / 50);

                if (rowBelow >= 0 && rowBelow < rows &&
                    colCenter >= 0 && colCenter < cols)
                {
                    char tileBelow = screenLayout[rowBelow][colCenter];

                    if (tileBelow == 'B' || tileBelow == 'U' || tileBelow == 'E')
                    {
                        // Scout << "Standing at [" << rowBelow << "][" << colCenter << "] on tile '" << tileBelow << "'\n";
                        isOnGround = true;

                        // Change sprite back to idle/run based on direction & speed
                        if (prev == "left")
                        {
                            currentFrame = 2;
                        }
                        else
                        {
                            currentFrame = 5;
                        }
                    }
                }

                // If not on ground, start falling
                if (!isOnGround)
                {
                    isJumping = true;
                    jumpVelocity = 0.0f; // Begin fall
                }
            }

            if (sonic.getTop() >= 497 + 80)
            { // Adjusted ground level for Sonic to come back to 497

                sonic.changePosition(Vector2f(sonic.getLeft(), 497 + 80));
                isJumping = false;
                jumpVelocity = 0.0f;

                // Reset to previous image after jump
                if (prev == "left")
                {
                    currentFrame = 1;
                }
                else
                {
                    currentFrame = 4;
                }
            }

            float sonicX = sonic.getLeft();

            if (sonicX < 250 && velocityX < 0)
            { // Sonic is trying to move left past x = 250
                if (scrollX > 0)
                {
                    scrollX -= speed * deltaTime;

                    currentFrame = (isJumping) ? 3 : 2;

                    sonic.changePosition(Vector2f(250, sonic.getTop()));
                }
                else
                {

                    currentFrame = (isJumping) ? 3 : 2;

                    sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                }
            }
            else if (sonicX > 800 && velocityX > 0)
            { // Sonic is trying to move right past x = 800
                if (scrollX < cols * 50 - screenResolution.x)
                {
                    scrollX += speed * deltaTime;
                    if (isJumping)
                    {
                        currentFrame = 6;
                    }
                    else
                    {
                        currentFrame = 5;
                    }

                    sonic.changePosition(Vector2f(800, sonic.getTop()));
                }
                else
                {
                    if (isJumping)
                    {
                        currentFrame = 6;
                    }
                    else
                    {
                        currentFrame = 5;
                    }

                    sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                }

                currentFrame = (isJumping) ? 6 : 5;
            }
            else
            {

                sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                if (velocityX > 0)
                {
                    if (isJumping)
                    {

                        currentFrame = 6;
                    }
                    else
                    {

                        currentFrame = 5;
                    }
                }
                else if (velocityX < 0)
                {
                    if (isJumping)
                    {
                        currentFrame = 3;
                    }
                    else
                    {
                        currentFrame = 2;
                    }
                }
                else
                {
                    if (isJumping)
                    {
                        currentFrame = prev == "left" ? 3 : 6;
                    }
                    else
                    {
                        currentFrame = prev == "left" ? 1 : 4;
                    }
                }
            }
        }
        else
        {
            bool movingUp = Keyboard::isKeyPressed(Keyboard::W);
            bool movingDown = Keyboard::isKeyPressed(Keyboard::S);

            float flySpeed = 200.0f; // Flying speed

            float s_fly_x = sonic.getLeft();

            if (s_fly_x < 250)
            { // Sonic is trying to move left past x = 250
                mode = 1;
                t_time = 0; // correct: reset t_time
                tales.flyactivated = false;
                first_fly_activation = false;
                tales.changeImage("jumpTales.png");
                tales.changeScale({1.2f, 1.2f});
                tales.setIntRect(0 * 45, 0, 40, 35);
            }
            else if (s_fly_x > 800)
            { // Sonic is trying to move right past x = 800
                mode = 1;
                t_time = 0; // correct: reset t_time
                tales.flyactivated = false;
                first_fly_activation = false;
                tales.changeImage("jumpTales.png");
                tales.changeScale({1.2f, 1.2f});
                tales.setIntRect(0 * 45, 0, 40, 35);
            }

            if (movingUp)
            {
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() - flySpeed * deltaTime));
            }
            if (movingDown)
            {
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() + flySpeed * deltaTime));
            }
        }

        animateRings(ringsElapsedTime, animationSpeed, ringFrame, ring1);
        // Iterate through all the enemies and animate them
        for (int i = 0; i < e1; i++)
        {
            animateCrabMeat(deltaTime, testEnemy1[i]);
        }

        for (int i = 0; i < e2; i++)
        {
            animateMotoBug(deltaTime, testEnemy2[i]);
        }

        for (int i = 0; i < e3; i++)
        {
            animateBeeBot(deltaTime, testEnemy3[i]);
        }

        for (int i = 0; i < e4; i++)
        {
            animateBatBrain(deltaTime, testEnemy4[i]);
        }

        if (currentFrame == 1)
        {
            sonic.changeImage("0left_still.png");

            tales.state = "still";
            knuckles.state = "left_still";

            tales.changeImage("tales_stilll.png");
            tales.setIntRect(0, 0, 86, 86);
            tales.changeScale({0.8f, 0.8f});

            knuckles.changeImage("k_still_left.png");
            knuckles.setIntRect(0, 0, 100, 100);
            knuckles.changeScale({1.f, 1.f});

            tales.changePosition({sonic.getLeft() + 10, sonic.getTop() - 13});
            knuckles.changePosition({sonic.getLeft() + 37, sonic.getTop() - 13});

            tales.frame = 0;
            knuckles.frame = 0;
        }
        else if (currentFrame == 2)
        {
            sonic.changeImage("0jog_left.png");

            if (knuckles.state != "left")
            {
                tales.setIntRect(0 * 45, 0, 45, 35);
            }
            tales.state = "left";
            if (knuckles.state != "left")
            {
                knuckles.setIntRect(0, 5, 35, 30);
            }
            knuckles.state = "left";

            knuckles.changeImage("k_run_left.png");
            knuckles.changeScale({1.8f, 1.8f});

            tales.changePosition({sonic.getLeft() + 19, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() + 37, sonic.getTop() - 2});
        }
        else if (currentFrame == 3)
        {
            sonic.changeImage("0upL.png");

            tales.state = "jump";
            knuckles.state = "jump_left";

            knuckles.changeImage("k_jump_left.png");
            knuckles.changeScale({0.85f, 0.85f});

            tales.changePosition({sonic.getLeft() + 12, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() + 30, sonic.getTop() - 4});
        }
        else if (currentFrame == 4)
        {
            sonic.changeImage("0right_still.png");

            tales.state = "still";
            knuckles.state = "right_still";

            tales.changeImage("tales_still.png");
            tales.setIntRect(0, 0, 86, 86);
            tales.changeScale({0.8f, 0.8f});

            knuckles.changeImage("k_still_right.png");
            knuckles.setIntRect(0, 0, 100, 100);
            knuckles.changeScale({1.f, 1.f});

            tales.changePosition({sonic.getLeft() - 28, sonic.getTop() - 13});
            knuckles.changePosition({sonic.getLeft() - 50, sonic.getTop() - 13});

            tales.frame = 0;
            knuckles.frame = 0;
        }
        else if (currentFrame == 5)
        {
            sonic.changeImage("0jog_right.png");
            if (tales.state != "right")
            {
                tales.setIntRect(2 * 45, 0, 45, 35);
            }
            tales.state = "right";
            if (knuckles.state != "right")
            {
                knuckles.setIntRect(0, 5, 35, 30);
            }
            knuckles.state = "right";

            knuckles.changeImage("k_run_right.png");
            knuckles.changeScale({1.8f, 1.8f});
            // knuckles.setIntRect(0, 5, 35, 30);

            tales.changePosition({sonic.getLeft() - 37, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() - 55, sonic.getTop() - 5});
        }
        else if (currentFrame == 6)
        {
            sonic.changeImage("0upR.png");

            tales.state = "jump";
            knuckles.state = "jump_right";

            knuckles.changeImage("k_jump_right.png");
            knuckles.changeScale({0.85f, 0.85f});

            tales.changePosition({sonic.getLeft() - 20, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() - 45, sonic.getTop() - 5});
        }

        knuckles.update(deltaTime);
        tales.update(deltaTime);
        sonic.update(deltaTime, velocityX, isJumping);

        for (int i = 0; i < 100; i++)
        {
            if (!fireBallsUnlimited[i].isDestroyed)
            {
                // Move in world space (don't apply scrollX here)
                Vector2f worldPos = fireBallsUnlimited[i].worldPosition; // You'll need to store world position separately
                worldPos.y += 100.f * deltaTime;

                // Destroy if it's too far vertically in the world
                if (worldPos.y > 700.f) // or some max level height
                {
                    fireBallsUnlimited[i].isDestroyed = true;
                    fireBallsUnlimited[i].sprite.setPosition(-1000, -1000);
                }
                else
                {
                    fireBallsUnlimited[i].worldPosition = worldPos;
                    Vector2f screenPos(worldPos.x - scrollX, worldPos.y);
                    fireBallsUnlimited[i].sprite.setPosition(screenPos);
                }
            }
        }

        // ---- For testEnemy1 ----
        for (int i = 0; i < e1; i++)
        {
            if (!testEnemy1[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy1[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }

                    isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy2 ----
        for (int i = 0; i < e2; i++)
        {
            if (!testEnemy2[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy2[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy3 ----
        for (int i = 0; i < e3; i++)
        {
            if (!testEnemy3[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy3[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy4 ----
        for (int i = 0; i < e4; i++)
        {
            if (!testEnemy4[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy4[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Sonic hit enemy4 from side -> lost 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Tales hit enemy4 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {

                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy4 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        window.clear(Color::White);
        background.draw(window);

        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                float px = j * 50 - scrollX;
                float py = i * 50;

                if (px + 50 < 0 || px > screenResolution.x)
                    continue;

                switch (screenLayout[i][j])
                {
                case 'B': // Wall (Ground)
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (tales.flyactivated)
                    {
                        if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                            playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                        {
                            if (playerTop > obstacleBottom && playerBottom < obstacleBottom)
                            {
                                tales.changePosition({sonic.getLeft(), sonic.getTop() + 2});
                            }
                            // From above (player landing on wall)
                            else if (playerBottom > obstacleTop && playerTop < obstacleTop)
                            {
                                tales.changePosition({sonic.getLeft(), sonic.getTop() - 1});
                            }
                            // From left (player moving right into wall)
                            else if (playerRight > obstacleLeft && playerLeft < obstacleLeft)
                            {
                                tales.changePosition({sonic.getLeft() - 1, sonic.getTop()});
                            }
                            // From right (player moving left into wall)
                            else if (playerLeft < obstacleRight && playerRight > obstacleRight)
                            {
                                tales.changePosition({sonic.getLeft() + 1, sonic.getTop()});
                            }
                        }
                    }
                    else
                    {
                        if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                            playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                        {
                            if (playerTop > obstacleBottom && playerBottom < obstacleBottom)
                            {
                                fallToGround = false;
                                isJumping = false;
                                sonic.changePosition({sonic.getLeft(), sonic.getTop() + 2});
                            }
                            // From above (player landing on wall)
                            else if (playerBottom > obstacleTop && playerTop < obstacleTop)
                            {
                                fallToGround = false;
                                isJumping = false;
                                sonic.changePosition({sonic.getLeft(), sonic.getTop() - 1});
                            }
                            // From left (player moving right into wall)
                            else if (playerRight > obstacleLeft && playerLeft < obstacleLeft)
                            {

                                if (mode == 3)
                                {
                                    screenLayout[i][j] = ' ';
                                    continue;
                                }

                                isJumping = false;
                                sonic.changePosition({sonic.getLeft() - 1, sonic.getTop()});
                            }
                            // From right (player moving left into wall)
                            else if (playerLeft < obstacleRight && playerRight > obstacleRight)
                            {
                                if (mode == 3)
                                {

                                    screenLayout[i][j] = ' ';
                                    continue;
                                }

                                isJumping = false;
                                sonic.changePosition({sonic.getLeft() + 1, sonic.getTop()});
                            }
                        }
                    }

                    brick1.changePosition(Vector2f(px, py));
                    brick1.draw(window);
                    break;
                }
                case 'U': // Platform (Collision only from above)
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                        playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                    {
                        // From above (player landing on wall)
                        if (playerBottom > obstacleTop && playerTop < obstacleTop)
                        {

                            isJumping = false;
                            sonic.changePosition({sonic.getLeft(), sonic.getTop() - 5});
                        }
                    }

                    brick2.changePosition(Vector2f(px, py));
                    brick2.draw(window);
                    break;
                }

                case 'S':
                {
                    float spikeLeft = j * 50 - scrollX;
                    float spikeTop = i * 50;
                    float spikeRight = spikeLeft + 50;
                    float spikeBottom = spikeTop + 50;

                    FloatRect spikeBounds(spikeLeft, spikeTop, 50, 50);
                    FloatRect playerBounds = hitbox.getGlobalBond();

                    if (spikeBounds.intersects(playerBounds) && !isImmune)
                    {
                        sonic.takeDamage(10); // Reduce health by 10
                        isImmune = true;
                        immunityTimer = 1.0f; // 1 second of immunity
                    }
                    spike.changePosition(Vector2f(px, py));
                    spike.draw(window);
                    break;
                }
                case 'E': // Last and second last row treated as platforms
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                        playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                    {
                        // From above (player landing on wall)
                        if (playerBottom > obstacleTop && playerTop < obstacleTop)
                        {

                            isJumping = false;
                            sonic.changePosition({sonic.getLeft(), sonic.getTop() - 5});
                        }
                    }

                    brick2.changePosition(Vector2f(px, py));
                    brick2.draw(window);
                    break;
                }
                case 'R': // Ring
                    if (!ringCollected[i][j])
                    {
                        // Same as before, handle ring collection logic here
                        FloatRect ringBounds(px + 15, py + 20, 19, 19);  // Ring size
                        FloatRect playerBounds = hitbox.getGlobalBond(); // Or sonic.getSprite().getGlobalBounds()

                        if (ringBounds.intersects(playerBounds))
                        {
                            // To play the ring sound
                            ringSound.play();

                            ringCollected[i][j] = true; // Mark ring as collected
                            sonic.addRings();           // Increase score/rings if you have such a function
                        }
                        else
                        {
                            ring1.changePosition(Vector2f(px + 15, py + 20));
                            ring1.draw(window);
                        }
                    }
                    break;
                }
            }
        }

        // displayText("Tails Position: (" + to_string(static_cast<int>(tales.getLeft())) + ", " + to_string(static_cast<int>(tales.getTop())) + ")", Vector2f(20.f, 170.f), window, 30, Color::White);
        // displayText("Sonic Position: (" + to_string(static_cast<int>(sonic.getLeft())) + ", " + to_string(static_cast<int>(sonic.getTop())) + ")", Vector2f(20.f, 210.f), window, 30, Color::White);

        if (isImmune)
        {
            displayText("IS IMMUNE", {screenResolution.x / 2 - 100, 50}, window, 40, Color::Magenta);
        }

        // hitbox.draw(window);
        if (!tales.flyactivated)
        {
            knuckles.draw(window);
            tales.draw(window);
            sonic.draw(window);
        }
        else
        {
            if (movingLeft)
            {
                tales.changeImage("fly_l.png");
                tales.changeScale({0.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (movingRight)
            {
                tales.changeImage("fly_r.png");
                tales.changeScale({.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (currentFrame == 1 || currentFrame == 3)
            {
                tales.changeImage("fly_l.png");
                tales.changeScale({0.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (currentFrame == 4 || currentFrame == 6)
            {
                tales.changeImage("fly_r.png");
                tales.changeScale({.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            tales.draw(window);
        }

        for (int i = 0; i < e1; i++)
        {
            if (testEnemy1[i].isAlive)
                testEnemy1[i].draw(window);
        }

        for (int i = 0; i < e2; i++)
        {
            if (testEnemy2[i].isAlive)
                testEnemy2[i].draw(window);
        }

        for (int i = 0; i < e3; i++)
        {
            if (testEnemy3[i].isAlive)
                testEnemy3[i].draw(window);
        }

        for (int i = 0; i < e4; i++)
        {
            if (testEnemy4[i].isAlive)
                testEnemy4[i].draw(window);
        }

        for (int i = 0; i < 100; i++)
        {
            if (!fireBallsUnlimited[i].isDestroyed)
            {
                window.draw(fireBallsUnlimited[i].sprite);
            }
        }

        if (minutes < gameOverTimer && !(TotalRingsToBeCollected == sonic.getRingsCollected()) && sonic.hp > 0)
        {
            displayText("Time: " + timeString, Vector2f(20.f, 10.f), window, 30, Color(16, 124, 43));
            displayText("Rings: " + to_string(sonic.getRingsCollected()), Vector2f(20.f, 50.f), window, 30, Color::Yellow);
            displayText(" / " + to_string(TotalRingsToBeCollected), Vector2f(120.f, 50.f), window, 30, Color::Green);
            displayText("Score: " + to_string(sonic.getScore()), Vector2f(20.f, 90.f), window, 30, Color::White);
        }
        else if (TotalRingsToBeCollected >= sonic.getRingsCollected())
        {
            showLevelUpScreen(window, 3.5f); // 5 seconds level-up screen

            return;
        }
        else if (sonic.hp <= 0)
        {
            showGameOverScreen(window, sonic, 3.5f); // 5 seconds game over screen

            return;
        }
        else // Time over but not game over
        {
            showTimeOverScreen(window, sonic, 3.5f); // show for 5 seconds
            return;
        }

        if (mode == 1)
            displayText("Sonic Control", {700, 5}, window, 30, Color::Blue);
        else if (mode == 2)
            displayText("Tails Control", {700, 5}, window, 30, Color::Yellow);
        else
            displayText("Knuckles Control", {700, 5}, window, 30, Color::Red);

        Back.draw(window);
        window.draw(hpBarBackground);
        window.draw(hpBar);

        window.display();
    }

    delete[] testEnemy1;
    delete[] testEnemy2;
    delete[] testEnemy3;
    delete[] testEnemy4;
}

void countEnemies_3(const char screenLayout[][300], int rows, int &e1, int &e2, int &e3, int &e4)
{
    e1 = e2 = e3 = e4 = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < 300; j++)
        {
            switch (screenLayout[i][j])
            {
            case '1':
                e1++;
                break;
            case '2':
                e2++;
                break;
            case '3':
                e3++;
                break;
            case '4':
                e4++;
                break;
            }
        }
    }
}

void spawnEnemies_3(char screenLayout[][300], int rows, int cols,
                  enemy testEnemy1[], int e1, int &idx1,
                  enemy testEnemy2[], int e2, int &idx2,
                  enemy testEnemy3[], int e3, int &idx3,
                  e4_type testEnemy4[], int e4, int &idx4)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vector2f position(j * 50, i * 50);
            char tile = screenLayout[i][j];

            if (tile == '1' && idx1 < e1)
            {
                testEnemy1[idx1].changeImage("CrabMeat.png");
                testEnemy1[idx1].changeScale({1.45f, 1.3f});
                testEnemy1[idx1].changePosition(position);
                testEnemy1[idx1].frameCount = 8;
                testEnemy1[idx1].frameTime = 0.15f;
                testEnemy1[idx1].x = position.x;
                testEnemy1[idx1].y = position.y;
                idx1++;
            }
            else if (tile == '2' && idx2 < e2)
            {
                testEnemy2[idx2].changeImage("motoBug.png");
                testEnemy2[idx2].changeScale({1.12f, 1.12f});
                testEnemy2[idx2].changePosition(position);
                testEnemy2[idx2].frameCount = 4;
                testEnemy2[idx2].frameTime = 0.15f;
                testEnemy2[idx2].x = position.x;
                testEnemy2[idx2].y = position.y;
                idx2++;
            }
            else if (tile == '3' && idx3 < e3)
            {
                testEnemy3[idx3].changeImage("beeBot.png");
                testEnemy3[idx3].changeScale({2.f, 2.f});
                testEnemy3[idx3].changePosition(position);
                testEnemy3[idx3].frameCount = 5;
                testEnemy3[idx3].frameTime = 0.15f;
                testEnemy3[idx3].x = position.x;
                testEnemy3[idx3].y = position.y;
                idx3++;
            }
            else if (tile == '4' && idx4 < e4)
            {
                testEnemy4[idx4].changeImage("batBrain.png");
                testEnemy4[idx4].changeScale({1.5f, 1.5f});
                testEnemy4[idx4].changePosition(position);
                testEnemy4[idx4].frameCount = 8;
                testEnemy4[idx4].frameTime = 0.15f;
                testEnemy4[idx4].x = position.x;
                testEnemy4[idx4].y = position.y;
                idx4++;
            }

            if (tile >= '1' && tile <= '4')
            {
                screenLayout[i][j] = ' ';
            }
        }
    }
}


int calculateRings_3(const char lvl[12][300])
{
    int rings = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            if (lvl[i][j] == 'R')
            {
                // cout << "Rings Found at " << i << " " << j << endl;
                rings++;
            }
        }
    }

    return rings;
}

void levelThree(RenderWindow &window, Event &event)
{

    Object background("bg3.jpg", Vector2f(0, 0), Vector2f(2.f, 1.5f));
    const int rows = 12;
    const int cols = 300;
    // enemy testEnemy1("CrabMeat.png", 270, 101, 1.45f, 1.3f, 8, 54.f, 41);
    // enemy testEnemy2("motoBug.png", 101, 100, 1.12f, 1.12f, 4, 43.5f, 41);
    // enemy testEnemy3("beeBot.png", 101, 400, 2.f, 2.f, 5, 34.f, 20);
    // enemy testEnemy4("batBrain.png", 551, 100, 1.5f, 1.5f, 5, 65.f, 40);

    char screenLayout[rows][cols] = {

        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B','B', 'B', 'B', 'E', 'U', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B','B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B','B', 'E', 'B', 'B', 'B', 'B', 'B', 'B'},
        {'B', ' ', ' ', ' ', 'B', 'E', 'U', 'U', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', 'U', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ',' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', '3', ' ', ' ', ' ', '3', '4', ' ', 'B', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', 'B', ' ', ' ', 'B', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ',' ', ' ', '3', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', 'E',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '3', ' ', ' ', ' ', ' ', 'B', 'B', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', '1', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', ' ', ' ', '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B',' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R'},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R',' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', 'E', 'U', 'E', 'U', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B','U', 'U', 'U', 'U', 'E', 'U', 'E', 'U', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', 'R', ' ', 'R',' ', '1', 'R', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'U', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', ' ', ' ', 'U', ' ', ' ', ' ', 'U', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'B',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'U', ' ', ' ', ' ', 'U', '1', 'R'},
        {'E', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'U', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', 'R', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'E', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R','U', 'U', 'U', 'E', 'U', 'U', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'R', ' ', ' ', ' ', '1', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', 'B', 'B', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'R', ' ', ' ', 'U', 'U', 'U', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B',' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', ' ', 'B', 'B', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ','U', 'U', 'R', ' ', ' ', 'U', 'U', 'U'},
        {'B', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', 'R', ' ', 'R', ' ', 'R', ' ', 'R', ' ', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'S', 'U',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'U', 'S', 'U', ' ', ' ', ' ', ' ', 'B', 'S', 'B', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', 'E', ' ', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', ' ', 'U', 'U', 'U', 'U', 'R', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', 'B', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B',' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'U', 'B', 'B', ' ', ' ', 'U', 'U', 'U', 'U', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', 'R', ' ', ' ', ' ', ' ', ' '},
        {'B', ' ', ' ', ' ', 'R', 'R', 'R', ' ', ' ', 'R', ' ', ' ', 'R', 'R', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', 'R', ' ', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', ' ', 'R', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '2', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', '2', 'B', 'B', ' ', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'R', 'R', 'R', 'R', ' ', ' ', 'R', 'B', ' ', ' ', '2', ' ', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' ', 'R', ' ', 'B', 'B', '2', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', ' ', '1', 'R', 'R', ' ', '2', 'B', 'B', ' ', '1', 'R', 'R', 'R', 'R', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', ' ', 'B', 'B', 'R', 'R', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'B', 'B', ' ', 'B', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', 'B',' ', 'R', ' ', 'B', 'B', '2', ' ', ' ', ' ', ' ', ' ', ' ', 'B', 'B', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', ' ', ' ', ' ', '1', 'R', 'R', 'R', 'R',' ', 'B', 'B', 'R', 'R', ' ', ' ', ' '},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'S','B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'S', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'S', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'S', 'B', ' ', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B','S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'S', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B','B', 'B', 'B', 'B', 'B', 'S', 'S', 'B'},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B','B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B','B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'U', 'B', 'B', 'B', 'B', 'B', 'B','B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'}};
 
        bool ringCollected[rows][cols] = {false}; // Initially, all rings are uncollected

    int e1 = 0, e2 = 0, e3 = 0, e4 = 0;
    countEnemies_3(screenLayout, 12, e1, e2, e3, e4);

    // cout << e1 << " " << e2 << " " << e3 << " " << e4 << endl;

    enemy *testEnemy1 = new enemy[e1];
    enemy *testEnemy2 = new enemy[e2];
    enemy *testEnemy3 = new enemy[e3];
    e4_type *testEnemy4 = new e4_type[e4];

    int idx1 = 0, idx2 = 0, idx3 = 0, idx4 = 0;

    spawnEnemies_3(screenLayout, rows, cols, testEnemy1, e1, idx1, testEnemy2, e2, idx2, testEnemy3, e3, idx3, testEnemy4, e4, idx4);

    int TotalRingsToBeCollected = calculateRings_3(screenLayout);

    cout << "Total Rings to be collected: " << TotalRingsToBeCollected << endl;

    float scrollX = 0.0f;
    float speed = 300.0f;
    int currentFrame = 4;
    string prev = "right";

    Obstacle brick1("brick2.png", 0, 0, 0.8f, 0.8f);
    Obstacle brick2("brick1.png", 0, 0, 0.8f, 0.8f);
    Obstacle brick3("brick3.png", 0, 0, 0.8f, 0.8f);
    Obstacle spike("spike.png", 0, 0, 0.75f, 0.75f);

    Ring ring1("ring.png", 0, 0, 1.25f, 1.25f);

    Sonic sonic(100, 100, 1.28f, 1.28f);
    Tales tales(100, 100, 1.5f, 1.5f);
    Knuckles knuckles(100, 100, 1.28f, 1.28f);

    Hitbox hitbox("0hit_box.png", 0, 0, 1.1f, 1.1f);

    // Test loading multiple enemy instances

    Clock deltaClock;
    Clock ScreenClockTimer;
    int ringFrame = 0;
    float animationSpeed = 0.15f;
    float ringsElapsedTime = 0.0f;
    float velocityX = 0;

    float sonicSpeed = 0.7f;
    float runSpeedMultiplier = 0.9f;

    ring1.setIntRect(0, 0, 19, 19);

    static bool isJumping = false;
    static float jumpVelocity = 0.0f;

    Button Back(" BACK ", Vector2f(920, 0), Color::White, 30);
    Back.ChangeColor();

    SoundBuffer jumpBuffer;
    Sound jumpSound;

    if (!jumpBuffer.loadFromFile("audio/Jump.wav"))
    {
        cerr << "Failed to load jump sound!" << std::endl;
        exit(EXIT_FAILURE);
    }
    jumpSound.setBuffer(jumpBuffer);
    jumpSound.setLoop(false); // Play only once per press
    jumpSound.setVolume(50);  // Set volume to 60% (default is 100%)

    SoundBuffer ringBuffer;
    Sound ringSound;

    // Load the ring sound
    if (!ringBuffer.loadFromFile("audio/Ring.wav"))
    {
        cerr << "Failed to load ring sound!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set up the sound
    ringSound.setBuffer(ringBuffer);
    ringSound.setLoop(false); // Play only once
    ringSound.setVolume(50);  // Set volume to 50% (default is 100%)

    float gameOverTimer = 2;
    bool gameOver = false;

    bool isImmune = false;
    float immunityTimer = 0.0f;

    bool fallToGround = false;

    // Initialize HP bar
    RectangleShape hpBarBackground(Vector2f(280.f, 20.f));
    hpBarBackground.setFillColor(Color::Red);
    hpBarBackground.setPosition(400.f, 15.f);

    RectangleShape hpBar(Vector2f(200.f, 20.f));
    hpBar.setFillColor(Color::Green);
    hpBar.setPosition(400.f, 15.f);

    fireBall fireBallsUnlimited[100];

    int ballsLaunched = 0;

    int mode = 1;             // Starting value
    bool pKeyPressed = false; // To avoid multiple increments on one press

    float switchTimerKnucles = 4;
    float k_time = 0;
    float switchTimerTales = 7;
    float t_time = 0;

    bool first_fly_activation = false;

    while (window.isOpen())
    {
        // cout << isJumping << " " << fallToGround<< endl;

        hpBar.setSize(Vector2f(200.f * (sonic.hp / 100.f), 20.f));

        if (tales.flyactivated)
        {
            hitbox.copyCordinates(sonic, 10);
        }
        else
        {
            hitbox.copyCordinates(sonic);
        }

        float deltaTime = deltaClock.restart().asSeconds();
        int totalSecondsPassed = static_cast<int>(ScreenClockTimer.getElapsedTime().asSeconds());
        int minutes = totalSecondsPassed / 60;
        int seconds = totalSecondsPassed % 60;
        for (int i = 0; i < e1; i++)
        {
            // Move left
            if (!testEnemy1[i].moveRight)
            {
                testEnemy1[i].movedDistance -= 50 * deltaTime;

                if (testEnemy1[i].movedDistance <= 0)
                {
                    testEnemy1[i].movedDistance = 0;
                    testEnemy1[i].moveRight = true;
                }
                testEnemy1[i].sprite.setScale(1.28f, 1.28f); // Face left
            }
            else // Move right
            {
                testEnemy1[i].movedDistance += 50 * deltaTime;

                if (testEnemy1[i].movedDistance >= 190)
                {
                    testEnemy1[i].movedDistance = 190;
                    testEnemy1[i].moveRight = false;
                }

                testEnemy1[i].sprite.setScale(-1.28f, 1.28f); // Face right (flipped)
            }

            // World position
            float worldX = testEnemy1[i].x + testEnemy1[i].movedDistance;
            float worldY = testEnemy1[i].y;

            // Screen position
            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Check if off-screen and hide
            if (screenX + testEnemy1[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                // Hide the enemy by moving it off-screen
                testEnemy1[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            // Adjust for flipped sprite anchor
            if (testEnemy1[i].sprite.getScale().x < 0)
                screenX += testEnemy1[i].getWidth();

            // Update actual visible position
            testEnemy1[i].changePosition(Vector2f(screenX, screenY));
        }

        for (int i = 0; i < e2; i++)
        {
            float worldX = testEnemy2[i].x;
            float worldY = testEnemy2[i].y;

            // Convert world coordinates to screen coordinates
            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Check if the enemy is off-screen and move it out of view if so
            if (screenX + testEnemy2[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy2[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            bool canMoveRight = true;
            bool canMoveLeft = true;

            // Check if there's a block on the right side of the enemy
            int colRight = static_cast<int>((worldX + testEnemy2[i].getWidth()) / 50); // Right side of the enemy
            int rowBelow = static_cast<int>((worldY + hitbox.getHeight()) / 50);       // Tile just below the enemy

            // Check the tile to the right of the enemy
            if (colRight >= 0 && colRight < cols && rowBelow >= 0 && rowBelow < rows)
            {
                char tileRight = screenLayout[rowBelow][colRight - 1];
                if (tileRight == 'B' || tileRight == 'U' || tileRight == 'E')
                {
                    canMoveRight = false; // Block movement to the right
                }
            }

            // Check if there's a block on the left side of the enemy
            int colLeft = static_cast<int>((worldX) / 50); // Left side of the enemy
            if (colLeft >= 0 && colLeft < cols && rowBelow >= 0 && rowBelow < rows)
            {
                char tileLeft = screenLayout[rowBelow][colLeft];
                if (tileLeft == 'B' || tileLeft == 'U' || tileLeft == 'E')
                {
                    canMoveLeft = false; // Block movement to the left
                }
            }

            // Check if Sonic and enemy are both on the screen
            if (sonic.getLeft() >= 0 && sonic.getRight() <= screenResolution.x &&
                screenX >= 0 && screenX <= screenResolution.x)
            {
                // Move enemy towards Sonic on the x-axis if no obstacle
                float e_S = 70.0f;
                if (canMoveRight && screenX < sonic.getLeft())
                {
                    testEnemy2[i].x += e_S * deltaTime;           // Move right
                    testEnemy2[i].sprite.setScale(-1.28f, 1.28f); // Flip sprite to face right
                }
                else if (canMoveLeft && screenX > sonic.getLeft())
                {
                    testEnemy2[i].x -= e_S * deltaTime; // Move left
                    testEnemy2[i].sprite.setOrigin(testEnemy2[i].getWidth() / 2, testEnemy2[i].getHeight() / 2);

                    if (canMoveRight && screenX < sonic.getLeft())
                    {
                        testEnemy2[i].x += e_S * deltaTime;
                        testEnemy2[i].sprite.setScale(-1.28f, 1.28f);
                    }
                    else if (canMoveLeft && screenX > sonic.getLeft())
                    {
                        testEnemy2[i].x -= e_S * deltaTime;
                        testEnemy2[i].sprite.setScale(1.28f, 1.28f);
                    }

                    testEnemy2[i].sprite.setScale(1.28f, 1.28f); // Flip sprite to face left
                }
            }
            // Update the enemy position on screen
            testEnemy2[i].changePosition(Vector2f(screenX + (canMoveLeft ? -20 : 20), screenY + 30));
        }

        for (int i = 0; i < e3; i++)
        {
            float worldX = testEnemy3[i].x;
            float worldY = testEnemy3[i].y;

            float screenX = worldX - scrollX;
            float screenY = worldY;

            if (screenX + testEnemy3[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy3[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            float targetX = sonic.getLeft() + scrollX;
            float targetY = sonic.getTop();

            float speed = 80.0f;

            float dx = targetX - worldX;
            float dy = targetY - worldY;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > 1.0f)
            {
                float moveX = (dx / dist) * speed * deltaTime;
                float moveY = (dy / dist) * speed * deltaTime;

                testEnemy3[i].x += moveX;
                testEnemy3[i].y += moveY;

                // Flip sprite
                if (moveX > 0)
                    testEnemy3[i].sprite.setScale(-2.f, 2.f); // Facing right
                else if (moveX < 0)
                    testEnemy3[i].sprite.setScale(2.f, 2.f); // Facing left
            }

            screenX = testEnemy3[i].x - scrollX;
            screenY = testEnemy3[i].y;

            testEnemy3[i].changePosition(Vector2f(screenX, screenY)); // You can manually tweak here if needed
        }
        for (int i = 0; i < e4; i++)
        {
            // Handle patrol movement
            if (!testEnemy4[i].moveRight)
            {
                testEnemy4[i].movedDistance -= 50 * deltaTime;

                if (testEnemy4[i].movedDistance <= 0)
                {
                    testEnemy4[i].movedDistance = 0;
                    testEnemy4[i].moveRight = true;
                }
                testEnemy4[i].sprite.setScale(1.28f, 1.28f); // Face left
            }
            else
            {
                testEnemy4[i].movedDistance += 50 * deltaTime;

                if (testEnemy4[i].movedDistance >= 190)
                {
                    testEnemy4[i].movedDistance = 190;
                    testEnemy4[i].moveRight = false;
                }
                testEnemy4[i].sprite.setScale(-1.28f, 1.28f); // Face right (flipped)
            }

            // World and screen positions
            float worldX = testEnemy4[i].x + testEnemy4[i].movedDistance;
            float worldY = testEnemy4[i].y;

            float screenX = worldX - scrollX;
            float screenY = worldY;

            // Skip if off-screen
            if (screenX + testEnemy4[i].getWidth() < 0 || screenX > screenResolution.x)
            {
                testEnemy4[i].changePosition(Vector2f(-999.f, -999.f));
                continue;
            }

            // Adjust position for flipped sprite
            if (testEnemy4[i].sprite.getScale().x < 0)
                screenX += testEnemy4[i].getWidth();

            // Check if Sonic is below the enemy within horizontal range
            float sonicX = sonic.getLeft() + scrollX;
            float sonicY = sonic.getTop();
            float xDistance = abs(sonicX - worldX);

            if (xDistance <= 400 && sonicY > worldY && testEnemy4[i].canFire)
            {
                for (int j = 0; j < 100; j++)
                {
                    if (fireBallsUnlimited[j].isDestroyed)
                    {

                        fireBallsUnlimited[j].worldPosition = Vector2f(worldX, worldY);
                        fireBallsUnlimited[j].sprite.setPosition(worldX - scrollX, worldY + 10);
                        fireBallsUnlimited[j].isDestroyed = false;
                        fireBallsUnlimited[j].hasStartedFalling = false;
                        fireBallsUnlimited[j].spawnClock.restart();

                        testEnemy4[i].canFire = false;
                        testEnemy4[i].fireballCooldown.restart(); // Start cooldown
                        break;
                    }
                }
            }

            if (!testEnemy4[i].canFire && testEnemy4[i].fireballCooldown.getElapsedTime().asSeconds() >= 4.0f)
            {
                testEnemy4[i].canFire = true;
            }

            // Apply final screen position
            testEnemy4[i].changePosition(Vector2f(screenX, screenY));
        }

        if (isImmune)
        {
            immunityTimer += deltaTime;
            if (immunityTimer >= 3.f)
            {
                isImmune = false;
                immunityTimer = 0 ;
            }
        }

        string timeString = (minutes < 10 ? "0" : "") + std::to_string(minutes) +
                            "::" +
                            (seconds < 10 ? "0" : "") + std::to_string(seconds);
        ringsElapsedTime += deltaTime;

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed)
                window.close();

            // Mechanics What to Happen when Mouse Pressed
            if (event.type == Event::MouseButtonPressed)
            {
                // Mouse Pixels Input Where Clicked
                Vector2i mousePosition = Mouse::getPosition(window);
                cout << " Mouse Clicked (" << mousePosition.x << "," << mousePosition.y << ") -- ";

                if (event.mouseButton.button == Mouse::Left)
                {
                    cout << " Left Mouse Button Pressed " << endl;
                    if (Back.isClicked(mousePosition))
                    {
                        // Back.ChangeColor();
                        cout << "Back is clicked" << endl;
                        showLeaderBoard = false;
                        menu = true;
                        return;
                    }
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::P))
            {
                if (!pKeyPressed)
                {
                    mode++;
                    if (mode > 3)
                    {

                        mode = 1;
                    }
                    if (mode != 2)
                    {
                        first_fly_activation = false;
                        tales.flyactivated = false;
                    }
                    pKeyPressed = true;
                }
            }
            else
            {
                pKeyPressed = false; // Reset when key is released
            }
        }
        k_time += deltaTime;
        if (k_time >= switchTimerKnucles && mode == 3)
        {
            mode = 1;
            k_time = 0; // correct: reset k_time
        }

        t_time += deltaTime;
        if (t_time >= switchTimerTales && mode == 2)
        {
            mode = 1;
            t_time = 0; // correct: reset t_time
            tales.flyactivated = false;
            first_fly_activation = false;
            tales.changeImage("jumpTales.png");
            tales.changeScale({1.2f, 1.2f});
            tales.setIntRect(0 * 45, 0, 40, 35);
        }

        float currentSpeed = sonicSpeed;

        // Movement Input
        bool movingLeft = Keyboard::isKeyPressed(Keyboard::A);
        bool movingRight = Keyboard::isKeyPressed(Keyboard::D);

        if (mode == 2)
        {
            if (!first_fly_activation)
            {
                sonic.changePosition({sonic.getLeft(), sonic.getTop() - 100});
                first_fly_activation = true;
            }
            tales.flyactivated = true;
            tales.changeImage("fly_l.png");
        }
        // Handle movement
        if (movingLeft && sonic.getLeft() > 50)
        {
            velocityX = -currentSpeed; // Use currentSpeed for movement
            if (isJumping)
            {
                currentFrame = 3;
            }
            else
            {
                currentFrame = 2;
            }

            sonic.changePosition(Vector2f(sonic.getLeft() - currentSpeed, sonic.getTop()));
            prev = "left";
        }
        else if (movingRight && sonic.getRight() < screenResolution.x - 50)
        {
            velocityX = currentSpeed; // Use currentSpeed for movement
            if (isJumping)
            {
                currentFrame = 6;
            }
            else
            {
                currentFrame = 5;
            }
            sonic.changePosition(Vector2f(sonic.getLeft() + currentSpeed, sonic.getTop()));

            prev = "right";
        }
        else
        {
            velocityX = 0.0f;
            if (isJumping && prev == "left")
            {
                currentFrame = 3;
            }
            else if (isJumping && prev == "right")
            {
                currentFrame = 6;
            }
            else
            {
                currentFrame = (prev == "left" ? 1 : 4);
            }
        }

        if (!tales.flyactivated)
        {
            if (Keyboard::isKeyPressed(Keyboard::Space) && (!isJumping) && fallToGround == false)
            {
                jumpSound.play();

                isJumping = true;
                jumpVelocity = 560.f; // Increased initial jump velocity for faster jump

                if (strcmp(prev.c_str(), "left") == 0)
                {
                    currentFrame = 6;
                }
                else
                {
                    currentFrame = 3;
                }
            }

            if (isJumping)
            {

                jumpVelocity -= 1100.0f * deltaTime; // Increased gravity effect for higher jump
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() - jumpVelocity * deltaTime));
            }
            // If Sonic is not jumping and no collision with ground or platform, then fall
            if (!isJumping)
            {
                bool isOnGround = false;

                // Compute center-bottom of Sonic
                float centerX = sonic.getLeft() + (hitbox.getWidth() / 2.0f);
                float bottomY = sonic.getTop() + hitbox.getHeight();

                // Convert to tile indices
                int rowBelow = static_cast<int>((bottomY) / 50) + 1; // +1 to check tile *just* below the bottom
                int colCenter = static_cast<int>((centerX + scrollX) / 50);

                if (rowBelow >= 0 && rowBelow < rows &&
                    colCenter >= 0 && colCenter < cols)
                {
                    char tileBelow = screenLayout[rowBelow][colCenter];

                    if (tileBelow == 'B' || tileBelow == 'U' || tileBelow == 'E')
                    {
                        // Scout << "Standing at [" << rowBelow << "][" << colCenter << "] on tile '" << tileBelow << "'\n";
                        isOnGround = true;

                        // Change sprite back to idle/run based on direction & speed
                        if (prev == "left")
                        {
                            currentFrame = 2;
                        }
                        else
                        {
                            currentFrame = 5;
                        }
                    }
                }

                // If not on ground, start falling
                if (!isOnGround)
                {
                    isJumping = true;
                    jumpVelocity = 0.0f; // Begin fall
                }
            }

            if (sonic.getTop() >= 497 + 80)
            { // Adjusted ground level for Sonic to come back to 497

                sonic.changePosition(Vector2f(sonic.getLeft(), 497 + 80));
                isJumping = false;
                jumpVelocity = 0.0f;

                // Reset to previous image after jump
                if (prev == "left")
                {
                    currentFrame = 1;
                }
                else
                {
                    currentFrame = 4;
                }
            }

            float sonicX = sonic.getLeft();

            if (sonicX < 250 && velocityX < 0)
            { // Sonic is trying to move left past x = 250
                if (scrollX > 0)
                {
                    scrollX -= speed * deltaTime;

                    currentFrame = (isJumping) ? 3 : 2;

                    sonic.changePosition(Vector2f(250, sonic.getTop()));
                }
                else
                {

                    currentFrame = (isJumping) ? 3 : 2;

                    sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                }
            }
            else if (sonicX > 800 && velocityX > 0)
            { // Sonic is trying to move right past x = 800
                if (scrollX < cols * 50 - screenResolution.x)
                {
                    scrollX += speed * deltaTime;
                    if (isJumping)
                    {
                        currentFrame = 6;
                    }
                    else
                    {
                        currentFrame = 5;
                    }

                    sonic.changePosition(Vector2f(800, sonic.getTop()));
                }
                else
                {
                    if (isJumping)
                    {
                        currentFrame = 6;
                    }
                    else
                    {
                        currentFrame = 5;
                    }

                    sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                }

                currentFrame = (isJumping) ? 6 : 5;
            }
            else
            {

                sonic.moveObject(Vector2f(velocityX * deltaTime, 0), deltaTime, screenResolution);
                if (velocityX > 0)
                {
                    if (isJumping)
                    {

                        currentFrame = 6;
                    }
                    else
                    {

                        currentFrame = 5;
                    }
                }
                else if (velocityX < 0)
                {
                    if (isJumping)
                    {
                        currentFrame = 3;
                    }
                    else
                    {
                        currentFrame = 2;
                    }
                }
                else
                {
                    if (isJumping)
                    {
                        currentFrame = prev == "left" ? 3 : 6;
                    }
                    else
                    {
                        currentFrame = prev == "left" ? 1 : 4;
                    }
                }
            }
        }
        else
        {
            bool movingUp = Keyboard::isKeyPressed(Keyboard::W);
            bool movingDown = Keyboard::isKeyPressed(Keyboard::S);

            float flySpeed = 200.0f; // Flying speed

            float s_fly_x = sonic.getLeft();

            if (s_fly_x < 250)
            { // Sonic is trying to move left past x = 250
                mode = 1;
                t_time = 0; // correct: reset t_time
                tales.flyactivated = false;
                first_fly_activation = false;
                tales.changeImage("jumpTales.png");
                tales.changeScale({1.2f, 1.2f});
                tales.setIntRect(0 * 45, 0, 40, 35);
            }
            else if (s_fly_x > 800)
            { // Sonic is trying to move right past x = 800
                mode = 1;
                t_time = 0; // correct: reset t_time
                tales.flyactivated = false;
                first_fly_activation = false;
                tales.changeImage("jumpTales.png");
                tales.changeScale({1.2f, 1.2f});
                tales.setIntRect(0 * 45, 0, 40, 35);
            }

            if (movingUp)
            {
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() - flySpeed * deltaTime));
            }
            if (movingDown)
            {
                sonic.changePosition(Vector2f(sonic.getLeft(), sonic.getTop() + flySpeed * deltaTime));
            }
        }

        animateRings(ringsElapsedTime, animationSpeed, ringFrame, ring1);
        // Iterate through all the enemies and animate them
        for (int i = 0; i < e1; i++)
        {
            animateCrabMeat(deltaTime, testEnemy1[i]);
        }

        for (int i = 0; i < e2; i++)
        {
            animateMotoBug(deltaTime, testEnemy2[i]);
        }

        for (int i = 0; i < e3; i++)
        {
            animateBeeBot(deltaTime, testEnemy3[i]);
        }

        for (int i = 0; i < e4; i++)
        {
            animateBatBrain(deltaTime, testEnemy4[i]);
        }

        if (currentFrame == 1)
        {
            sonic.changeImage("0left_still.png");

            tales.state = "still";
            knuckles.state = "left_still";

            tales.changeImage("tales_stilll.png");
            tales.setIntRect(0, 0, 86, 86);
            tales.changeScale({0.8f, 0.8f});

            knuckles.changeImage("k_still_left.png");
            knuckles.setIntRect(0, 0, 100, 100);
            knuckles.changeScale({1.f, 1.f});

            tales.changePosition({sonic.getLeft() + 10, sonic.getTop() - 13});
            knuckles.changePosition({sonic.getLeft() + 37, sonic.getTop() - 13});

            tales.frame = 0;
            knuckles.frame = 0;
        }
        else if (currentFrame == 2)
        {
            sonic.changeImage("0jog_left.png");

            if (knuckles.state != "left")
            {
                tales.setIntRect(0 * 45, 0, 45, 35);
            }
            tales.state = "left";
            if (knuckles.state != "left")
            {
                knuckles.setIntRect(0, 5, 35, 30);
            }
            knuckles.state = "left";

            knuckles.changeImage("k_run_left.png");
            knuckles.changeScale({1.8f, 1.8f});

            tales.changePosition({sonic.getLeft() + 19, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() + 37, sonic.getTop() - 2});
        }
        else if (currentFrame == 3)
        {
            sonic.changeImage("0upL.png");

            tales.state = "jump";
            knuckles.state = "jump_left";

            knuckles.changeImage("k_jump_left.png");
            knuckles.changeScale({0.85f, 0.85f});

            tales.changePosition({sonic.getLeft() + 12, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() + 30, sonic.getTop() - 4});
        }
        else if (currentFrame == 4)
        {
            sonic.changeImage("0right_still.png");

            tales.state = "still";
            knuckles.state = "right_still";

            tales.changeImage("tales_still.png");
            tales.setIntRect(0, 0, 86, 86);
            tales.changeScale({0.8f, 0.8f});

            knuckles.changeImage("k_still_right.png");
            knuckles.setIntRect(0, 0, 100, 100);
            knuckles.changeScale({1.f, 1.f});

            tales.changePosition({sonic.getLeft() - 28, sonic.getTop() - 13});
            knuckles.changePosition({sonic.getLeft() - 50, sonic.getTop() - 13});

            tales.frame = 0;
            knuckles.frame = 0;
        }
        else if (currentFrame == 5)
        {
            sonic.changeImage("0jog_right.png");
            if (tales.state != "right")
            {
                tales.setIntRect(2 * 45, 0, 45, 35);
            }
            tales.state = "right";
            if (knuckles.state != "right")
            {
                knuckles.setIntRect(0, 5, 35, 30);
            }
            knuckles.state = "right";

            knuckles.changeImage("k_run_right.png");
            knuckles.changeScale({1.8f, 1.8f});
            // knuckles.setIntRect(0, 5, 35, 30);

            tales.changePosition({sonic.getLeft() - 37, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() - 55, sonic.getTop() - 5});
        }
        else if (currentFrame == 6)
        {
            sonic.changeImage("0upR.png");

            tales.state = "jump";
            knuckles.state = "jump_right";

            knuckles.changeImage("k_jump_right.png");
            knuckles.changeScale({0.85f, 0.85f});

            tales.changePosition({sonic.getLeft() - 20, sonic.getTop()});
            knuckles.changePosition({sonic.getLeft() - 45, sonic.getTop() - 5});
        }

        knuckles.update(deltaTime);
        tales.update(deltaTime);
        sonic.update(deltaTime, velocityX, isJumping);

        for (int i = 0; i < 100; i++)
        {
            if (!fireBallsUnlimited[i].isDestroyed)
            {
                // Move in world space (don't apply scrollX here)
                Vector2f worldPos = fireBallsUnlimited[i].worldPosition; // You'll need to store world position separately
                worldPos.y += 100.f * deltaTime;

                // Destroy if it's too far vertically in the world
                if (worldPos.y > 700.f) // or some max level height
                {
                    fireBallsUnlimited[i].isDestroyed = true;
                    fireBallsUnlimited[i].sprite.setPosition(-1000, -1000);
                }
                else
                {
                    fireBallsUnlimited[i].worldPosition = worldPos;
                    Vector2f screenPos(worldPos.x - scrollX, worldPos.y);
                    fireBallsUnlimited[i].sprite.setPosition(screenPos);
                }
            }
        }

        // ---- For testEnemy1 ----
        for (int i = 0; i < e1; i++)
        {
            if (!testEnemy1[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy1[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }

                    isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy1 from above\n";
                    testEnemy1[i].isAlive = false;
                    testEnemy1[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy2 ----
        for (int i = 0; i < e2; i++)
        {
            if (!testEnemy2[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy2[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy2 from above\n";
                    testEnemy2[i].isAlive = false;
                    testEnemy2[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy3 ----
        for (int i = 0; i < e3; i++)
        {
            if (!testEnemy3[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy3[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy3 from above\n";
                    testEnemy3[i].isAlive = false;
                    testEnemy3[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy3 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        // ---- Same logic for testEnemy4 ----
        for (int i = 0; i < e4; i++)
        {
            if (!testEnemy4[i].isAlive)
                continue;

            FloatRect enemyBounds = testEnemy4[i].getGlobalBond();
            FloatRect sonicBounds = hitbox.getGlobalBond();
            FloatRect talesBounds = tales.getGlobalBond();
            FloatRect knucklesBounds = knuckles.getGlobalBond();

            if (enemyBounds.intersects(sonicBounds))
            {
                float sonicBottom = sonicBounds.top + sonicBounds.height;
                float enemyTop = enemyBounds.top;

                if (sonicBottom - enemyTop <= 10.0f)
                {
                    cout << "Sonic killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Sonic hit enemy4 from side -> lost 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(talesBounds))
            {
                float talesBottom = talesBounds.top + talesBounds.height;
                float enemyTop = enemyBounds.top;

                if (talesBottom - enemyTop <= 10.0f)
                {
                    cout << "Tales killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {
                    if (!isImmune)
                    {
                        cout << "Tales hit enemy4 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }

            if (enemyBounds.intersects(knucklesBounds))
            {
                float knucklesBottom = knucklesBounds.top + knucklesBounds.height;
                float enemyTop = enemyBounds.top;

                if (knucklesBottom - enemyTop <= 10.0f)
                {
                    cout << "Knuckles killed enemy4 from above\n";
                    testEnemy4[i].isAlive = false;
                    testEnemy4[i].changePosition(Vector2f(-1000, -1000));
                    sonic.addRings(5);
                }
                else
                {

                    if (!isImmune)
                    {
                        cout << "Knuckles hit enemy4 from side -> Sonic loses 1 HP\n";
                        sonic.takeDamage(5);
                    }isImmune = true;
                }
                continue;
            }
        }

        window.clear(Color::White);
        background.draw(window);

        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                float px = j * 50 - scrollX;
                float py = i * 50;

                if (px + 50 < 0 || px > screenResolution.x)
                    continue;

                switch (screenLayout[i][j])
                {
                case 'B': // Wall (Ground)
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (tales.flyactivated)
                    {
                        if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                            playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                        {
                            if (playerTop > obstacleBottom && playerBottom < obstacleBottom)
                            {
                                tales.changePosition({sonic.getLeft(), sonic.getTop() + 2});
                            }
                            // From above (player landing on wall)
                            else if (playerBottom > obstacleTop && playerTop < obstacleTop)
                            {
                                tales.changePosition({sonic.getLeft(), sonic.getTop() - 1});
                            }
                            // From left (player moving right into wall)
                            else if (playerRight > obstacleLeft && playerLeft < obstacleLeft)
                            {
                                tales.changePosition({sonic.getLeft() - 1, sonic.getTop()});
                            }
                            // From right (player moving left into wall)
                            else if (playerLeft < obstacleRight && playerRight > obstacleRight)
                            {
                                tales.changePosition({sonic.getLeft() + 1, sonic.getTop()});
                            }
                        }
                    }
                    else
                    {
                        if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                            playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                        {
                            if (playerTop > obstacleBottom && playerBottom < obstacleBottom)
                            {
                                fallToGround = false;
                                isJumping = false;
                                sonic.changePosition({sonic.getLeft(), sonic.getTop() + 2});
                            }
                            // From above (player landing on wall)
                            else if (playerBottom > obstacleTop && playerTop < obstacleTop)
                            {
                                fallToGround = false;
                                isJumping = false;
                                sonic.changePosition({sonic.getLeft(), sonic.getTop() - 1});
                            }
                            // From left (player moving right into wall)
                            else if (playerRight > obstacleLeft && playerLeft < obstacleLeft)
                            {

                                if (mode == 3)
                                {
                                    screenLayout[i][j] = ' ';
                                    continue;
                                }

                                isJumping = false;
                                sonic.changePosition({sonic.getLeft() - 1, sonic.getTop()});
                            }
                            // From right (player moving left into wall)
                            else if (playerLeft < obstacleRight && playerRight > obstacleRight)
                            {
                                if (mode == 3)
                                {

                                    screenLayout[i][j] = ' ';
                                    continue;
                                }

                                isJumping = false;
                                sonic.changePosition({sonic.getLeft() + 1, sonic.getTop()});
                            }
                        }
                    }

                    brick1.changePosition(Vector2f(px, py));
                    brick1.draw(window);
                    break;
                }
                case 'U': // Platform (Collision only from above)
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                        playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                    {
                        // From above (player landing on wall)
                        if (playerBottom > obstacleTop && playerTop < obstacleTop)
                        {

                            isJumping = false;
                            sonic.changePosition({sonic.getLeft(), sonic.getTop() - 5});
                        }
                    }

                    brick2.changePosition(Vector2f(px, py));
                    brick2.draw(window);
                    break;
                }

                case 'S':
                {
                    float spikeLeft = j * 50 - scrollX;
                    float spikeTop = i * 50;
                    float spikeRight = spikeLeft + 50;
                    float spikeBottom = spikeTop + 50;

                    FloatRect spikeBounds(spikeLeft, spikeTop, 50, 50);
                    FloatRect playerBounds = hitbox.getGlobalBond();

                    if (spikeBounds.intersects(playerBounds) && !isImmune)
                    {
                        sonic.takeDamage(10); // Reduce health by 10
                        isImmune = true;
                        immunityTimer = 1.0f; // 1 second of immunity
                    }
                    spike.changePosition(Vector2f(px, py));
                    spike.draw(window);
                    break;
                }
                case 'E': // Last and second last row treated as platforms
                {
                    // Obstacle bounds
                    float obstacleLeft = px;
                    float obstacleTop = py;
                    float obstacleRight = px + 50;
                    float obstacleBottom = py + 50;

                    // Player bounds
                    float playerLeft = hitbox.getLeft();
                    float playerTop = hitbox.getTop();
                    float playerRight = hitbox.getRight();
                    float playerBottom = hitbox.getBottom();

                    // Manual collision detection with debug output
                    if (playerRight >= obstacleLeft && playerLeft <= obstacleRight &&
                        playerBottom >= obstacleTop && playerTop <= obstacleBottom)
                    {
                        // From above (player landing on wall)
                        if (playerBottom > obstacleTop && playerTop < obstacleTop)
                        {

                            isJumping = false;
                            sonic.changePosition({sonic.getLeft(), sonic.getTop() - 5});
                        }
                    }

                    brick2.changePosition(Vector2f(px, py));
                    brick2.draw(window);
                    break;
                }
                case 'R': // Ring
                    if (!ringCollected[i][j])
                    {
                        // Same as before, handle ring collection logic here
                        FloatRect ringBounds(px + 15, py + 20, 19, 19);  // Ring size
                        FloatRect playerBounds = hitbox.getGlobalBond(); // Or sonic.getSprite().getGlobalBounds()

                        if (ringBounds.intersects(playerBounds))
                        {
                            // To play the ring sound
                            ringSound.play();

                            ringCollected[i][j] = true; // Mark ring as collected
                            sonic.addRings();           // Increase score/rings if you have such a function
                        }
                        else
                        {
                            ring1.changePosition(Vector2f(px + 15, py + 20));
                            ring1.draw(window);
                        }
                    }
                    break;
                }
            }
        }

        // displayText("Tails Position: (" + to_string(static_cast<int>(tales.getLeft())) + ", " + to_string(static_cast<int>(tales.getTop())) + ")", Vector2f(20.f, 170.f), window, 30, Color::White);
        // displayText("Sonic Position: (" + to_string(static_cast<int>(sonic.getLeft())) + ", " + to_string(static_cast<int>(sonic.getTop())) + ")", Vector2f(20.f, 210.f), window, 30, Color::White);

        if (isImmune)
        {
            displayText("IS IMMUNE", {screenResolution.x / 2 - 100, 50}, window, 40, Color::Magenta);
        }

        // hitbox.draw(window);
        if (!tales.flyactivated)
        {
            knuckles.draw(window);
            tales.draw(window);
            sonic.draw(window);
        }
        else
        {
            if (movingLeft)
            {
                tales.changeImage("fly_l.png");
                tales.changeScale({0.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (movingRight)
            {
                tales.changeImage("fly_r.png");
                tales.changeScale({.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (currentFrame == 1 || currentFrame == 3)
            {
                tales.changeImage("fly_l.png");
                tales.changeScale({0.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            else if (currentFrame == 4 || currentFrame == 6)
            {
                tales.changeImage("fly_r.png");
                tales.changeScale({.6f, .6f});
                tales.setIntRect(0, 0, 300, 300);
            }
            tales.draw(window);
        }

        for (int i = 0; i < e1; i++)
        {
            if (testEnemy1[i].isAlive)
                testEnemy1[i].draw(window);
        }

        for (int i = 0; i < e2; i++)
        {
            if (testEnemy2[i].isAlive)
                testEnemy2[i].draw(window);
        }

        for (int i = 0; i < e3; i++)
        {
            if (testEnemy3[i].isAlive)
                testEnemy3[i].draw(window);
        }

        for (int i = 0; i < e4; i++)
        {
            if (testEnemy4[i].isAlive)
                testEnemy4[i].draw(window);
        }

        for (int i = 0; i < 100; i++)
        {
            if (!fireBallsUnlimited[i].isDestroyed)
            {
                window.draw(fireBallsUnlimited[i].sprite);
            }
        }

        if (minutes < gameOverTimer && !(TotalRingsToBeCollected >= sonic.getRingsCollected()) && sonic.hp > 0)
        {
            displayText("Time: " + timeString, Vector2f(20.f, 10.f), window, 30, Color(16, 124, 43));
            displayText("Rings: " + to_string(sonic.getRingsCollected()), Vector2f(20.f, 50.f), window, 30, Color::Yellow);
            displayText(" / " + to_string(TotalRingsToBeCollected), Vector2f(120.f, 50.f), window, 30, Color::Green);
            displayText("Score: " + to_string(sonic.getScore()), Vector2f(20.f, 90.f), window, 30, Color::White);
        }
        else if (TotalRingsToBeCollected >= sonic.getRingsCollected())
        {
            showLevelUpScreen(window, 3.5f); // 5 seconds level-up screen

            return;
        }
        else if (sonic.hp <= 0)
        {
            showGameOverScreen(window, sonic, 3.5f); // 5 seconds game over screen

            return;
        }
        else // Time over but not game over
        {
            showTimeOverScreen(window, sonic, 3.5f); // show for 5 seconds
            return;
        }

        if (mode == 1)
            displayText("Sonic Control", {700, 5}, window, 30, Color::Blue);
        else if (mode == 2)
            displayText("Tails Control", {700, 5}, window, 30, Color::Yellow);
        else
            displayText("Knuckles Control", {700, 5}, window, 30, Color::Red);

        Back.draw(window);
        window.draw(hpBarBackground);
        window.draw(hpBar);

        window.display();
    }

    delete[] testEnemy1;
    delete[] testEnemy2;
    delete[] testEnemy3;
    delete[] testEnemy4;
}