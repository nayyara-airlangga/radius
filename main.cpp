#include <GL/freeglut.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "trig.h"

#define screenWidth 1024
#define screenHeight 512
#define viewHeight 340
#define wallWidth 8
#define mapWidth 8
#define mapHeight 8
#define mapSize 64
#define tileSize 64
#define mvSpeed 0.2
#define rotSpeed 0.175

typedef struct {
    bool w, a, s, d;
} MvKeyStates;

MvKeyStates mvKeyStates;

int worldMap[mapSize] =
    {1, 1, 1, 1, 1, 1, 1, 1,
     1, 0, 1, 0, 0, 1, 1, 1,
     1, 0, 1, 0, 0, 0, 1, 1,
     1, 0, 1, 0, 0, 0, 0, 1,
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
    float wallDist;

    ra = fixAngle(pAngle + rays / 2);

    for (int r = 0; r < rays; r++) {
        float sideDistY = INT_MAX;
        float yRayX = playerX;
        float yRayY = playerY;

        // Y-Side Ray
        dof = 0;
        float invTan = 1 / tan(degToRad(ra));

        // Upwards
        if (sin(degToRad(ra)) > 0.001) {
            ry = ((int)playerY >> 6 << 6) - 0.0001;
            rx = (playerY - ry) * invTan + playerX;

            yoff = -tileSize;
            xoff = -yoff * invTan;
        }
        // Downwards
        else if (sin(degToRad(ra)) < -0.001) {
            ry = ((int)playerY >> 6 << 6) + tileSize;
            rx = (playerY - ry) * invTan + playerX;

            yoff = tileSize;
            xoff = -yoff * invTan;
        }
        // Straight left or right
        else {
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
                yRayX = rx;
                yRayY = ry;
                sideDistY = distance(playerX, playerY, yRayX, yRayY, ra);
                dof = 8;
            } else {
                rx += xoff;
                ry += yoff;
                dof += 1;
            }
        }

        float sideDistX = INT_MAX;
        float xRayX = playerX;
        float xRayY = playerY;

        // X-Side Ray
        dof = 0;
        float Tan = tan(degToRad(ra));

        // Left
        if (cos(degToRad(ra)) < -0.001) {
            rx = ((int)playerX >> 6 << 6) - 0.0001;
            ry = (playerX - rx) * Tan + playerY;

            xoff = -tileSize;
            yoff = -xoff * Tan;
        }
        // Right
        else if (cos(degToRad(ra)) > 0.001) {
            rx = ((int)playerX >> 6 << 6) + tileSize;
            ry = (playerX - rx) * Tan + playerY;

            xoff = tileSize;
            yoff = -xoff * Tan;
        }
        // Straight up or down
        else {
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
                xRayX = rx;
                xRayY = ry;
                sideDistX = distance(playerX, playerY, xRayX, xRayY, ra);
                dof = 8;
            } else {
                rx += xoff;
                ry += yoff;
                dof += 1;
            }
        }

        // Choose shorter ray between Y-side or X-side
        if (sideDistX <= sideDistY) {
            rx = xRayX;
            ry = xRayY;
            wallDist = sideDistX;
            glColor3f(0.9, 0, 0);
        } else if (sideDistY < sideDistX) {
            rx = yRayX;
            ry = yRayY;
            wallDist = sideDistY;
            glColor3f(0.7, 0, 0);
        }

        // Draw ray
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX, playerY);
        glVertex2i(rx, ry);
        glEnd();

        // Draw 3d walls
        float ca = fixAngle(pAngle - ra);

        // For reference, see orthogonal projection vector length
        wallDist = wallDist * cos(degToRad(ca)); // Handle fisheye effect

        float lineHeight = tileSize * viewHeight / wallDist;
        if (lineHeight > viewHeight) {
            lineHeight = viewHeight;
        }
        float lineOff = viewHeight / 2 - lineHeight / 2;

        glLineWidth(wallWidth);
        glBegin(GL_LINES);
        glVertex2i(r * wallWidth + screenWidth / 2 + 20, lineOff + viewHeight / 4);
        glVertex2i(r * wallWidth + screenWidth / 2 + 20, lineHeight + lineOff + viewHeight / 4);
        glEnd();

        // Increment degree
        ra = fixAngle(ra - 1);
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
    glVertex2i(playerX + pDeltaX * 20, playerY + pDeltaY * 20);
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

