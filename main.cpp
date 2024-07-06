#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

#define screenWidth 1024
#define screenHeight 512

void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, screenWidth, screenHeight, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void disableReshape(int, int) {
    glutReshapeWindow(screenWidth, screenHeight);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Raycaster");
    glutReshapeFunc(disableReshape);

    init();

    glutDisplayFunc(display);
    glutMainLoop();
}