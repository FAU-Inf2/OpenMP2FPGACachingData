#include "cmdline.h"

#include <stdlib.h>

static struct cmdline data = {.res_x = 500, .res_y = 500, .iterations = 100};

static void usage(int argc, char **argv) {
  fprintf(stderr, "%s [--iter <iterations>] [-batch] [X Y]\n", argv[0]);
  exit(EXIT_FAILURE);
}

int cmdline_parse(int argc, char **argv) {
  if (argc == 2 && !strcmp(argv[1], "-h")) {
    usage(argc, argv);
  }
  int cur = 1;
  while (argc > cur) {
    if (!strcmp(argv[cur], "--iter")) {
      if (argc == cur++)
        usage(argc, argv);
      data.iterations = atoi(argv[cur++]);
    } else if (!strcmp(argv[cur], "-batch")) {
      cur++;
      data.batch = 1;
    } else {
      data.res_x = atoi(argv[cur++]);
      if (cur == argc)
        usage(argc, argv);
      data.res_y = atoi(argv[cur++]);
    }
  }
  return 0;
}

struct cmdline *cmdline_get_data() {
  return &data;
};