float prevTime, newTime, deltaTime;

// Time deltas to adjust speed based on frame rate
void setDeltaTime() {
    newTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = newTime - prevTime;
    prevTime = newTime;
}

void movePlayer() {
    // Offset for collision detection
    int xoff, yoff;
    if (pDeltaX < 0) {
        xoff = -20;
    } else {
        xoff = 20;
    }
    if (pDeltaY < 0) {
        yoff = -20;
    } else {
        yoff = 20;
    }

    int tilePx = playerX / tileSize;
    int tilePxAddoff = (playerX + xoff) / tileSize;
    int tilePxSuboff = (playerX - xoff / 2) / tileSize;
    int tilePy = (playerY) / tileSize;
    int tilePyAddoff = (playerY + yoff) / tileSize;
    int tilePySuboff = (playerY - yoff / 2) / tileSize;

    // Position movements
    if (mvKeyStates.w) {
        if (worldMap[tilePy * mapWidth + tilePxAddoff] == 0) {
            playerX += pDeltaX * mvSpeed * deltaTime;
        }
        if (worldMap[tilePyAddoff * mapWidth + tilePx] == 0) {
            playerY += pDeltaY * mvSpeed * deltaTime;
        }
    }
    if (mvKeyStates.s) {
        if (worldMap[tilePy * mapWidth + tilePxSuboff] == 0) {
            playerX -= pDeltaX * mvSpeed * deltaTime;
        }
        if (worldMap[tilePySuboff * mapWidth + tilePx] == 0) {
            playerY -= pDeltaY * mvSpeed * deltaTime;
        }
    }

    // Camera rotation
    if (mvKeyStates.a) {
        pAngle += rotSpeed * deltaTime;
        pAngle = fixAngle(pAngle);
        pDeltaX = cos(degToRad(pAngle));
        pDeltaY = -sin(degToRad(pAngle));
    }
    if (mvKeyStates.d) {
        pAngle -= rotSpeed * deltaTime;
        pAngle = fixAngle(pAngle);
        pDeltaX = cos(degToRad(pAngle));
        pDeltaY = -sin(degToRad(pAngle));
    }

    glutPostRedisplay();
}

void display() {
    setDeltaTime();
    movePlayer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap();
    drawRays(60);
    drawPlayer();
    glutSwapBuffers();
}

void disableReshape(int, int) {
    glutReshapeWindow(screenWidth, screenHeight);
}

void onKeyDown(unsigned char key, int x, int y) {
    if (key == 'w') {
        mvKeyStates.w = true;
    }
    if (key == 'a') {
        mvKeyStates.a = true;
    }
    if (key == 's') {
        mvKeyStates.s = true;
    }
    if (key == 'd') {
        mvKeyStates.d = true;
    }
}

void onKeyUp(unsigned char key, int x, int y) {
    if (key == 'w') {
        mvKeyStates.w = false;
    }
    if (key == 'a') {
        mvKeyStates.a = false;
    }
    if (key == 's') {
        mvKeyStates.s = false;
    }
    if (key == 'd') {
        mvKeyStates.d = false;
    }
}

// Screen and player initialization
void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, screenWidth, screenHeight, 0);

    playerX = screenWidth / 4;
    playerY = screenHeight / 2;
    pAngle = 90;
    pDeltaX = cos(degToRad(pAngle));
    pDeltaY = -sin(degToRad(pAngle));
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Radius");
    glutReshapeFunc(disableReshape);

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutMainLoop();
}