#include "raylib.h"
#include <iostream>
#include <random>
#include <string>

#define MAX_SIZE 15
#define CELL_SIZE 40

using namespace std;

random_device rd;
mt19937 gen(rd());

Texture2D flagTexture;
Texture2D mineTexture;
Texture2D mikuHappy;
Texture2D mikuSad;
Music bgMusic;
Music sadMusic;
bool isShaking = false;
double shakeStartTime = 0.0;
const float SHAKE_DURATION = 4.0f;

int randomValue(int a, int b) {
  return uniform_int_distribution<int>(a, b)(gen);
}

struct Cell {
  bool mine;
  bool revealed;
  int neighborMines;
  bool flagged;
};

struct CellPos {
  int row;
  int col;
};

template <typename T> class Queue {
  T *arr;
  int size;
  int front;
  int rear;

public:
  Queue(int size = MAX_SIZE) {
    this->size = size;
    arr = new T[size];
    front = -1;
    rear = -1;
  }

  bool isEmpty() { return front == -1; }

  bool isFull() { return rear == size - 1; }

  T Front() {
    if (isEmpty()) {
      cout << "Queue is empty" << endl;
      return T();
    }
    return arr[front];
  }

  void enqueue(T val) {
    if (isFull()) {
      cout << "Queue is full" << endl;
      return;
    }
    if (isEmpty())
      front = 0;

    rear++;
    arr[rear] = val;
  }

  void dequeue() {
    if (isEmpty()) {
      cout << "Queue is empty" << endl;
      return;
    }

    front++;
    if (front > rear)
      front = rear = -1;
  }
};

class DoubleQueue {
  Queue<Queue<Cell>> Grid;

public:
  DoubleQueue() {
    for (int i = 0; i < MAX_SIZE; i++) {
      Queue<Cell> rowQueue;
      for (int j = 0; j < MAX_SIZE; j++) {
        Cell cell = {false, false, 0, false};
        rowQueue.enqueue(cell);
      }
      Grid.enqueue(rowQueue);
    }
  }

  Cell getCell(int row, int col) {
    Queue<Queue<Cell>> tempGrid;

    for (int i = 1; i < row; i++) {
      tempGrid.enqueue(Grid.Front());
      Grid.dequeue();
    }

    Queue<Cell> requiredRow = Grid.Front();
    Grid.dequeue();

    Queue<Cell> tempRow;

    for (int i = 1; i < col; i++) {
      tempRow.enqueue(requiredRow.Front());
      requiredRow.dequeue();
    }

    Cell requiredCell = requiredRow.Front();
    tempRow.enqueue(requiredCell);
    requiredRow.dequeue();

    while (!requiredRow.isEmpty()) {
      tempRow.enqueue(requiredRow.Front());
      requiredRow.dequeue();
    }

    tempGrid.enqueue(tempRow);

    while (!Grid.isEmpty()) {
      tempGrid.enqueue(Grid.Front());
      Grid.dequeue();
    }

    Grid = tempGrid;
    return requiredCell;
  }

  void setCell(Cell c, int row, int col) {
    Queue<Queue<Cell>> tempGrid;

    for (int i = 1; i < row; i++) {
      tempGrid.enqueue(Grid.Front());
      Grid.dequeue();
    }

    Queue<Cell> requiredRow = Grid.Front();
    Grid.dequeue();

    Queue<Cell> tempRow;

    for (int i = 1; i < col; i++) {
      tempRow.enqueue(requiredRow.Front());
      requiredRow.dequeue();
    }

    tempRow.enqueue(c);
    requiredRow.dequeue();

    while (!requiredRow.isEmpty()) {
      tempRow.enqueue(requiredRow.Front());
      requiredRow.dequeue();
    }

    tempGrid.enqueue(tempRow);

    while (!Grid.isEmpty()) {
      tempGrid.enqueue(Grid.Front());
      Grid.dequeue();
    }

    Grid = tempGrid;
  }

  // void drawCell(Cell c, int row, int col, int cellSize) {
  //     int x = (col - 1) * cellSize;
  //     int y = (row - 1) * cellSize;
  //     int padding = cellSize / 4;
  //     int fontSize = cellSize / 2;

  //     if(!c.revealed) {
  //         //hidden cell
  //         DrawRectangle(x, y, cellSize, cellSize, LIGHTGRAY);
  //         if(c.flagged) {
  //             //f in the cell
  //             int flagSize = cellSize - 16;
  //         int flagX = x + (cellSize - flagSize) / 2;
  //         int flagY = y + (cellSize - flagSize) / 2;

