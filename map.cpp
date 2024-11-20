#include <iostream>
#include <windows.h>
#include <ctime>
#include <conio.h>
#include <fstream>
#include <algorithm>

using namespace std;

class SnakeGame {
private:
    const int width = 20;
    const int height = 20;
    int x, y, fruitX, fruitY;
    int score, highScore;
    int speed;
    bool gameOver, pause;
    int tailX[100], tailY[100];
    int nTail;

    int topScores[5] = {0, 0, 0, 0, 0};

    enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
    eDirection dir;

public:
    SnakeGame() : x(0), y(0), fruitX(0), fruitY(0), score(0), highScore(0), speed(400), gameOver(false), pause(false), nTail(0), dir(STOP) {}

    void LoadHighScores() {
        ifstream file("highscores.txt");
        if (file.is_open()) {
            for (int i = 0; i < 5; ++i) {
                file >> topScores[i];
            }
            highScore = topScores[0];
            file.close();
        }
    }

    void SaveHighScores() {
        ofstream file("highscores.txt");
        if (file.is_open()) {
            for (int i = 0; i < 5; ++i) {
                file << topScores[i] << endl;
            }
            file.close();
        }
    }

    void Setup() {
        srand(time(NULL));
        gameOver = false;
        dir = STOP;
        x = width / 2 - 1;
        y = height / 2 - 1;
        fruitX = rand() % width;
        fruitY = rand() % height;
        score = 0;
        nTail = 0;
        speed = 400;
    }

    void Draw() {
        Sleep(speed);
        cout << "\033[1;1H"; // Очистка экрана
        for (int i = 0; i < width + 1; i++)
            cout << "# ";
        cout << endl;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0 || j == width - 1) {
                    cout << "# ";
                }
                if (i == y && j == x) {
                    cout << "V "; // Голова змеи
                }
                else if (i == fruitY && j == fruitX) {
                    cout << "@ "; // Фрукт
                }
                else {
                    bool print = false;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            print = true;
                            cout << "o "; // Тело змеи
                        }
                    }
                    if (!print)
                        cout << "  ";
                }
            }
            cout << endl;
        }
        for (int i = 0; i < width + 1; i++)
            cout << "# ";
        cout << endl;
        cout << "Score: " << score << " | High Score: " << highScore << endl; // Изменение здесь
    }

    void Input() {
        if (_kbhit()) {
            char key = _getch();
            if (key == 'p') {
                pause = !pause;
            }
            if (!pause) {
                switch (key) {
                    case 'a':
                        if (dir != RIGHT)
                            dir = LEFT;
                        break;
                    case 'd':
                        if (dir != LEFT)
                            dir = RIGHT;
                        break;
                    case 'w':
                        if (dir != DOWN)
                            dir = UP;
                        break;
                    case 's':
                        if (dir != UP)
                            dir = DOWN;
                        break;
                }
            }
            if (key == 'x') {
                gameOver = true;
                system("cls");
                cout << "Gameover! Your score is: " << score << endl;
            }
        }
    }

    void CheckHighScores() {
        if (score > topScores[4]) { // Проверка на 5-е место
            topScores[4] = score;
            sort(topScores, topScores + 5, greater<int>()); // Сортировка рекордов по убыванию
            cout << "Congratulations! You've made it to the top 5 with a score of: " << score << endl;
        }
    }

    void Logic() {
        int prevX = tailX[0];
        int prevY = tailY[0];
        int prev2x, prev2y;
        tailX[0] = x;
        tailY[0] = y;
        for (int i = 1; i < nTail; i++) {
            prev2x = tailX[i];
            prev2y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2x;
            prevY = prev2y;
        }
        switch (dir) {
            case LEFT:
                x--;
                break;
            case RIGHT:
                x++;
                break;
            case UP:
                y--;
                break;
            case DOWN:
                y++;
                break;
        }
        if (x >= width - 1)
            x = 0;
        else if (x < 0)
            x = width - 2;
        if (y >= height)
            y = 0;
        else if (y < 0)
            y = height - 1;
        // Проверка на столкновение с хвостом
        for (int i = 0; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y) {
                gameOver = true;
                system("cls");
                cout << "Gameover! Your score is: " << score << endl;
            }
        }
        // Проверка на поедание фрукта
        if (x == fruitX && y == fruitY) {
            // Проверка, чтобы фрукт не появлялся на хвосте
            bool onTail = true;
            while (onTail) {
                onTail = false;
                fruitX = rand() % (width - 1);
                fruitY = rand() % height;
                for (int i = 0; i < nTail; i++) {
                    if (tailX[i] == fruitX && tailY[i] == fruitY) {
                        onTail = true;
                        break;
                    }
                }
            }
            score++;
            speed -= 10;
            if (speed < 50)
                speed = 50;
            cout << "\a";
            if (nTail < 100) { //Limita la lungimea sarpelui
                tailX[nTail] = prevX;
                tailY[nTail] = prevY;
                nTail++;
            }
        }
        // Обновление highScore
        if (score > highScore) {
            highScore = score; // Обновляем высокие баллы
        }
    }

    void DisplayTopScores() {
        cout << "Top Scores:" << endl;
        for (int i = 0; i < 5; ++i) {
            cout << (i + 1) << ". " << topScores[i] << " fruits" << endl; // Вывод топ-5 рекордов в вертикальном формате
        }
    }

    void Decision() {
        char menu;
        cout << "Want to play? (Y/N): ";
        cin >> menu;
        if (menu == 'Y' || menu == 'y') {
            system("cls");
            Setup();
            while (!gameOver) {
                Draw();
                Input();
                if (!pause) {
                    Logic();
                }
            }
            CheckHighScores(); // Проверяем рекорды в конце игры
            SaveHighScores(); // Сохраняем только если рекорды обновились
            DisplayTopScores(); // Отобразить рекорды после окончания игры
            Decision();
        }
        else if (menu == 'N' || menu == 'n') {
            system("cls");
            cout << "Let's play later!" << endl;
        }
        else {
            system("cls");
            cout << "Wrong action!" << endl;
            Decision();
        }
    }
};

int main() {
    SnakeGame game;
    game.LoadHighScores();
    cout << "Welcome to my game - Snake, let's take a look at some rules." << endl;
    cout << "1. Use WASD to move" << endl;
    cout << "2. Use X to stop the game." << endl;
    cout << "3. Use P to pause game. You can return pressing P again." << endl;
    cout << "4. Don't try to eat your tail, only fruits!" << endl;
    game.Decision();
    return 0;
}
