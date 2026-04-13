#include "Camera.h"
#include "MicroGlut.h"
// https://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/#the-actual-code

// External terrain function
extern float getHeight(float x, float z);

Camera::Camera()
{
    position = vec3(0.5, 3, 0.7);
    horizontalAngle = 0; //3.14
    verticalAngle = 0.0f;

    speed = 5.0f;
    cameraHeight = 1.8f;
    cameraRadius = 0.5f;
}
void Camera::processMouse(float dx, float dy)
{
    horizontalAngle -= dx * sensitivity;
    verticalAngle   += dy * sensitivity;

    if (verticalAngle > 1.5f) verticalAngle = 1.5f;
    if (verticalAngle < -1.5f) verticalAngle = -1.5f;
}

void Camera::update(float deltaTime)
{
    // direction vector 
    vec3 direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    vec3 right = vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    vec3 up = cross(right, direction);

    //  movement 
    float frameSpeed = speed * deltaTime;

    if (glutKeyIsDown('w'))
        position = position + direction * frameSpeed;

    if (glutKeyIsDown('s'))
        position = position - direction * frameSpeed;

    if (glutKeyIsDown('d'))
        position = position + right * frameSpeed;

    if (glutKeyIsDown('a'))
        position = position - right * frameSpeed;
    
    // terrain constraint
    float terrainHeight = getHeight(position.x, position.z);
    if (position.y < terrainHeight + cameraHeight)
        position.y = terrainHeight + cameraHeight;

    //  view matrix 
    vec3 target = position + direction;

    viewMatrix = lookAt(
        position.x, position.y, position.z,
        target.x, target.y, target.z,
        up.x, up.y, up.z
    );
}

mat4 Camera::getViewMatrix() const
{
    return viewMatrix;
}

vec3 Camera::getPosition() const
{
    return position;
}