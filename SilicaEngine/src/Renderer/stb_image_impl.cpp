/**
 * @file stb_image_impl.cpp
 * @brief STB Image implementation compilation unit
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Separate compilation unit for STB Image to avoid multiple definition issues.
 */

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// This file exists solely to compile the STB image implementation
// in a single translation unit to avoid linker conflicts.