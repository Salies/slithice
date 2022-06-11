/*
    Slithice - C++ image processing library
    Copyright (C) 2022 Daniel Serezane

    This file is part of Slithice.

    Slithice is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slithice is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slithice.  If not, see <https://www.gnu.org/licenses/>
 */

#ifndef SLITHICE_H
#define SLITHICE_H

#pragma once

#include <vector>
#include <functional>

namespace slithice {
    using uint = unsigned int;
    using uchar = unsigned char;

    // Converts a RGB32 image to Grayscale8.
    void to_grayscale(uint* in, int w, int h, uchar* out);

    // Splits an RGB32 image in (R, G, B).
    void split_rgb(uint *in, int w, int h, uint *r_out, uint *g_out, uint *b_out);

    // Invert a Grayscale8 image.
    void invert_gray(uchar* in, int w, int h, uchar* out);

    // Invert a RGB32 image.
    void invert(uint* in, int w, int h, uint* out);

    // Builds histogram for a Grayscale8 image.
    void build_hisogram(uchar* in, int w, int h, std::vector<double> &out);

    // Histogram equalization for a Grayscale 8 image.
    void eq_histogram(uchar* in, int w, int h, std::vector<double>& his, uchar* out);

    // Adds 10% of salt and pepper noise to a Grayscale8 image.
    void add_salt_pepper(uchar* in, int w, int h, uchar* out);

    // Executes convolution on a Grayscale8 image according to param: kernel.
    void convolution(uchar *in, int w, int h, float* kernel, int kWidth, int kHeight, uchar *out);
    // Overloaded convolution: normalizes values by lambda function f(x).
    void convolution(uchar* in, int w, int h, float* kernel, int kWidth, int kHeight, std::function<void(int*)> f, uchar* out);
    // Overloaded convolution: outputs (int) values. Useful when the tone range is wide and normalization is not straightforward.
    void convolution(uchar* in, int w, int h, float* kernel, int kWidth, int kHeight, int *out, int &min, int &max);

    // Binarizes a Grayscale8 image.
    void binarize(uchar* in, int w, int h, uchar* out);

    // Applies dynamic range compression to a Grayscale8 image. Output is to be normalized.
    void dynamic_range_compression(uchar* in, int w, int h, float c, float gamma, int* out);

    // Normalizes a Grayscale8 image.
    void normalize(int* in, int w, int h, int max, int min, uchar* out);

    // Converts a RGB color to HSL (mspaint standard).
    void rgb_to_hsl(uchar r, uchar g, uchar b, uchar& h, uchar& s, uchar& l);

    // Converts a HSL color (mspaint standard) to RGB.
    void hsl_to_rgb(uchar h, uchar s, uchar l, uchar& r, uchar& g, uchar& b);
} // namespace slithice

#endif // SLITHICE_H