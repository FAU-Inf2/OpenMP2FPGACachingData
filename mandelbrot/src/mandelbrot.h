#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stdlib.h>

int *mandelbrot(double *real, double *imag, size_t size, size_t iterations);

// Checks a set of mandelbrot results for correctness
// small deviations are allowed
void mandelbrot_assert_result(double *real, double *imag, size_t size,
                              size_t iterations, int *results_to_check);

#endif
