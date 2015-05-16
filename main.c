#include "fft.h"
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

void testfft(char* filename) {
    unsigned char *data;
    int x, y, comp;

    FILE *file = fopen(filename, "rb");

    if (!file) {
        return;
    }

    data = stbi_load_from_file(file, &x, &y, &comp, STBI_rgb_alpha);

    float** m = malloc2d(x, y);

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            m[i][j] = (float) data[i + j * x];
        }
    }

    free2d(m);

    stbi_image_free(data);
}

int main(int argc, char** argv) {
	return 0;
}
