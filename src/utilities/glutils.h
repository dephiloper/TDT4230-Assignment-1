#pragma once

#include "mesh.h"
#include "utilities/imageLoader.hpp"
#include "utilities/glfont.h"

unsigned int generateBuffer(Mesh &mesh);
unsigned int generateTexture(PNGImage &image);