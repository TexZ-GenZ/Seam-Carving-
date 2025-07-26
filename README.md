# Seam Carving Image Resizer

This project implements a basic Seam Carving algorithm in C++ for content-aware image resizing. It allows you to reduce the width of an image by intelligently removing "least important" pixel seams, preserving more of the image's visual content compared to simple scaling.

## Features

* **Image Loading:** Loads common image formats (JPEG, PNG, etc.) using `stb_image.h`.
* **Energy Map Calculation:** Computes an energy map for the image, highlighting areas of high contrast.
* **Minimum Seam Finding:** Uses dynamic programming to find the vertical seam with the lowest energy.
* **Seam Removal:** Removes the identified seam, reducing the image width by one pixel.
* **Step-by-Step Output:** Saves intermediate images to a `resized_output` folder, showing each step of the resizing process with the removed seam highlighted in red, and the image after removal.

## How to Compile and Run

### Prerequisites

* **C++17 Compiler:** (e.g., g++ 7.x or newer)
* **`stb_image.h`:** Place this header file in your project directory.
    * [stb_image.h on GitHub](https://github.com/nothings/stb/blob/master/stb_image.h)

### Compilation

Navigate to your project directory in the terminal and use the following command:

```bash
g++ main.cpp -o seam_carver -std=c++17 -lstdc++fs
````

  * `-std=c++17`: Enables C++17 features, specifically `std::filesystem`.
  * `-lstdc++fs`: Links the filesystem library (might be needed on some systems, especially older g++ versions).

### Running the Application

Run the compiled executable from your terminal, providing the input image path and the target width as arguments:

```bash
./seam_carver img.jpg 300
```

  * Replace `img.jpg` with the actual path to your input image.
  * Replace `300` with your desired target width.

The program will create a `resized_output` folder (and clear it if it already exists), saving intermediate images (`highlighted_step_X.png`, `resized_step_X.png`) and the `final_resized_XXXxYYY.png` image.

## Example Images

Here are some examples of the process:

**Original Image (`img.jpg`):**

![Original Image](https://github.com/user-attachments/assets/9900bf7b-1eda-431c-ae46-e6abedcde290)

**Highlighted Steps (`highlighted_step_n.png`):**

![Highlighted Step 1](https://github.com/user-attachments/assets/37e5ce96-c8b7-47f8-831e-a238271b0372)
![Highlighted Step 86](https://github.com/user-attachments/assets/794c5300-5379-4bf0-b2e1-7771e016cc38)
![Highlighted Step 226](https://github.com/user-attachments/assets/cde79a33-aa5b-4ab6-9afe-feb4f574714c)

**Final Resized Image (`final_resized_300x360.png`):**

![Final Resized Image](https://github.com/user-attachments/assets/508aefd5-0f24-4b39-90c0-286382f47c62)

## `.gitignore`

```
stb_image.h
main.exe
resized_output/
stb_image_write.h
```
