#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "cmdline.h"
#include "mandelbrot.h"

int main(int argc, char **argv) {
  cmdline_parse(argc, argv);
  int resx = cmdline_get_data()->res_x;
  int resy = cmdline_get_data()->res_y;
  camera_set_resolution(resx, resy);
  camera_set_position(-0.5, 0, 4);
  double *camera_vals_real, *camera_vals_imag;
  camera_get_values(&camera_vals_real, &camera_vals_imag);
  int *mandelbrot_data =
      mandelbrot(camera_vals_real, camera_vals_imag, resx * resy,
                 cmdline_get_data()->iterations);
  mandelbrot_assert_result(camera_vals_real, camera_vals_imag, resx * resy,
                           cmdline_get_data()->iterations, mandelbrot_data);

  free(mandelbrot_data);
  free(camera_vals_real);
  free(camera_vals_imag);
}
