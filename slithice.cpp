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

#include "slithice.h"
#include <random>
#include <vector>
#include <cmath>

// Converts a RGB32 image to Grayscale8.
void slithice::to_grayscale(const QImage& in, QImage& out) {
    uchar* data_out;
    for (int j = 0; j < in.height(); j++) {
        const QRgb *data_in = (QRgb*)(in.constScanLine(j));
        data_out = out.scanLine(j);
        for (int i = 0; i < in.width(); i++) {
            // Funny how the compiler doesn't complain I'm assigning an int value
            // to an unsigned char. La garant�a soy yo!
            data_out[i] = (qRed(data_in[i]) + qGreen(data_in[i]) + qBlue(data_in[i])) / 3;
        }
    }
}

// Splits an RGB32 image in (R, G, B).
void slithice::split_rgb(const QImage& in, QImage& r_out, QImage& g_out, QImage& b_out) {
    QRgb* redBits = nullptr, * greenBits = nullptr, * blueBits = nullptr;
    for (int j = 0; j < in.height(); j++) {
        const QRgb* data_in = (QRgb*)(in.constScanLine(j));
        redBits = (QRgb*)(r_out.scanLine(j));
        greenBits = (QRgb*)(g_out.scanLine(j));
        blueBits = (QRgb*)(b_out.scanLine(j));
        for (int i = 0; i < in.width(); i++) {
            redBits[i] = data_in[i] & 0xffff0000;
            greenBits[i] = data_in[i] & 0xff00ff00;
            blueBits[i] = data_in[i] & 0xff0000ff;
        }
    }
}

// Invert a Grayscale8 image.
void slithice::invert_gray(const QImage& in, QImage& out) {
    uchar* bits;
    for (int j = 0; j < in.height(); j++) {
        const uchar* in_bits = in.constScanLine(j);
        bits = out.scanLine(j);
        for (int i = 0; i < in.width(); i++)
            bits[i] = 255 - in_bits[i];
    }
}

// Invert a RGB32 image.
void slithice::invert(const QImage& in, QImage& out) {
    QRgb* data_out;
    for (int j = 0; j < in.height(); j++) {
        const QRgb* data_in = (QRgb*)(in.constScanLine(j));
        data_out = (QRgb*)(out.scanLine(j));
        for (int i = 0; i < in.width(); i++)
            data_out[i] = qRgb(255 - qRed(data_in[i]), 255 - qGreen(data_in[i]), 255 - qBlue(data_in[i]));
    }
}

// Builds histogram for a Grayscale8 image.
void slithice::build_histogram(const QImage& in, std::vector<double>& out) {
    out.reserve(256);
    out.resize(256);
    std::fill(out.begin(), out.end(), 0);

    for (int j = 0; j < in.height(); j++) {
        const uchar* in_bits = in.constScanLine(j);
        for (int i = 0; i < in.width(); i++)
            out[in_bits[i]]++;
    }
}

// Histogram equalization for a Grayscale 8 image.
// param: in will be an equalized version of itself.
void slithice::eq_histogram(QImage& in, std::vector<double>& h) {
    int freq_acc = 0, width = in.width(), height = in.height(), i;
    uchar* lut = new uchar[256]; // lookup table para economizar processamento
    float escala = 255.0f / (width * height);

    for (i = 0; i < 256; i++) {
        freq_acc += h[i];
        lut[i] = std::max(0, (uchar)(freq_acc * escala) - 1);
    }

    uchar* bits;
    std::fill(h.begin(), h.end(), 0);
    for (int j = 0; j < height; j++) {
        bits = in.scanLine(j);
        for (i = 0; i < width; i++) {
            bits[i] = lut[bits[i]];
            h[bits[i]]++;
        }
    }

    delete[] lut;
}

