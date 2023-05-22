#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include <stdlib.h>

void camera_set_position(double center_x, double center_y, double width);
void camera_set_resolution(uint32_t width, uint32_t height);
void camera_get_values(double **real, double **imag);
size_t camera_linearize(uint32_t x, uint32_t y);
void camera_zoom_in(int pos_x, int pos_y);
void camera_zoom_out();
uint32_t camera_get_resolution_x();
uint32_t camera_get_resolution_y();
#endif
