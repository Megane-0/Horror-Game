#ifndef GHOST_MOVEMENT_H
#define GHOST_MOVEMENT_H

#include "VectorUtils4.h"
#include <vector>

extern vec3 gGhostPos;
extern float gGhostYaw;
extern float gGhostSpeed;
extern float gGhostYawOffset;

void initGhost(vec3 startPos, float speed, std::vector<vec3> route);

void updateGhost(float deltaTime);

mat4 getGhostMoveMatrix();

#endif