// Adds 10% of salt and pepper noise to a Grayscale8 image.
void slithice::add_salt_pepper(const QImage& in, QImage& out) {
    int size = in.width() * in.height();
    int colors[2] = { 0, 255 };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, size - 1);
    std::uniform_int_distribution<> distCor(0, 1);

    out = in.copy();
    uchar* bits = out.scanLine(0);
    for (int i = 0; i < size / 10; i++)
        bits[dist(gen)] = colors[distCor(gen)];
}

// Executes convolution on a Grayscale8 image according to param: kernel.
void slithice::convolution(const QImage& in, float* kernel, int k_width, int k_height, QImage& out) {
    out = in.copy(); // keep corners

    const int img_width = in.width(), img_height = in.height(),
        k_center_j = k_width >> 1, k_center_i = k_height >> 1;
    int offsetJ = 0, offsetI = 0, lim_j = 0, lim_i = 0, acc_color;
    uchar* bitsB;

    for (int j = k_center_j; j < img_height - k_center_j; j++) {
        bitsB = out.scanLine(j);
        for (int i = k_center_i; i < img_width - k_center_i; i++) {
            acc_color = 0;
            for (int mj = 0; mj < k_height; mj++) {
                // The convolution kernel is mirrored.
                offsetJ = k_height - mj - 1;
                lim_j = j + k_center_i - offsetJ;
                const uchar *bits = in.constScanLine(lim_j);
                for (int mi = 0; mi < k_width; mi++) {
                    offsetI = k_width - mi - 1;
                    lim_i = i + k_center_j - offsetI;
                    if (lim_j >= 0 && lim_j < img_height && lim_i >= 0 && lim_i < img_width) {
                        acc_color += bits[lim_i] * kernel[(k_width * offsetJ) + offsetI];
                    }
                }
            }
            bitsB[i] = acc_color;
        }
    }
}

// Overloaded convolution: normalizes values by lambda function f(x).
void slithice::convolution(const QImage& in, float* kernel, int k_width, int k_height, std::function<void(int*)> f, QImage& out) {
    out = in.copy();

    const int img_width = in.width(), img_height = in.height(),
        k_center_j = k_width >> 1, k_center_i = k_height >> 1;
    int offsetJ = 0, offsetI = 0, lim_j = 0, lim_i = 0, acc_color;
    uchar* bitsB;

    for (int j = k_center_j; j < img_height - k_center_j; j++) {
        bitsB = out.scanLine(j);
        for (int i = k_center_i; i < img_width - k_center_i; i++) {
            acc_color = 0;
            for (int mj = 0; mj < k_height; mj++) {
                offsetJ = k_height - mj - 1;
                lim_j = j + k_center_i - offsetJ;
                const uchar* bits = in.constScanLine(lim_j);
                for (int mi = 0; mi < k_width; mi++) {
                    offsetI = k_width - mi - 1;
                    lim_i = i + k_center_j - offsetI;
                    if (lim_j >= 0 && lim_j < img_height && lim_i >= 0 && lim_i < img_width) {
                        acc_color += bits[lim_i] * kernel[(k_width * offsetJ) + offsetI];
                    }
                }
            }
            f(&acc_color);
            bitsB[i] = acc_color;
        }
    }
}
// Overloaded convolution: outputs (int) values. Useful when the tone range is wide and normalization is not straightforward.
void slithice::convolution(const QImage& in, float* kernel, int k_width, int k_height, int* out) {
    const int img_width = in.width(), img_height = in.height(),
        k_center_j = k_width >> 1, k_center_i = k_height >> 1;
    int offsetJ = 0, offsetI = 0, lim_j = 0, lim_i = 0, acc_color;
    uchar* bitsB;

    for (int j = k_center_j; j < img_height - k_center_j; j++) {
        for (int i = k_center_i; i < img_width - k_center_i; i++) {
            acc_color = 0;
            for (int mj = 0; mj < k_height; mj++) {
                // The convolution kernel is mirrored.
                offsetJ = k_height - mj - 1;
                lim_j = j + k_center_i - offsetJ;
                const uchar* bits = in.constScanLine(lim_j);
                for (int mi = 0; mi < k_width; mi++) {
                    offsetI = k_width - mi - 1;
                    lim_i = i + k_center_j - offsetI;
                    if (lim_j >= 0 && lim_j < img_height && lim_i >= 0 && lim_i < img_width) {
                        acc_color += bits[lim_i] * kernel[(k_width * offsetJ) + offsetI];
                    }
                }
            }
            out[(img_width * j) + i] = acc_color;
        }
    }
}

