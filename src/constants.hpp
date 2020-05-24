#pragma once

#include <vector>
#include <string>

const std::vector<std::string> allModelNames{ "fish", "can1", "can2", "can3", "can4", "diver", "bottle1", "bottle2", "bottle3", "sixpack1", "sixpack2", "sixpack3", "background" };
const std::vector<std::string> allShaderNames{ "player", "testing", "sceneobject", "background", "collectible"};

constexpr float COLLECTIBLESCALE = 0.2f;
constexpr float PLAYERSCALE = 0.5f;
constexpr float DOMERADIUS = 7.4f;