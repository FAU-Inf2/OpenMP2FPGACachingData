#include "camera.h"

#include <stdio.h>

static double camera_center_x, camera_center_y, camera_width;
static uint32_t resolution_width, resolution_height;
static double zoom_factor = 1.6;

void camera_set_position(double center_x, double center_y, double width) {
  camera_center_x = center_x;
  camera_center_y = center_y;
  camera_width = width;
}

uint32_t camera_get_resolution_x() { return resolution_width; }

uint32_t camera_get_resolution_y() { return resolution_height; }

void camera_set_resolution(uint32_t width, uint32_t height) {
  resolution_width = width;
  resolution_height = height;
}

void camera_get_values(double **real, double **imag) {
  double *res_real =
      malloc(resolution_width * resolution_height * sizeof(double));
  double *res_imag =
      malloc(resolution_width * resolution_height * sizeof(double));
  double aspect_ratio = (double)resolution_height / (double)resolution_width;
  double camera_height = camera_width * aspect_ratio;
  for (uint32_t y = 0; y < resolution_height; y++) {
    for (uint32_t x = 0; x < resolution_width; x++) {
      size_t array_idx = camera_linearize(x, y);
      double relative_x = (double)x / resolution_width;
      double relative_y = (double)y / resolution_height;
      double res_x = (relative_x - 0.5) * camera_width + camera_center_x;
      double res_y = (relative_y - 0.5) * camera_height + camera_center_y;
      res_real[array_idx] = res_x;
      res_imag[array_idx] = res_y;
    }
  }
  *real = res_real;
  *imag = res_imag;
}

size_t camera_linearize(uint32_t x, uint32_t y) {
  return x + y * resolution_width;
}

void camera_zoom_in(int pos_x, int pos_y) {
  double aspect_ratio = (double)resolution_height / (double)resolution_width;
  double camera_height = camera_width * aspect_ratio;
  double relative_x = (double)pos_x / resolution_width;
  double relative_y = (double)pos_y / resolution_height;
  double new_x = (relative_x - 0.5) * camera_width + camera_center_x;
  double new_y = (relative_y - 0.5) * camera_height + camera_center_y;
  camera_set_position(new_x, new_y, camera_width / zoom_factor);
}
void camera_zoom_out() {
  camera_set_position(camera_center_x, camera_center_y,
                      camera_width * zoom_factor);
}
