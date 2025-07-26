#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Pixel {
    unsigned char r, g, b;
};

bool loadImageTo2DVector(const std::string& filename,
                          std::vector<std::vector<Pixel>>& image_data,
                          int& width,
                          int& height) {
    int channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Error: Could not load image from " << filename << std::endl;
        return false;
    }

    image_data.resize(height);
    for (int i = 0; i < height; ++i) {
        image_data[i].resize(width);
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixel_start_index = (i * width + j) * channels;
            image_data[i][j].r = data[pixel_start_index];
            image_data[i][j].g = data[pixel_start_index + (channels > 1 ? 1 : 0)];
            image_data[i][j].b = data[pixel_start_index + (channels > 2 ? 2 : 0)];
        }
    }

    stbi_image_free(data);
    return true;
}

bool saveImageFrom2DVector(const std::string& filename,
                           const std::vector<std::vector<Pixel>>& image_data,
                           int width,
                           int height) {
    std::vector<unsigned char> flat_data(width * height * 3);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int flat_index = (i * width + j) * 3;
            flat_data[flat_index + 0] = image_data[i][j].r;
            flat_data[flat_index + 1] = image_data[i][j].g;
            flat_data[flat_index + 2] = image_data[i][j].b;
        }
    }

    if (stbi_write_png(filename.c_str(), width, height, 3, flat_data.data(), width * 3)) {
        return true;
    } else {
        std::cerr << "Error: Could not save image to " << filename << std::endl;
        return false;
    }
}

void calculateEnergyMap(const std::vector<std::vector<Pixel>>& image_data,
                        std::vector<std::vector<double>>& energy_map,
                        int width,
                        int height) {
    energy_map.resize(height);
    for (int i = 0; i < height; ++i) {
        energy_map[i].resize(width);
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                energy_map[i][j] = 1000.0;
                continue;
            }

            double energy_x = 0.0;
            double energy_y = 0.0;

            energy_x += std::abs(image_data[i][j+1].r - image_data[i][j-1].r);
            energy_x += std::abs(image_data[i][j+1].g - image_data[i][j-1].g);
            energy_x += std::abs(image_data[i][j+1].b - image_data[i][j-1].b);

            energy_y += std::abs(image_data[i+1][j].r - image_data[i-1][j].r);
            energy_y += std::abs(image_data[i+1][j].g - image_data[i-1][j].g);
            energy_y += std::abs(image_data[i+1][j].b - image_data[i-1][j].b);

            energy_map[i][j] = energy_x + energy_y;
        }
    }
}

std::vector<int> findMinVerticalSeam(const std::vector<std::vector<double>>& energy_map,
                                     int width,
                                     int height) {
    std::vector<std::vector<double>> dp_cost(height, std::vector<double>(width));

    for (int j = 0; j < width; ++j) {
        dp_cost[0][j] = energy_map[0][j];
    }

    for (int i = 1; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double min_prev_cost = std::numeric_limits<double>::max();

            min_prev_cost = std::min(min_prev_cost, dp_cost[i-1][j]);

            if (j > 0) {
                min_prev_cost = std::min(min_prev_cost, dp_cost[i-1][j-1]);
            }

            if (j < width - 1) {
                min_prev_cost = std::min(min_prev_cost, dp_cost[i-1][j+1]);
            }

            dp_cost[i][j] = energy_map[i][j] + min_prev_cost;
        }
    }

    std::vector<int> seam_path(height);

    double min_total_cost = std::numeric_limits<double>::max();
    int last_row_seam_col = -1;

    for (int j = 0; j < width; ++j) {
        if (dp_cost[height-1][j] < min_total_cost) {
            min_total_cost = dp_cost[height-1][j];
            last_row_seam_col = j;
        }
    }
    seam_path[height-1] = last_row_seam_col;

    for (int i = height - 2; i >= 0; --i) {
        int current_col = seam_path[i+1];

        double min_val = std::numeric_limits<double>::max();
        int next_seam_col = -1;

        min_val = dp_cost[i][current_col];
        next_seam_col = current_col;

        if (current_col > 0) {
            if (dp_cost[i][current_col - 1] < min_val) {
                min_val = dp_cost[i][current_col - 1];
                next_seam_col = current_col - 1;
            }
        }

        if (current_col < width - 1) {
            if (dp_cost[i][current_col + 1] < min_val) {
                min_val = dp_cost[i][current_col + 1];
                next_seam_col = current_col + 1;
            }
        }
        seam_path[i] = next_seam_col;
    }

    return seam_path;
}