  //         DrawTexturePro(
  //         flagTexture,
  //         Rectangle{0, 0, (float)flagTexture.width,
  //         (float)flagTexture.height}, Rectangle{(float)flagX, (float)flagY,
  //         (float)flagSize, (float)flagSize}, Vector2{0, 0}, 0.0f, WHITE
  //     );
  //         }
  //     }
  //     else if(c.mine) {
  //         //draw mine
  //         DrawRectangle(x, y, cellSize, cellSize, DARKGRAY);
  //         int mineSize = cellSize - 16;
  //         int mineX = x + (cellSize - mineSize) / 2;
  //         int mineY = y + (cellSize - mineSize) / 2;

  //         DrawTexturePro(
  //         mineTexture,
  //         Rectangle{0, 0, (float)flagTexture.width,
  //         (float)flagTexture.height}, Rectangle{(float)mineX, (float)mineY,
  //         (float)mineSize, (float)mineSize}, Vector2{0, 0}, 0.0f, WHITE
  //     );
  //     }
  //     else {
  //         //safe cell
  //         DrawRectangle(x, y, cellSize, cellSize, DARKGRAY);
  //         if(c.neighborMines > 0) {
  //             //no of neighbor mines
  //             Color textColor;
  //             switch(c.neighborMines) {
  //                 case 1: textColor = BLUE; break;
  //                 case 2: textColor = GREEN; break;
  //                 case 3: textColor = RED; break;
  //             }
  //             DrawText(TextFormat("%i", c.neighborMines), x + padding, y +
  //             padding, fontSize, textColor);
  //         }
  //     }
  //     DrawRectangleLines(x, y, cellSize, cellSize, BLACK);
  // }

  void drawCell(Cell c, int row, int col, int cellSize) {
    int x = (col - 1) * cellSize;
    int y = (row - 1) * cellSize;
    int padding = cellSize / 4;
    int fontSize = cellSize / 2;

    if (!c.revealed) {
      DrawRectangle(x, y, cellSize, cellSize, Color{192, 192, 192, 255});

      DrawLine(x, y, x + cellSize, y, WHITE);
      DrawLine(x, y, x, y + cellSize, WHITE);
      DrawLine(x + 1, y + 1, x + cellSize - 1, y + 1, WHITE);
      DrawLine(x + 1, y + 1, x + 1, y + cellSize - 1, WHITE);

      DrawLine(x, y + cellSize, x + cellSize, y + cellSize,
               Color{128, 128, 128, 255});
      DrawLine(x + cellSize, y, x + cellSize, y + cellSize,
               Color{128, 128, 128, 255});
      DrawLine(x + 1, y + cellSize - 1, x + cellSize - 1, y + cellSize - 1,
               Color{128, 128, 128, 255});
      DrawLine(x + cellSize - 1, y + 1, x + cellSize - 1, y + cellSize - 1,
               Color{128, 128, 128, 255});

      if (c.flagged) {
        int flagSize = cellSize - 16;
        int flagX = x + (cellSize - flagSize) / 2;
        int flagY = y + (cellSize - flagSize) / 2;

        DrawTexturePro(flagTexture,
                       Rectangle{0, 0, (float)flagTexture.width,
                                 (float)flagTexture.height},
                       Rectangle{(float)flagX, (float)flagY, (float)flagSize,
                                 (float)flagSize},
                       Vector2{0, 0}, 0.0f, WHITE);
      }
    } else if (c.mine) {
      DrawRectangle(x, y, cellSize, cellSize, Color{225, 225, 225, 255});

      DrawRectangleLines(x, y, cellSize, cellSize, Color{128, 128, 128, 255});

      int mineSize = cellSize - 16;
      int mineX = x + (cellSize - mineSize) / 2;
      int mineY = y + (cellSize - mineSize) / 2;

      DrawTexturePro(
          mineTexture,
          Rectangle{0, 0, (float)mineTexture.width, (float)mineTexture.height},
          Rectangle{(float)mineX, (float)mineY, (float)mineSize,
                    (float)mineSize},
          Vector2{0, 0}, 0.0f, BLACK);
    } else {
      DrawRectangle(x, y, cellSize, cellSize, Color{225, 225, 225, 255});
      DrawRectangleLines(x, y, cellSize, cellSize, Color{128, 128, 128, 255});

      if (c.neighborMines > 0) {
        Color textColor;
        switch (c.neighborMines) {
        case 1:
          textColor = BLUE;
          break;
        case 2:
          textColor = Color{0, 128, 0, 255};
          break;
        case 3:
          textColor = RED;
          break;
        case 4:
          textColor = Color{0, 0, 128, 255};
          break;
        case 5:
          textColor = Color{128, 0, 0, 255};
          break;
        case 6:
          textColor = Color{0, 128, 128, 255};
          break;
        case 7:
          textColor = BLACK;
          break;
        case 8:
          textColor = Color{128, 128, 128, 255};
          break;
        default:
          textColor = BLACK;
        }
        const char *numberText = TextFormat("%i", c.neighborMines);
        int textWidth = MeasureText(numberText, fontSize);
        int textX = x + (cellSize - textWidth) / 2;
        int textY = y + (cellSize - fontSize) / 2;

        DrawText(numberText, textX, textY, fontSize, textColor);
      }
    }
  }

