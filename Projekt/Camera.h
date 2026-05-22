#pragma once

#include "VectorUtils4.h"

class Camera {
public:
    Camera();

    void update(float deltaTime);
    void processMouse(float dx, float dy);
    mat4 getViewMatrix() const;
    vec3 getPosition() const;
    vec3 getDirection() const;
    void setPosition(vec3 _position);
    vec3 getForward() const;
    vec3 getRight() const;
    vec3 getUp() const;

    const float sensitivity = 0.0025f;
    float cameraRadius;

    
private:
    float horizontalAngle;
    float verticalAngle;
    vec3 position;
    vec3 direction;
    float speed;
    float mouseSpeed;
    float cameraHeight;

    vec3 forward;
    vec3 right;
    vec3 up;
    

    mat4 viewMatrix;
};