std::vector<std::vector<Pixel>> removeVerticalSeam(const std::vector<std::vector<Pixel>>& original_image,
                                                   const std::vector<int>& seam_path,
                                                   int original_width,
                                                   int height,
                                                   int& new_width) {
    new_width = original_width - 1;
    std::vector<std::vector<Pixel>> new_image(height, std::vector<Pixel>(new_width));

    for (int i = 0; i < height; ++i) {
        int seam_col = seam_path[i];
        int current_new_col = 0;
        for (int j = 0; j < original_width; ++j) {
            if (j == seam_col) {
                continue;
            }
            new_image[i][current_new_col] = original_image[i][j];
            current_new_col++;
        }
    }
    return new_image;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_image_path> <target_width>" << std::endl;
        return 1;
    }

    std::string input_image_path = argv[1];
    int target_width = std::stoi(argv[2]);

    std::vector<std::vector<Pixel>> original_full_image;
    int original_full_width, original_full_height;

    if (!loadImageTo2DVector(input_image_path, original_full_image, original_full_width, original_full_height)) {
        std::cerr << "Failed to load image." << std::endl;
        return 1;
    }

    if (target_width <= 0 || target_width >= original_full_width) {
        std::cerr << "Error: Target width must be greater than 0 and less than original width (" << original_full_width << ")." << std::endl;
        return 1;
    }

    std::cout << "Image loaded: " << original_full_width << "x" << original_full_height << std::endl;
    std::cout << "Target width: " << target_width << std::endl;

    std::vector<std::vector<Pixel>> current_image = original_full_image;
    int current_width = original_full_width;
    int current_height = original_full_height;

    std::string output_dir_name = "resized_output";
    if (std::filesystem::exists(output_dir_name)) {
        for (const auto& entry : std::filesystem::directory_iterator(output_dir_name)) {
            std::filesystem::remove_all(entry.path());
        }
    }
    std::filesystem::create_directory(output_dir_name);

    int step_count = 0;
    while (current_width > target_width) {
        std::cout << "Resizing step " << ++step_count << ": Current width " << current_width << std::endl;

        std::vector<std::vector<double>> energy_map;
        calculateEnergyMap(current_image, energy_map, current_width, current_height);

        std::vector<int> vertical_seam = findMinVerticalSeam(energy_map, current_width, current_height);

        std::vector<std::vector<Pixel>> highlighted_step_image = current_image;
        for (int i = 0; i < current_height; ++i) {
            int seam_col = vertical_seam[i];
            highlighted_step_image[i][seam_col].r = 255;
            highlighted_step_image[i][seam_col].g = 0;
            highlighted_step_image[i][seam_col].b = 0;
        }
        std::string highlighted_filename = output_dir_name + "/highlighted_step_" + std::to_string(step_count) + ".png";
        if (!saveImageFrom2DVector(highlighted_filename, highlighted_step_image, current_width, current_height)) {
            std::cerr << "Failed to save highlighted intermediate image " << highlighted_filename << std::endl;
        }

        int new_width_after_removal;
        current_image = removeVerticalSeam(current_image, vertical_seam, current_width, current_height, new_width_after_removal);
        current_width = new_width_after_removal;

        std::string step_filename = output_dir_name + "/resized_step_" + std::to_string(step_count) + ".png";
        if (!saveImageFrom2DVector(step_filename, current_image, current_width, current_height)) {
            std::cerr << "Failed to save intermediate image " << step_filename << std::endl;
        }
    }

    std::string final_resized_path = output_dir_name + "/final_resized_" + std::to_string(target_width) + "x" + std::to_string(original_full_height) + ".png";
    if (saveImageFrom2DVector(final_resized_path, current_image, current_width, current_height)) {
        std::cout << "Final resized image saved to " << final_resized_path << std::endl;
    } else {
        std::cerr << "Failed to save final resized image." << std::endl;
    }

    return 0;
}
