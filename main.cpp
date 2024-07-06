#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

#define screenWidth 1024
#define screenHeight 512
#define mapWidth 8
#define mapHeight 8
#define tileSize 64

int worldMap[mapHeight][mapWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

float playerX, playerY;

void drawPlayer() {
    glColor3f(0, 255, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(playerX, playerY);
    glEnd();
}

void drawMap() {
    int x0, y0;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            // Default color is black to denote blank space
            if (worldMap[y][x] == 1) {
                glColor3f(1, 1, 1);
            } else {
                glColor3f(0, 0, 0);
            }

            x0 = x * tileSize;
            y0 = y * tileSize;

            // Draw square with two triangles
            glBegin(GL_TRIANGLES);
            glVertex2i(x0 + 1, y0 + 1);
            glVertex2i(x0 + 1, y0 + tileSize - 1);
            glVertex2i(x0 + tileSize - 1, y0 + tileSize - 1);
            glVertex2i(x0 + 1, y0 + 1);
            glVertex2i(x0 + tileSize - 1, y0 + 1);
            glVertex2i(x0 + tileSize - 1, y0 + tileSize - 1);
            glEnd();
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap();
    drawPlayer();
    glutSwapBuffers();
}

void disableReshape(int, int) {
    glutReshapeWindow(screenWidth, screenHeight);
}

void onKeyPress(unsigned char key, int x, int y) {
    if (key == 'w') {
        playerY -= 5;
    }
    if (key == 'a') {
        playerX -= 5;
    }
    if (key == 's') {
        playerY += 5;
    }
    if (key == 'd') {
        playerX += 5;
    }

    glutPostRedisplay();
}

// Screen and player initialization
void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, screenWidth, screenHeight, 0);

    playerX = screenWidth / 4;
    playerY = screenHeight / 2;
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Radius");
    glutReshapeFunc(disableReshape);

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(onKeyPress);
    glutMainLoop();
}