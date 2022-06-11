#include "slithice.h"
#include <random>

// Converts a RGB32 image to Grayscale8.
void slithice::to_grayscale(const QImage& in, QImage& out) {
    uchar* data_out;
    for (int j = 0; j < in.height(); j++) {
        const QRgb *data_in = (QRgb*)(in.constScanLine(j));
        data_out = out.scanLine(j);
        for (int i = 0; i < in.width(); i++) {
            // Funny how the compiler doesn't complain I'm assigning an int value
            // to an unsigned char. La garantía soy yo!
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
}

// Adds 10% of salt and pepper noise to a Grayscale8 image.
void slithice::add_salt_pepper(const QImage& in, QImage& out) {
    int size = in.width() * in.height();
    int cores[2] = { 0, 255 };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, size - 1);
    std::uniform_int_distribution<> distCor(0, 1);

    out = in.copy();
    uchar* bits = out.scanLine(0);
    for (int i = 0; i < size / 10; i++)
        bits[dist(gen)] = cores[distCor(gen)];
}

// Executes convolution on a Grayscale8 image according to param: kernel.
void convolution(const QImage& in, float* kernel, int kWidth, int kHeight, QImage& out);
// Overloaded convolution: normalizes values by lambda function f(x).
void convolution(const QImage& in, float* kernel, int kWidth, int kHeight, std::function<void(int*)> f, QImage& out);
// Overloaded convolution: outputs (int) values. Useful when the tone range is wide and normalization is not straightforward.
void convolution(const QImage& in, float* kernel, int kWidth, int kHeight, int* out, int& min, int& max);

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