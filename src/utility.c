#include "utility.h"

/* randm
 * Takes in an integer, and uses it as a max for the random
 * number it outputs. Supposed to work like Random.nextInt() in
 * java.
 */
int randm (int max) {
        return rand() % max;
}

/* nmod
 * Modulo operator that acts like the java one.
 */
int nmod (int left, int right) {
        left %= right;
        if (left < 0) {
                left += right;
        }
        return left;
}

int perlin2d_noise2 (int x, int y, u_int8_t *hash, int seed) {
        static int tmp;
        tmp = hash[(y + seed) % 256];
        return hash[(tmp + x) % 256];
}

double perlin2d_lerp (double x, double y, double s) {
        return x + s * s * (3 - 2 * s) * (y - x);
}

/* perlin2d
 * Simple 2D perlin noise function.
 */
double perlin2d (
        int seed,
        double x, double y,
        double freq
) {
        static u_int8_t hash[256] =

        {208,34,231,213,32,248,233,56,161,78,24,140,
        71,48,140,254,245,255,247,247,40,185,248,251,245,28,124,204,
        204,76,36,1,107,28,234,163,202,224,245,128,167,204,9,92,217,
        54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,
        204,8,81,70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,
        169,112,32,97,53,195,13,203,9,47,104,125,117,114,124,165,203,
        181,235,193,206,70,180,174,0,167,181,41,164,30,116,127,198,245,
        146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,228,108,
        245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,
        237,55,89,232,50,217,64,244,157,199,121,252,90,17,212,203,149,
        152,140,187,234,177,73,174,193,100,192,143,97,53,145,135,19,
        103,13,90,135,151,199,91,239,247,33,39,145,101,120,99,3,186,86,
        99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,135,
        176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,
        137,139,255,114,20,218,113,154,27,127,246,250,1,8,198,250,209,
        92,222,173,21,88,102,219};

        double xa = x * freq; //0.0625 TODO: update chunkGen TODO: figure out wtf that means
        double ya = y * freq; //0.0625
        double amp = 1.0;
        double fin = 0;
        double div = 0.0;

        int i;
        for(i = 0; i < 4; i++) {
                div += 256 * amp;

                int x_int = xa;
                int y_int = ya;
                double x_frac = xa - x_int;
                double y_frac = ya - y_int;
                int s = perlin2d_noise2(x_int, y_int, hash, seed);
                int t = perlin2d_noise2(x_int + 1, y_int,     hash, seed);
                int u = perlin2d_noise2(x_int,     y_int + 1, hash, seed);
                int v = perlin2d_noise2(x_int + 1, y_int + 1, hash, seed);
                double low  = perlin2d_lerp(s, t, x_frac);
                double high = perlin2d_lerp(u, v, x_frac);

                fin += perlin2d_lerp(low, high, y_frac) * amp;
                amp /= 2;
                xa *= 2;
                ya *= 2;
        }

        return fin/div;
}

/* dist2d
 * Find the distance between two points in two dimensional space.
 */
double dist2d (double x1, double y1, double x2, double y2) {
        return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

/* dist3d
 * Find the distance between two points in three dimensional space.
 */
double dist3d (double x1, double y1, double z1, double x2, double y2, double z2) {
        return sqrt (
                pow(x1 - x2, 2) +
                pow(y1 - y2, 2) +
                pow(z1 - z2, 2));
}
