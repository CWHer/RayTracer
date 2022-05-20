#include <iostream>
#include <fstream>
#include <cstdio>
#include "../raytracer.h"
using namespace std;

const char *filename[] = {
    "image1.ppm", "image2.ppm",
    "image3.ppm", "image4.ppm",
    "image5.ppm", "image6.ppm"};
// "image7.ppm", "image8.ppm",
// "image9.ppm", "image10.ppm"};
constexpr auto aspect_ratio = 1.0;
const int image_width = 3000;
constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
Color w[image_height][image_width];

int main()
{
    ios::sync_with_stdio(0);
    int samples_per_pixel = 0;
    for (const auto &file : filename)
    {
        char temp[20];
        ifstream fin(file);
        if (!fin)
            continue;
        samples_per_pixel++;
        fin.getline(temp, 15);
        fin.getline(temp, 15);
        fin.getline(temp, 15);
        for (int j = image_width - 1; ~j; --j)
            for (int i = 0; i < image_width; ++i)
            {
                int r, g, b;
                fin >> r >> g >> b;
                w[j][i] += Color(r, g, b) / 255;
            }
    }
    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >= 0; --j)
        for (int i = 0; i < image_width; ++i)
            write_color(std::cout, w[j][i], samples_per_pixel);
    return 0;
}
