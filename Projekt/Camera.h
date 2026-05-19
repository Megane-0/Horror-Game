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
    

    mat4 viewMatrix;
};