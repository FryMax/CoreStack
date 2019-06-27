#include "etc.png.h"
#include "../../3rd-party/lodepng/lodepng.h"

LIB_NAMESPACE_START

bitmap32 png_read_image_file(const std::string& file)
{
    std::vector<std::uint8_t> fileData;
    std::vector<std::uint8_t> decoded;

    unsigned int file_w = 0;
    unsigned int file_h = 0;

    lodepng::load_file(fileData, file);
    lodepng::decode(decoded, file_w, file_h, fileData, LodePNGColorType::LCT_RGBA, 8U);

    bitmap32 out;
    INT2 out_size = INT2{ file_w, file_h };
    out.copy_from_ptr(&decoded[0], out_size);

    return out;
}

void png_write_image_file(const bitmap32& img, const std::string& file)
{
    std::vector<byte> Output(img.data_size());
    memcpy(&Output[0], img.raw_data(), img.data_size());

    //enum { NO_ALPHA_CHANNEL = false };
    //if (NO_ALPHA_CHANNEL)
    //{
    //    ForArray(i, img.pixel_count())
    //        img.raw_data(i)->a = 255;
    //}

    std::vector<byte> ImageBuffer;
    lodepng::encode(ImageBuffer, Output, img.size().x, img.size().y, LodePNGColorType::LCT_RGBA, 8U);
    lodepng::save_file(ImageBuffer, file);
}

LIB_NAMESPACE_END