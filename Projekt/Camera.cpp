#include "Camera.h"
#include "MicroGlut.h"
// https://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/#the-actual-code

// External terrain function
extern float getHeight(float x, float z);

Camera::Camera()
{
    position = vec3(0, 5, 0);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;

    speed = 5.0f;
    mouseSpeed = 0.0002f;
}

void Camera::update(float deltaTime, int mouseX, int mouseY)
{
    int centerX = 300;
    int centerY = 300;
    
    
    
    // Mouse input 
    horizontalAngle += mouseSpeed  * (centerX - mouseX); // * deltaTime
    verticalAngle   += mouseSpeed  * (centerY - mouseY); // * deltaTime

    // Clamp pitch
    if (verticalAngle > 1.5f) verticalAngle = 1.5f;
    if (verticalAngle < -1.5f) verticalAngle = -1.5f;

    ignoreMouse = true;
    // Reset cursor
    glutWarpPointer(centerX, centerY);

    // Direction 
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

    // Movement 
    if (glutKeyIsDown('w'))
        position = position + direction * (deltaTime * speed);

    if (glutKeyIsDown('s'))
        position = position - direction * (deltaTime * speed);

    if (glutKeyIsDown('d'))
        position = position + right * (deltaTime * speed);

    if (glutKeyIsDown('a'))
        position = position - right * (deltaTime * speed);

    // Terrain constraint
    position.y = getHeight(position.x, position.z);

    // View matrix
    viewMatrix = lookAt(
        position.x, position.y, position.z,
        position.x + direction.x,
        position.y + direction.y,
        position.z + direction.z,
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