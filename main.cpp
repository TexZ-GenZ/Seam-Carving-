#include <iostream>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Pixel
{
    unsigned char r, g, b; // Red, Green, Blue components
};

bool loadImageTo2DVector(const std::string &filename,
                         std::vector<std::vector<Pixel>> &image_data,
                         int &width,
                         int &height)
{
    int channels; // Number of color channels (e.g., 3 for RGB, 4 for RGBA)

    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        std::cerr << "Error: Could not load image from " << filename << std::endl;
        return false;
    }

    image_data.resize(height);
    for (int i = 0; i < height; ++i)
    {
        image_data[i].resize(width);
    }

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int pixel_start_index = (i * width + j) * channels;
            image_data[i][j].r = data[pixel_start_index];
            image_data[i][j].g = data[pixel_start_index + (channels > 1 ? 1 : 0)];
            image_data[i][j].b = data[pixel_start_index + (channels > 2 ? 2 : 0)];
        }
    }

    stbi_image_free(data);

    return true;
}

int main()
{
    std::string input_image_path = "img.jpg";
    std::vector<std::vector<Pixel>> my_image;
    int img_width, img_height;

    std::cout << "Attempting to load image: " << input_image_path << std::endl;

    if (loadImageTo2DVector(input_image_path, my_image, img_width, img_height))
    {
        std::cout << "Image loaded successfully!" << std::endl;
        std::cout << "Dimensions: " << img_width << "x" << img_height << std::endl;

        if (img_height > 0 && img_width > 0)
        {
            std::cout << "Top-left pixel (R,G,B): ("
                      << static_cast<int>(my_image[0][0].r) << ", "
                      << static_cast<int>(my_image[0][0].g) << ", "
                      << static_cast<int>(my_image[0][0].b) << ")" << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to load image." << std::endl;
    }

    return 0;
}