void slithice::median_filter(const QImage& in, int m_width, int m_height, QImage& out) {
    out = in.copy();

    const int img_width = in.width(), img_height = in.height(),
        m_center_j = m_width >> 1, m_center_i = m_height >> 1, m_center = (m_width * m_height) >> 1;
    int offsetJ = 0, offsetI = 0, lim_j = 0, lim_i = 0;
    int pos; // current position of the median vector
    uchar *bitsB;
    std::vector<uchar> mdn(m_width * m_height);

    for (int j = m_center_j; j < img_height - m_center_j; j++) {
        bitsB = out.scanLine(j);
        for (int i = m_center_i; i < img_width - m_center_i; i++) {
            pos = 0;
            for (int mj = 0; mj < m_height; mj++) {
                offsetJ = m_height - mj - 1;
                lim_j = j + m_center_i - offsetJ;
                const uchar *bits = in.constScanLine(lim_j);
                for (int mi = 0; mi < m_width; mi++) {
                    offsetI = m_width - mi - 1;
                    lim_i = i + m_center_j - offsetI;
                    if (lim_j >= 0 && lim_j < img_height && lim_i >= 0 && lim_i < img_width) {
                        mdn[pos] = bits[lim_i];
                        pos++;
                    }
                }
            }
            std::sort(mdn.begin(), mdn.end());
            bitsB[i] = mdn[m_center];
        }
    }
}

// Binarizes a Grayscale8 image.
void slithice::binarize(const QImage& in, QImage& out) {
    uchar* bits_out;
    for (int j = 0; j < in.height(); j++) {
        bits_out = out.scanLine(j);
        const uchar* bits_in = in.constScanLine(j);
        for (int i = 0; i < in.width(); i++) {
            if (bits_in[i] < 128) {
                bits_out[i] = 0;
                continue;
            }
            bits_out[i] = 255;
        }
    }
}

