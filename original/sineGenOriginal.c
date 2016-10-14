#include < stdio.h >
#include < stdint.h >
#include < stdlib.h >
#include < math.h >

int16_t         pcm[BUFSIZ];

double          mults[] = {
    1.0 / 2.0,
    1.0 / 3.0,
    1.0 / 4.0,
    2,
    3,
    4,
    9.0 / 8.0,
    81.0 / 64.0,
    4.0 / 3,
    3.0 / 2,
    27.0 / 16,
    243.0 / 128,
    2
};

int
main()
{

    int             q,
                    size = 10000000;
    double          p = 0;
    double          incr = 10000;
    int             len;
    double          shift;
    srand(time(NULL));

    double          amp;

    while (size > 0) {
    len = (rand() % (BUFSIZ - 2000)) + 2000;
    shift = (double) ((rand() % 500) - 250) / 20.0;
    for (q = 0; q < len; q++) {
        amp = 30000;
        if (q < 1000) {
            amp = (double) q / 1000 * 30000;
        }
        if (q > len - 1000) {
            amp = (double) (len - q) / 1000 * 30000;
        }
        pcm[q] = (int16_t) (sin(p / 10000.0) * amp);
        p += incr;
        incr += shift;
        shift *= .999;
    }
    size -= fwrite(pcm, sizeof(int16_t), len, stdout);//pipe in to ffmpeg
    // incr+=(rand()%3)-1;
    incr *= mults[rand() % (sizeof(mults) / sizeof(double))];

    if (incr < 45000) {
        incr = 45000;
    }
    if (incr > 55000) {
        incr = 55000;
    }

    }
    fprintf(stderr, "Ya ne segfolt!\n");
    return EXIT_SUCCESS;
}
