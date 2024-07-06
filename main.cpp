#include <GL/freeglut.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926535

#define screenWidth 1024
#define screenHeight 512
#define mapWidth 8
#define mapHeight 8
#define mapSize 64
#define tileSize 64

int worldMap[mapSize] =
    {1, 1, 1, 1, 1, 1, 1, 1,
     1, 0, 1, 0, 0, 0, 0, 1,
     1, 0, 1, 0, 0, 1, 0, 1,
     1, 0, 1, 0, 0, 1, 0, 1,
     1, 0, 0, 0, 0, 0, 0, 1,
     1, 0, 0, 0, 0, 1, 0, 1,
     1, 0, 0, 0, 0, 0, 0, 1,
     1, 1, 1, 1, 1, 1, 1, 1};

float playerX, playerY;
float pDeltaX, pDeltaY;
float pAngle;

void drawRays(int rays) {
    int mx, my, mp, dof;
    float rx, ry, ra, xoff, yoff;

    ra = pAngle;

    for (int r = 0; r < rays; r++) {

        // Y-Side Ray
        dof = 0;
        float atan = -1 / tan(ra);

        // Upwards
        if (ra > PI) {
            ry = playerY - 0.0001;
            rx = (playerY - ry) * atan + playerX;

            yoff = -tileSize;
            xoff = -yoff * atan;
        }
        // Downwards
        if (ra < PI) {
            ry = playerY + tileSize;
            rx = (playerY - ry) * atan + playerX;

            yoff = tileSize;
            xoff = -yoff * atan;
        }
        // Straight left or right
        if (ra == 0 || ra == PI) {
            rx = playerX;
            ry = playerY;
            dof = mapHeight;
        }

        // Check only until map bounds
        while (dof < mapHeight) {
            mx = (int)rx >> 6;
            my = (int)ry >> 6;
            mp = my * mapWidth + mx;

            // Check if hit wall
            if (mp > 0 && mp < mapHeight * mapWidth && worldMap[mp] == 1) {
                dof = 8;
            } else {
                rx += xoff;
                ry += yoff;
                dof += 1;
            }
        }

        // Draw ray
        glColor3f(1, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX, playerY);
        glVertex2i(rx, ry);
        glEnd();
    }
}

void drawPlayer() {
    // Draw player pixel
    glColor3f(0, 255, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(playerX, playerY);
    glEnd();

    // Draw point
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(playerX, playerY);
    glVertex2i(playerX + pDeltaX * 5, playerY + pDeltaY * 5);
    glEnd();
}

void drawMap() {
    int x0, y0;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            // Default color is black to denote blank space
            if (worldMap[y * mapWidth + x] == 1) {
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
    drawRays(1);
    glutSwapBuffers();
}

void disableReshape(int, int) {
    glutReshapeWindow(screenWidth, screenHeight);
}

void onKeyPress(unsigned char key, int x, int y) {
    if (key == 'w') {
        playerX += pDeltaX;
        playerY += pDeltaY;
    }
    if (key == 'a') {
        pAngle -= 0.1;
        if (pAngle < 0) {
            pAngle += 2 * PI;
        }
        pDeltaX = cos(pAngle) * 5;
        pDeltaY = sin(pAngle) * 5;
    }
    if (key == 's') {
        playerX -= pDeltaX;
        playerY -= pDeltaY;
    }
    if (key == 'd') {
        pAngle += 0.1;
        if (pAngle > 2 * PI) {
            pAngle -= 2 * PI;
        }
        pDeltaX = cos(pAngle) * 5;
        pDeltaY = sin(pAngle) * 5;
    }

    glutPostRedisplay();
}

// Screen and player initialization
void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, screenWidth, screenHeight, 0);

    playerX = screenWidth / 4;
    playerY = screenHeight / 2;
    pAngle = 3 * PI / 2;
    pDeltaX = cos(pAngle) * 5;
    pDeltaY = sin(pAngle) * 5;
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