  void drawRow(int row) {
    Queue<Queue<Cell>> tempGrid;

    for (int i = 1; i < row; i++) {
      tempGrid.enqueue(Grid.Front());
      Grid.dequeue();
    }

    Queue<Cell> requiredRow = Grid.Front();
    Grid.dequeue();

    Queue<Cell> tempRow;
    int col = 1;
    while (!requiredRow.isEmpty()) {
      Cell c = requiredRow.Front();
      drawCell(c, row, col, CELL_SIZE);
      requiredRow.dequeue();
      tempRow.enqueue(c);
      col += 1;
    }

    tempGrid.enqueue(tempRow);

    while (!Grid.isEmpty()) {
      tempGrid.enqueue(Grid.Front());
      Grid.dequeue();
    }

    Grid = tempGrid;
  }

  void drawGrid() {
    for (int i = 1; i <= MAX_SIZE; i++) {
      drawRow(i);
    }
  }

  void initializeMines(int mines, int clickedRow, int clickedCol) {
    int minesPlaced = 0;
    while (minesPlaced < mines) {
      int row = randomValue(1, MAX_SIZE);
      int col = randomValue(1, MAX_SIZE);
      if (row == clickedRow && col == clickedCol) {
        continue;
      }
      Cell c = getCell(row, col);
      if (!c.mine) {
        c.mine = true;
        setCell(c, row, col);
        cout << "Placed mine at (" << row << ", " << col << ")\n";
        minesPlaced += 1;
      }
    }
  }

  void revealAllMines() {
    for (int i = 1; i <= MAX_SIZE; i++) {
      for (int j = 1; j <= MAX_SIZE; j++) {
        Cell cell = getCell(i, j);
        if (cell.mine) {
          cell.revealed = true;
          setCell(cell, i, j);
        }
      }
    }
  }
  int calculateCellNeighborMines(int row, int col) {
    int minesCount = 0;
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (i == 0 && j == 0) {
          continue;
        }
        int newRow = row + i;
        int newCol = col + j;
        if (newCol >= 1 && newCol <= MAX_SIZE && newRow >= 1 &&
            newRow <= MAX_SIZE) {
          Cell neighbor = getCell(newRow, newCol);
          if (neighbor.mine) {
            minesCount += 1;
          }
        }
      }
    }
    return minesCount;
  }
  void calculateNeighborMines() {
    for (int i = 1; i <= MAX_SIZE; i++) {
      for (int j = 1; j <= MAX_SIZE; j++) {
        Cell currentCell = getCell(i, j);
        currentCell.neighborMines = calculateCellNeighborMines(i, j);
        setCell(currentCell, i, j);
      }
    }
  }

  void revealFlood(int row, int col, int &scoreIncrement, int &safeCells) {
    Queue<CellPos> toReveal(MAX_SIZE * MAX_SIZE);
    CellPos startCell = {row, col};
    toReveal.enqueue(startCell);
    while (!toReveal.isEmpty()) {
      CellPos CellPosition = toReveal.Front();
      toReveal.dequeue();

      Cell currentCell = getCell(CellPosition.row, CellPosition.col);

      if (currentCell.revealed || currentCell.mine) {
        continue;
      }
      currentCell.revealed = true;
      safeCells += 1;
      scoreIncrement += 1;
      setCell(currentCell, CellPosition.row, CellPosition.col);
      if (currentCell.neighborMines > 0) {
        continue;
      }
      for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
          if (i == 0 && j == 0) {
            continue;
          }
          int newRow = CellPosition.row + i;
          int newCol = CellPosition.col + j;
          if (newCol >= 1 && newCol <= MAX_SIZE && newRow >= 1 &&
              newRow <= MAX_SIZE) {
            Cell neighborCell = getCell(newRow, newCol);
            if (!neighborCell.revealed && !neighborCell.mine) {
              CellPos newCell = {newRow, newCol};
              toReveal.enqueue(newCell);
            }
          }
        }
      }
    }
  }
};

