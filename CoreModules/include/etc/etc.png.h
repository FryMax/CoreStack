#pragma once
#include "../shared_in.h"

LIB_NAMESPACE_START

// TODO
//move to io manager
bitmap32 png_read_image_file(const std::string& file);
void png_write_image_file(const bitmap32& img, const std::string& file);

LIB_NAMESPACE_END