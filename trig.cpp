#define _USE_MATH_DEFINES

#include <math.h>

#include "trig.h"

float degToRad(float angle) {
    return angle * M_PI / 180;
}

float fixAngle(float angle) {
    if (angle > 359) {
        angle -= 360;
    }
    if (angle < 0) {
        angle += 360;
    }
    return angle;
}

float distance(float x1, float y1, float x2, float y2, float angle) {
    return cos(degToRad(angle)) * (x2 - x1) - sin(degToRad(angle)) * (y2 - y1);
}