void slithice::sobel(const QImage& in, int* dx, int* dy, int* mag, QImage& out) {
    int w = in.width(), h = in.height(), max = -256, min = 256;
    float sobel_x[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 }, sobel_y[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
    convolution(in, sobel_x, 3, 3, dx);
    convolution(in, sobel_y, 3, 3, dy);

    // Calculating the magnitude and extracting max and min
    for (int i = 1; i < w - 1; i++) {
        for (int j = 1; j < h - 1; j++) {
            mag[(w * j) + i] = (int)std::sqrt((dx[(w * j) + i] * dx[(w * j) + i]) + (dy[(w * j) + i] * dy[(w * j) + i]));
            if (mag[(w * j) + i] > max)
                max = mag[(w * j) + i];
            if (mag[(w * j) + i] < min)
                min = mag[(w * j) + i];
        }
    }

    // Normalizing magnitude
    out = QImage(w, h, QImage::Format_Grayscale8);
    normalize(mag, max, min, out);
}

// Applies dynamic range compression to a Grayscale8 image. Output is normalized.
void slithice::dynamic_range_compression(const QImage& in, float c, float gamma, QImage& out) {
    int w = in.width(), h = in.height(), max = 0, min = (int)(c * std::pow(in.scanLine(0)[0], gamma));
    int* comp = new int[w * h];

    for (int j = 0; j < h; j++) {
        const uchar *bits = in.constScanLine(j);
        for (int i = 0; i < w; i++) {
            comp[(w * j) + i] = (int)(c * std::pow(bits[i], gamma));
            if (comp[(w * j) + i] < min)
                min = comp[(w * j) + i];
            if (comp[(w * j) + i] > max)
                max = comp[(w * j) + i];
        }
    }

    // Normalize the output
    normalize(comp, max, min, out);
    delete[] comp;
}

// Normalizes a Grayscale8 image.
void slithice::normalize(int *in, int max, int min, QImage& out) {
    if (max == min) return;

    uchar* bits;
    int h = out.height(), w = out.width();
    for (int j = 0; j < h; j++) {
        bits = out.scanLine(j);
        for (int i = 0; i < w; i++)
            bits[i] = ((in[(w * j) + i] - min) / (float)(max - min)) * 255;
    }
}

void slithice::thresholding(const QImage& in, uchar threshold, QImage& out) {
    out = in.copy();

    uchar* bits;
    for (int j = 0; j < in.height(); j++) {
        bits = out.scanLine(j);
        for (int i = 0; i < in.width(); i++)
            if (bits[i] < threshold) bits[i] = 0;
    }
}

// Converts a RGB color to HSL (mspaint standard).
// Based on: https://www.rapidtables.com/convert/color/rgb-to-hsl.html
void slithice::rgb_to_hsl(uchar r, uchar g, uchar b, uchar* h, uchar* s, uchar* l) {
    	float rr, gg, bb, cmax, cmin, delta, hh = 0, ll = 0;
	rr = r / 255.0f;
	gg = g / 255.0f;
	bb = b / 255.0f;
	cmax = std::max({ rr, gg, bb });
	cmin = std::min({ rr, gg, bb });
	ll = (cmax + cmin) / 2.0f;
	if (cmax == cmin)
		*h = *s = 0;
	// Calcula H e S
	else {
		float delta = cmax - cmin;
		// S est� na escala de 0...1, converter p/ 240
		*s = std::round((delta / (1.0f - std::abs((2 * ll) - 1.0f))) * 240.f);
		if (cmax == rr)
			hh = ((gg - bb) / delta) + (gg < bb ? 6.0f : 0.0f);
		else if (cmax == gg)
			hh = ((bb - rr) / delta) + 2.0f;
		else if (cmax == bb)
			hh = ((rr - gg) / delta) + 4.0f;

		if (hh < 0.0f)
			hh += 360.f;
		hh *= 60.0f;
		// h foi dado na escala 360, converter para 240
		// j� arredonda para depois converter p/ inteiro
		// lembrando que o valor nunca vai chegar a 240
		// pois na escala de 360 ele nunca chega a 360 (0 <= H < 360)
		hh = std::round((hh * 2.0f) / 3.0f);
		*h = hh;
	}
	// Calcula L
	*l = std::round(ll * 240.0f);
}


// Converts a HSL color (mspaint standard) to RGB.
// Based on: https://en.wikipedia.org/wiki/HSL_and_HSV ("HSL to RGB alternative")
void slithice::hsl_to_rgb(uchar h, uchar s, uchar l, uchar* r, uchar* g, uchar* b) {
    auto hsl_f = [](int n, float H, float S, float L) {
        float k = std::fmod((n + (H / 30.0f)), 12.0f);
        return std::round((L - (S * std::min(L, 1.0f - L)) * std::max(-1.0f, std::min({ k - 3.0f, 9.0f - k, 1.0f }))) * 255.0f);
    };

    float ll = l / 240.0f;
    // Imagem with no saturation (grayscale)
    if (s == 0) {
        *r = *g = *b = std::round(ll * 255.0f);
        return;
    }

    float hh = (h / 2.0f) * 3.0f, ss = s / 240.0f, rr = 0, gg = 0, bb = 0;

    *r = hsl_f(0, hh, ss, ll);
    *g = hsl_f(8, hh, ss, ll);
    *b = hsl_f(4, hh, ss, ll);
}