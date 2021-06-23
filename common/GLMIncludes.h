#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // OpenGL使用的默认深度范围是[-1,1]
#define GLM_FORCE_LEFT_HANDED // 强制使用左手坐标系，注意，定义GLM_LEFT_HANDED宏是没用的
#include "glm-0.9.9.7/glm/glm/glm.hpp"
#include "glm-0.9.9.7/glm/glm/gtc/matrix_transform.hpp"