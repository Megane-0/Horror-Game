#pragma once

#include "VectorUtils4.h"

class Camera {
public:
    Camera();

    void update(float deltaTime);
    void processMouse(float dx, float dy);
    mat4 getViewMatrix() const;
    vec3 getPosition() const;

    const float sensitivity = 0.0025f;

    
private:
    float horizontalAngle;
    float verticalAngle;
    vec3 position;
    float speed;
    float mouseSpeed;
    float cameraHeight;
    float cameraRadius;

    mat4 viewMatrix;
};