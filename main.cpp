#include <GL/freeglut.h>
#include <cmath>
#include <stdlib.h>

#include "texture.h"
#include "trig.h"

#define screenWidth 1024
#define screenHeight 512
#define viewHeight 340
#define wallWidth 8
#define mapWidth 8
#define mapHeight 8
#define mapSize 64
#define tileSize 64
#define mvSpeed 0.15
#define rotSpeed 0.175

typedef struct {
    bool w, a, s, d;
} MvKeyStates;

MvKeyStates mvKeyStates;

int mapWalls[mapSize] =
    {1, 2, 2, 3, 3, 2, 2, 1,
     1, 0, 2, 0, 0, 0, 0, 1,
     1, 0, 2, 0, 0, 0, 0, 1,
     3, 0, 2, 0, 0, 0, 0, 3,
     3, 0, 0, 0, 0, 0, 0, 3,
     1, 0, 0, 0, 0, 2, 0, 1,
     1, 0, 0, 0, 0, 0, 0, 1,
     1, 2, 2, 4, 4, 2, 2, 1};

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
            if (mp > 0 && mp < mapHeight * mapWidth && mapWalls[mp] > 0) {
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
            if (mp > 0 && mp < mapHeight * mapWidth && mapWalls[mp] > 0) {
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

        // Wall shading
        float shade;

        // Choose shorter ray between Y-side or X-side
        if (sideDistX <= sideDistY) {
            rx = xRayX;
            ry = xRayY;
            wallDist = sideDistX;
            shade = 0.5;
            glColor3f(0.7, 0, 0);
        } else if (sideDistY < sideDistX) {
            rx = yRayX;
            ry = yRayY;
            wallDist = sideDistY;
            shade = 1;
            glColor3f(0.9, 0, 0);
        }

        // Draw ray
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX, playerY);
        glVertex2i(rx, ry);
        glEnd();

        // Draw 3d walls
        float textureCode = mapWalls[((int)ry / tileSize * mapWidth + (int)rx / tileSize)];

        float ca = fixAngle(pAngle - ra);

        // For reference, see orthogonal projection vector length
        wallDist = wallDist * cos(degToRad(ca)); // Handle fisheye effect

        float lineHeight = tileSize * viewHeight / wallDist;

        // Prevent abrupt line height clipping when closer to wall
        float textureYStep = textureHeight / lineHeight;
        float textureYOff = 0;

        if (lineHeight > viewHeight) {
            textureYOff = (lineHeight - viewHeight) / 2;
            lineHeight = viewHeight;
        }
        float lineOff = viewHeight / 2 - lineHeight / 2;

        float textureY = textureYStep * textureYOff;
        float textureX;

        // Handle flipping when viewing south or west
        if (shade == 1) {
            textureX = ((int)rx / 2) % textureWidth;
            if (ra > 180) {
                textureX = textureWidth - 1 - textureX;
            }
        } else {
            textureX = ((int)ry / 2) % textureHeight;
            if (ra > 90 && ra < 270) {
                textureX = textureHeight - 1 - textureX;
            }
        }

        glLineWidth(wallWidth);
        glBegin(GL_POINTS);

        for (int y = 0; y < lineHeight; y++) {
            float color = textures[textureCode][(int)textureY * textureWidth + (int)textureX] * shade;

            glColor3f(color, color, color);
            glVertex2i(r * wallWidth + screenWidth / 2 + 20, y + lineOff + viewHeight / 4);

            textureY += textureYStep;
        }
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
            if (mapWalls[y * mapWidth + x] > 0) {
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
        if (mapWalls[tilePy * mapWidth + tilePxAddoff] == 0) {
            playerX += pDeltaX * mvSpeed * deltaTime;
        }
        if (mapWalls[tilePyAddoff * mapWidth + tilePx] == 0) {
            playerY += pDeltaY * mvSpeed * deltaTime;
        }
    }
    if (mvKeyStates.s) {
        if (mapWalls[tilePy * mapWidth + tilePxSuboff] == 0) {
            playerX -= pDeltaX * mvSpeed * deltaTime;
        }
        if (mapWalls[tilePySuboff * mapWidth + tilePx] == 0) {
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

void drawViewBorder() {
    glColor3f(211, 211, 211);
    glLineWidth(8);
    glBegin(GL_LINES);
    // Left
    glVertex2i(screenWidth / 2 + 10, ((5 * viewHeight) >> 2) + 10);
    glVertex2i(screenWidth / 2 + 10, viewHeight / 4 - 10);
    // Right
    glVertex2i(screenWidth - 10, ((5 * viewHeight) >> 2) + 10);
    glVertex2i(screenWidth - 10, viewHeight / 4 - 10);
    // Up
    glVertex2i(screenWidth / 2 + 10, viewHeight / 4 - 10);
    glVertex2i(screenWidth - 10, viewHeight / 4 - 10);
    // Down
    glVertex2i(screenWidth / 2 + 10, ((5 * viewHeight) >> 2) + 10);
    glVertex2i(screenWidth - 10, ((5 * viewHeight) >> 2) + 10);
    glEnd();
}

void display() {
    setDeltaTime();
    movePlayer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawViewBorder();
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

// Initialization stuff like position, screen, and textures
void init() {
    loadTextures();

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