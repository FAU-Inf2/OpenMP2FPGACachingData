#ifndef CMDLINE_H
#define CMDLINE_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct cmdline {
  uint32_t iterations;
  uint32_t res_x;
  uint32_t res_y;
  uint32_t batch;
};

struct cmdline *cmdline_get_data();
int cmdline_parse(int argc, char **argv);
#endif
