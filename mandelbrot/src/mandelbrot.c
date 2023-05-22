#include "mandelbrot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int *mandelbrot(double *real, double *imag, size_t size, size_t iterations) {
  int *result = malloc(sizeof(int) * size);
  double *dat = malloc(sizeof(double) * size * 2);
  memcpy(dat, real, sizeof(double) * size);
  memcpy(dat + size, imag, sizeof(double) * size);

  printf("computing mandelbrot... ");
  struct timeval a, b, sub;
  gettimeofday(&a, NULL);
#pragma omp target map(to : dat [0:size * 2]) map(tofrom : result [0:size])
  {
    for (size_t i = 0; i < size; i++) {
      result[i] = -1;
      double result_real = 0;
      double result_imag = 0;
      double c_real = dat[i];
      double c_imag = dat[i + size];
      for (size_t iter = 0; iter < iterations; iter++) {
        double new_real =
            result_real * result_real - result_imag * result_imag + c_real;
        double new_imag = result_real * result_imag * 2 + c_imag;
        double abs = new_real * new_real + new_imag * new_imag;
        // printf("%f %f | %d : %f\n", c_real, c_imag, iter, abs);
        if (abs > 4) {
          result[i] = iter;
          break;
        }
        result_real = new_real;
        result_imag = new_imag;
      }
    }
  }
  gettimeofday(&b, NULL);
  timersub(&b, &a, &sub);

  printf("took %ld.%06ld s\n", (long int)sub.tv_sec, (long int)sub.tv_usec);

  return result;
}

// implementation without pragmas
static int *mandelbrot2(double *real, double *imag, size_t size,
                        size_t iterations) {
  int *result = malloc(sizeof(int) * size);
  for (size_t i = 0; i < size; i++) {
    result[i] = -1;
    double result_real = 0;
    double result_imag = 0;
    double c_real = real[i];
    double c_imag = imag[i];
    for (size_t iter = 0; iter < iterations; iter++) {
      double new_real =
          result_real * result_real - result_imag * result_imag + c_real;
      double new_imag = result_real * result_imag * 2 + c_imag;
      double abs = new_real * new_real + new_imag * new_imag;
      // printf("%f %f | %d : %f\n", c_real, c_imag, iter, abs);
      if (abs > 4) {
        result[i] = iter;
        break;
      }
      result_real = new_real;
      result_imag = new_imag;
    }
  }
  return result;
}

void mandelbrot_assert_result(double *real, double *imag, size_t size,
                              size_t iterations, int *results_to_check) {
  printf("checking mandelbrot computation... ");
  int *res = mandelbrot2(real, imag, size, iterations);
  size_t hits = 0;
  for (size_t i = 0; i < size; i++) {
    char a = (res[i] == -1);
    char b = (results_to_check[i] == -1);
    if (a == b)
      hits++;
  }
  double ratio = (double)hits / (double)size;
  printf("%f precision\n", ratio);
  if (ratio < 0.9) {
    fprintf(stderr, "Mandelbrot computation incorrect, deviation too large!\n");
    exit(EXIT_FAILURE);
  }
}
