// Copyright (C) 2023 akiaji-k

#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H
#include "stdint.h"

#include "tiffconf.h"
#include "tiff.h"
enum class Process {
    ReadImages,
    RoiIrradiated,
    RoiUnIrradiated,
    RoiNormalize,
    WriteImages,
};


struct TiffTags {
    uint32_t image_width = 0;
    uint32_t image_length = 0;
    uint16_t compression = 0;
    float x_resolution = 0.0;
    float y_resolution = 0.0;
    uint16_t resolution_unit = 0;
};

#endif // CUSTOM_TYPES_H
