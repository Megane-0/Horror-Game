#include "GhostMovement.h"
#include <math.h>


static std::vector<vec3> sRoute;
static int sTargetIndex = 0;
static int sDirection = 1; // 1 = forward, -1 = backward


vec3 gGhostPos = vec3(0,0,0);
float gGhostYaw = 0.0f;
float gGhostSpeed = 1.0f;
float gGhostYawOffset = 0.0f;


void initGhost(vec3 startPos, float speed, std::vector<vec3> route) {
    // Setup up initial stuff
    gGhostPos = startPos;
    gGhostSpeed = speed;
    sRoute = route;
    sTargetIndex = 0;
    sDirection = 1;
}

void updateGhost(float deltaTime) {
    if (sRoute.empty()) return;

    vec3 target = sRoute[sTargetIndex];
    
    // Calculate direction we are going to
    vec3 diff = target - gGhostPos;
    diff.y = 0; 

    // Calculate distance (will be used to check if we're there)
    float distance = sqrt(diff.x * diff.x + diff.z * diff.z);
    float moveStep = gGhostSpeed * deltaTime;

    // We are there means distance is less then our next step
    if (distance <= moveStep) {
        gGhostPos.x = target.x;
        gGhostPos.z = target.z;

        // Start looking at next coord
        sTargetIndex += sDirection;

        // The ping pong logic
        if (sTargetIndex >= (int)sRoute.size()) {
            sTargetIndex = sRoute.size() - 2;
            sDirection = -1;
        } else if (sTargetIndex < 0) {
            sTargetIndex = 1;
            sDirection = 1;
        }
    } else {
        // Move to next step
        vec3 dir = normalize(diff);
        gGhostPos.x += dir.x * moveStep;
        gGhostPos.z += dir.z * moveStep;

        // And rotate
        gGhostYaw = atan2(dir.x, dir.z) + gGhostYawOffset;
    }
}

mat4 getGhostMoveMatrix() {
    // Get a matrix that is also translated and rotated
    return T(gGhostPos.x, gGhostPos.y, gGhostPos.z) * Ry(gGhostYaw);
}