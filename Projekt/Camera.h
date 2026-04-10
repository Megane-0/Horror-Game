#pragma once

#include "VectorUtils4.h"

class Camera {
public:
    Camera();

    void update(float deltaTime, int mouseX, int mouseY);

    mat4 getViewMatrix() const;
    vec3 getPosition() const;
    bool ignoreMouse = false;
    vec2 lastMouse;
    bool firstMouse;
    bool ignoreNextMouse = false;
    const int CENTER_X = 1024/2;
    const int CENTER_Y = 768/2;
    bool rWasDown = false;
    const float sensitivity = 0.0025f;
private:
    vec3 position;

    float horizontalAngle;
    float verticalAngle;

    float speed;
    float mouseSpeed;
    float cameraHeight;
    float cameraRadius;

    mat4 viewMatrix;
};