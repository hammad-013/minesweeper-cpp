#include <iostream>
#include "raylib.h"
#include <string>
#define MAX_SIZE 15
#define CELL_SIZE 40

using namespace std;

Texture2D flagTexture;
Texture2D mineTexture;

struct Cell {
    bool mine;
    bool revealed;
    int neighborMines;
    bool flagged;
};

template <typename T>
class Queue {
    int front;
    int rear;
    T arr[MAX_SIZE];
    int size = MAX_SIZE;

public:
    Queue() {
        front = rear = -1;
    }


    bool isEmpty() { return front == -1; }

    bool isFull() { return rear == size - 1; }

    T Front()
    {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return T();
        }
        return arr[front];
    }

    void enqueue(T val)
    {
        if (isFull()) {
            cout << "Queue is full" << endl;
            return;
        }
        if (isEmpty())
            front = 0;

        rear++;
        arr[rear] = val;
    }

    void dequeue()
    {
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

        for(int i = 1; i < row; i++) {
            tempGrid.enqueue(Grid.Front());
            Grid.dequeue();
        }

        Queue<Cell> requiredRow = Grid.Front();
        Grid.dequeue();

        Queue<Cell> tempRow;

        for(int i = 1; i < col; i++) {
            tempRow.enqueue(requiredRow.Front());
            requiredRow.dequeue();
        }

        Cell requiredCell = requiredRow.Front();
        tempRow.enqueue(requiredCell);
        requiredRow.dequeue();

        while(!requiredRow.isEmpty()) {
            tempRow.enqueue(requiredRow.Front());
            requiredRow.dequeue();
        }

        tempGrid.enqueue(tempRow);

        while(!Grid.isEmpty()) {
            tempGrid.enqueue(Grid.Front());
            Grid.dequeue();
        }

        Grid = tempGrid;
        return requiredCell;
    }

    void setCell(Cell c, int row, int col) {
        Queue<Queue<Cell>> tempGrid;

        for(int i = 1; i < row; i++) {
            tempGrid.enqueue(Grid.Front());
            Grid.dequeue();
        }

        Queue<Cell> requiredRow = Grid.Front();
        Grid.dequeue();

        Queue<Cell> tempRow;

        for(int i = 1; i < col; i++) {
            tempRow.enqueue(requiredRow.Front());
            requiredRow.dequeue();
        }

        tempRow.enqueue(c);
        requiredRow.dequeue();

        while(!requiredRow.isEmpty()) {
            tempRow.enqueue(requiredRow.Front());
            requiredRow.dequeue();
        }

        tempGrid.enqueue(tempRow);

        while(!Grid.isEmpty()) {
            tempGrid.enqueue(Grid.Front());
            Grid.dequeue();
        }

        Grid = tempGrid;
    }

    void drawCell(Cell c, int row, int col, int cellSize) {
        int x = (col - 1) * cellSize;
        int y = (row - 1) * cellSize;
        int padding = cellSize / 4;
        int fontSize = cellSize / 2;

        if(!c.revealed) {
            //hidden cell
            DrawRectangle(x, y, cellSize, cellSize, DARKGRAY);
            if(c.flagged) {
                //f in the cell
                // DrawText("F", x + padding, y + padding, fontSize, RED);
                int flagSize = cellSize - 16;                 // a bit smaller than cell
            int flagX = x + (cellSize - flagSize) / 2;
            int flagY = y + (cellSize - flagSize) / 2;

            DrawTexturePro(
            flagTexture,
            Rectangle{0, 0, (float)flagTexture.width, (float)flagTexture.height},  // source
            Rectangle{(float)flagX, (float)flagY, (float)flagSize, (float)flagSize}, // dest
            Vector2{0, 0},      // origin (top-left)
            0.0f,               // rotation
            WHITE               // tint
        );
            }
        }
        else if(c.mine) {
            //draw mine
            DrawRectangle(x, y, cellSize, cellSize, LIGHTGRAY);
            // DrawText("*", x + padding, y + padding, fontSize, RED);
int mineSize = cellSize - 16;                 // a bit smaller than cell
            int mineX = x + (cellSize - mineSize) / 2;
            int mineY = y + (cellSize - mineSize) / 2;

            DrawTexturePro(
            flagTexture,
            Rectangle{0, 0, (float)flagTexture.width, (float)flagTexture.height},  // source
            Rectangle{(float)mineX, (float)mineY, (float)mineSize, (float)mineSize}, // dest
            Vector2{0, 0},      // origin (top-left)
            0.0f,               // rotation
            WHITE               // tint
        );
        }
        else {
            //safe cell
            DrawRectangle(x, y, cellSize, cellSize, LIGHTGRAY);
            if(c.neighborMines > 0) {
                //no of neighbor mines
                DrawText(TextFormat("%i", c.neighborMines), x + padding, y + padding, fontSize, WHITE);
            }
        }
        DrawRectangleLines(x, y, cellSize, cellSize, BLACK);
    }

    void drawRow(int row) {
        Queue<Queue<Cell>> tempGrid;

        for(int i = 1; i < row; i++) {
            tempGrid.enqueue(Grid.Front());
            Grid.dequeue();
        }

        Queue<Cell> requiredRow = Grid.Front();
        Grid.dequeue();

        Queue<Cell> tempRow;
        int col = 1;
        while(!requiredRow.isEmpty()) {
            Cell c = requiredRow.Front();
            drawCell(c, row, col, CELL_SIZE);
            requiredRow.dequeue();
            tempRow.enqueue(c);
            col += 1;
        }

        tempGrid.enqueue(tempRow);

        while(!Grid.isEmpty()) {
            tempGrid.enqueue(Grid.Front());
            Grid.dequeue();
        }

        Grid = tempGrid;
    }

    void drawGrid() {
        for(int i = 1; i <= MAX_SIZE; i++) {
            drawRow(i);
        }
    }
};

class Game {
public:
    DoubleQueue grid;

    void leftClick(int row, int col) {
        Cell c = grid.getCell(row, col);
        c.revealed = true;
        grid.setCell(c, row, col);
    }

    void rightClick(int row, int col) {
        Cell c = grid.getCell(row, col);
        c.flagged = !c.flagged;
        grid.setCell(c, row, col);
    }
};
int main()
{
    Game Game;
    const int screenWidth = 600;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Minesweeper");
    flagTexture = LoadTexture("flag.png");
    mineTexture = LoadTexture("mine.png");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Game.grid.drawGrid();
        EndDrawing();

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int col = GetMouseX() / CELL_SIZE + 1;
            int row = GetMouseY() / CELL_SIZE + 1;

            if(col >= 1 && col <= MAX_SIZE && row >= 1 && row <= MAX_SIZE) {
                Game.leftClick(row, col);
            }
        }
        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            int col = GetMouseX() / CELL_SIZE + 1;
            int row = GetMouseY() / CELL_SIZE + 1;

            if(col >= 1 && col <= MAX_SIZE && row >= 1 && row <= MAX_SIZE) {
                Game.rightClick(row, col);
            }
        }
    }

    CloseWindow();
    return 0;
}