/**
 * @file stb_impl.cpp
 * @brief STB Image Write implementation file
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * This file contains the STB Image Write implementation to avoid multiple
 * definition errors when the library is included in multiple source files.
 */

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h> 