class Game {
public:
  bool firstClick;
  bool gameOver;
  bool gameWin;
  int score;
  int flagScore;
  int totalSafeCells;
  int revealedSafeCells;
  double startTime;
  DoubleQueue grid;
  int totalMines;

  Game(int mines = 40) {
    firstClick = false;
    gameOver = false;
    score = 0;
    flagScore = 0;
    gameWin = false;
    totalMines = mines;
    totalSafeCells = MAX_SIZE * MAX_SIZE - totalMines;
    revealedSafeCells = 0;
    startTime = 0.0;
    grid = DoubleQueue();
  }

  void leftClick(int row, int col) {
    if (!firstClick) {
      firstClick = true;
      grid.initializeMines(totalMines, row, col);
      Cell c = grid.getCell(row, col);
      c.revealed = true;
      startTime = GetTime();
      revealedSafeCells += 1;
      score += 1;
      grid.setCell(c, row, col);
      grid.calculateNeighborMines();
    } else {
      Cell c = grid.getCell(row, col);
      c.revealed = true;
      if (c.mine) {
        cout << "Game Over! You clicked on a mine at (" << row << ", " << col
             << ")\n";
        score += flagScore;
        score = max(0, score);

        StopMusicStream(bgMusic);
        PlayMusicStream(sadMusic);
        isShaking = true;
        shakeStartTime = GetTime();

        grid.revealAllMines();
        gameOver = true;
        return;
      } else {
        if (c.neighborMines == 0) {
          grid.revealFlood(row, col, score, revealedSafeCells);
        } else {
          revealedSafeCells += 1;
          score += 1;
          grid.setCell(c, row, col);
        }
        if (revealedSafeCells == totalSafeCells) {
          cout << "Congratulations! You have revealed all safe cells!\n";
          gameWin = true;
          return;
        }
      }
    }
  }

