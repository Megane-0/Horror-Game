#pragma once

#include "VectorUtils4.h"

class Camera {
public:
    Camera();

    void update(float deltaTime, int mouseX, int mouseY);

    mat4 getViewMatrix() const;
    vec3 getPosition() const;
    bool ignoreMouse = false;

private:
    vec3 position;

    float horizontalAngle;
    float verticalAngle;

    float speed;
    float mouseSpeed;

    mat4 viewMatrix;
};