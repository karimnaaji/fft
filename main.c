#include "fft.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void free2d(float** p) { 
    free(p[0]);
    free(p);
}

float** malloc2d(int v, int h) {
    int i;
    float** m;
    float* p;

    m = (float**) malloc(sizeof(float*) * v);
    p = (float*) malloc(sizeof(float) * h * v);

    for (i = 0; i < v; i++, p += h) {
        m[i] = p;
    }

    return m;
}

int powerOf2(unsigned int v) {
    return v > 0 && (v & (v - 1)) == 0;
}


unsigned int toRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return (r) | (g << 8) | (b << 16) | (a << 24);
}

void center(float** in, float** out, int length, int width) {
    int i, j;

    for(i = 0; i < length; i++) {
        for(j = 0; j < width; j++) {
            if(i < length / 2) {
                if(j < width / 2) {
                    out[i][j] = in[i + length / 2][j + width / 2];
                } else {
                    out[i][j] = in[i + length / 2][j - width / 2];
                }
            } else {
                if(j < width / 2) {
                    out[i][j] = in[i - length / 2][j + width / 2];
                } else {
                    out[i][j] = in[i - length / 2][j - width / 2];
                }
            }
        }
    }
}

void testfft(char* filename, char* spectrum) {
    unsigned char* data;
    unsigned int* modfft;
    float** m, **modulus, **mim;
    int x, y, comp;
    int offset = 0;

    FILE *file = fopen(filename, "rb");

    if (!file) {
        fclose(file);
        return;
    }

    data = stbi_load_from_file(file, &x, &y, &comp, STBI_rgb_alpha);

    if (x != y || !powerOf2(x)) {
        printf("Warning: Image dimensions must be a power of 2\n");
        goto error;
    }

    m = malloc2d(x, y);
    modulus = malloc2d(x, y);

    printf("width %d, height %d\n", x, y);

    for (int j = 0; j < y; ++j) {
        for (int i = 0; i < x; ++i) { 
            int off = i + j * x + offset;
            m[i][j] = (float) data[off] + (float) data[off + 1] + (float) data[off + 2];
            m[i][j] /= comp - 1;
            offset += comp - 1;
        }
    }

    mim = malloc2d(x, y);

    // perform fast fourier transform on the 2d signal (red channel)
    fft(m, mim, x, y);

    // compute the modulus of the fourier transform
    mod(modulus, m, mim, x, y);

    // shift the fourier transform to move the zero frequency in center
    fftshift(modulus, x, y);

    // expand the modulus (for viewing purposes)
    mult(modulus, 100.0f, x, y); 

    center(modulus, m, x, y);

    modfft = (unsigned int*) malloc(sizeof(unsigned int) * x * y * comp);

    for (int j = 0; j < y; ++j) {
        for (int i = 0; i < x; ++i) {
            unsigned char v = (unsigned char)m[i][j];
            modfft[i + j * x] = toRGBA(v, v, v, 255);
        }
    }

    int res = stbi_write_png(spectrum, x, y, comp, modfft, 0);

    if (!res) {
        printf("Error saving file\n");
    }

    free2d(modulus);
    free2d(mim);
    free2d(m);
    free(modfft);

error:
    stbi_image_free(data);
    fclose(file);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: fft.out image_in [image_out]\n");
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        testfft(argv[1], argv[2]);
    } else {
        testfft(argv[1], "spectrum.png");
    }

	return EXIT_SUCCESS;
}