  void rightClick(int row, int col) {
    Cell c = grid.getCell(row, col);
    if (!c.revealed) {
      if (!c.flagged && c.mine) {
        flagScore += 3;
      } else if (!c.flagged && !c.mine) {
        flagScore -= 2;
      }
      c.flagged = !c.flagged;
      grid.setCell(c, row, col);
    }
  }
  void reset() {
    firstClick = false;
    gameOver = false;
    gameWin = false;
    score = 0;
    flagScore = 0;
    revealedSafeCells = 0;
    grid = DoubleQueue();
  }
};
int main() {
  const int screenWidth = 600;
  const int screenHeight = 680;

  InitWindow(screenWidth, screenHeight, "Minesweeper");
  InitAudioDevice();
  SetMasterVolume(1.0f);

  bgMusic = LoadMusicStream("miku.ogg");
  sadMusic = LoadMusicStream("miku-angry.ogg");
  bgMusic.looping = true;
  PlayMusicStream(bgMusic);

  Image icon = LoadImage("mine.png");
  SetWindowIcon(icon);
  flagTexture = LoadTexture("flag.png");
  mineTexture = LoadTexture("mine.png");
  mikuHappy = LoadTexture("miku-happy.png");
  mikuSad = LoadTexture("miku-sad.png");
  SetTargetFPS(60);

  const int mikuDisplaySize = 100;
  const int mikuX = screenWidth / 2 - mikuDisplaySize / 2;
  const int mikuY = screenHeight - mikuDisplaySize;

  Game Game;
  while (!WindowShouldClose()) {
    SetMusicVolume(bgMusic, 0.75f);
    UpdateMusicStream(bgMusic);
    UpdateMusicStream(sadMusic);
    BeginDrawing();
    ClearBackground(BLACK);
    Game.grid.drawGrid();

    //     if(Game.gameOver) {
    //         DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0,
    //         150}); DrawText("GAME OVER!", screenWidth/2 - 120, screenHeight/2
    //         - 20, 40, RED); DrawText("Press R to Restart", screenWidth/2 -
    //         100, screenHeight/2 + 30, 20, WHITE); DrawText(TextFormat("Score:
    //         %d", Game.score), screenWidth/2 - 50, screenHeight/2 + 60, 25,
    //         WHITE);

    // DrawTexturePro(
    //     mikuSad,
    //     Rectangle{0, 0, (float)mikuSad.width, (float)mikuSad.height},
    //     Rectangle{(float)mikuX, (float)mikuY, (float)mikuDisplaySize,
    //     (float)mikuDisplaySize}, Vector2{0, 0}, 0.0f, WHITE
    // );

    //         PauseMusicStream(bgMusic);
    //     }

    if (Game.gameOver) {
      double elapsedShake = GetTime() - shakeStartTime;

      if (isShaking && elapsedShake < SHAKE_DURATION) {
        float intensity = 15.0f * (1.0f - elapsedShake / SHAKE_DURATION);
        float offsetX = (float)(rand() % 100 - 50) * intensity / 50.0f;
        float offsetY = (float)(rand() % 100 - 50) * intensity / 50.0f;

        DrawTexturePro(
            mikuSad,
            Rectangle{0, 0, (float)mikuSad.width, (float)mikuSad.height},
            Rectangle{mikuX + offsetX, mikuY + offsetY, (float)mikuDisplaySize,
                      (float)mikuDisplaySize},
            Vector2{0, 0}, 0.0f, WHITE);
      } else {
        isShaking = false;

        DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 180});
        DrawText("GAME OVER!", screenWidth / 2 - 120, screenHeight / 2 - 40, 40,
                 RED);
        DrawText("Press R to Restart", screenWidth / 2 - 100,
                 screenHeight / 2 + 10, 20, WHITE);
        DrawText(TextFormat("Score: %d", Game.score), screenWidth / 2 - 60,
                 screenHeight / 2 + 40, 25, WHITE);

        DrawTexturePro(
            mikuSad,
            Rectangle{0, 0, (float)mikuSad.width, (float)mikuSad.height},
            Rectangle{(float)mikuX, (float)mikuY, (float)mikuDisplaySize,
                      (float)mikuDisplaySize},
            Vector2{0, 0}, 0.0f, WHITE);
      }
    } else if (Game.gameWin) {
      DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 150});
      DrawText("CONGRATULATIONS!", screenWidth / 2 - 170, screenHeight / 2 - 20,
               30, GREEN);
      DrawText("You Win!", screenWidth / 2 - 80, screenHeight / 2 + 20, 30,
               GREEN);
      DrawText("Press R to Restart", screenWidth / 2 - 110,
               screenHeight / 2 + 60, 20, WHITE);
      DrawText(TextFormat("Score: %d", Game.score), screenWidth / 2 - 70,
               screenHeight / 2 + 90, 25, WHITE);
      PauseMusicStream(bgMusic);
    } else {
      double elapsed;
      if (Game.firstClick) {
        elapsed = GetTime() - Game.startTime;
      } else {
        elapsed = 0.0;
      }
      int totalSeconds = (int)elapsed;
      int minutes = totalSeconds / 60;
      int seconds = totalSeconds % 60;
      DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 460, 630, 25,
               WHITE);
      DrawText(TextFormat("Score: %d", Game.score), 10, 630, 25, WHITE);
      ResumeMusicStream(bgMusic);

      float bounce = sinf(GetTime() * 4.0f) * 5.0f;
      float currentY = mikuY + bounce;

      DrawTexturePro(
          mikuHappy,
          Rectangle{0, 0, (float)mikuHappy.width, (float)mikuHappy.height},
          Rectangle{(float)mikuX, currentY, (float)mikuDisplaySize,
                    (float)mikuDisplaySize},
          Vector2{0, 0}, 0.0f, WHITE);
    }

    EndDrawing();

    if (IsKeyPressed(KEY_R) && (Game.gameOver || Game.gameWin)) {
      Game.reset();
      StopMusicStream(sadMusic);
      PlayMusicStream(bgMusic);
      isShaking = false;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !Game.gameOver) {
      int col = GetMouseX() / CELL_SIZE + 1;
      int row = GetMouseY() / CELL_SIZE + 1;

      if (col >= 1 && col <= MAX_SIZE && row >= 1 && row <= MAX_SIZE) {
        Game.leftClick(row, col);
      }
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !Game.gameOver) {
      int col = GetMouseX() / CELL_SIZE + 1;
      int row = GetMouseY() / CELL_SIZE + 1;

      if (col >= 1 && col <= MAX_SIZE && row >= 1 && row <= MAX_SIZE) {
        Game.rightClick(row, col);
      }
    }
  }

  CloseWindow();
  StopMusicStream(bgMusic);
  UnloadMusicStream(bgMusic);
  CloseAudioDevice();
  UnloadImage(icon);
  return 0;
}