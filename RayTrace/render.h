#pragma once
#include<vector>
#include"Point.h"
#include <cstdint>

void render(std::vector<uint32_t>& image, size_t width, size_t height, int threads = 1,int scene=1);

Point rgb(uint32_t color);
