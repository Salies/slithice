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
#include <QImage>

namespace slithice {
    using uint = unsigned int;
    using uchar = unsigned char;

    // Converts a RGB32 image to Grayscale8.
    void to_grayscale(const QImage &in, QImage &out);

    // Splits an RGB32 image in (R, G, B).
    void split_rgb(const QImage& in, QImage& r_out, QImage& g_out, QImage& b_out);

    // Invert a Grayscale8 image.
    void invert_gray(const QImage& in, QImage& out);

    // Invert a RGB32 image.
    void invert(const QImage& in, QImage& out);

    // Builds histogram for a Grayscale8 image.
    void build_histogram(const QImage& in, std::vector<double> &out);

    // Histogram equalization for a Grayscale 8 image.
    void eq_histogram(QImage& in, std::vector<double>& h);

    // Adds 10% of salt and pepper noise to a Grayscale8 image.
    void add_salt_pepper(const QImage& in, QImage& out);

    // Executes convolution on a Grayscale8 image according to param: kernel.
    void convolution(const QImage& in, float* kernel, int kWidth, int kHeight, QImage& out);
    // Overloaded convolution: normalizes values by lambda function f(x).
    void convolution(const QImage& in, float* kernel, int kWidth, int kHeight, std::function<void(int*)> f, QImage& out);
    // Overloaded convolution: outputs (int) values. Useful when the tone range is wide and normalization is not straightforward.
    void convolution(const QImage& in, float* kernel, int kWidth, int kHeight, int *out, int &min, int &max);

    // Binarizes a Grayscale8 image.
    void binarize(const QImage& in, QImage& out);

    // Applies dynamic range compression to a Grayscale8 image. Output is to be normalized.
    void dynamic_range_compression(const QImage& in, float c, float gamma, uint* out);

    // Normalizes a Grayscale8 image.
    void normalize(const QImage& in, int max, int min, QImage& out);

    // Converts a RGB color to HSL (mspaint standard).
    void rgb_to_hsl(uchar r, uchar g, uchar b, uchar& h, uchar& s, uchar& l);

    // Converts a HSL color (mspaint standard) to RGB.
    void hsl_to_rgb(uchar h, uchar s, uchar l, uchar& r, uchar& g, uchar& b);
} // namespace slithice

#endif // SLITHICE_H