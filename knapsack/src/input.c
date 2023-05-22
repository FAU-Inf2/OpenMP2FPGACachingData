#include <stdio.h>
#include <stdlib.h>

void gen_input(int *values, int *weights, int num_items) {
  for (int i = 0; i < num_items; i++) {
    values[i] = (rand() % 1000) + 1;
    weights[i] = (rand() % 1000) + 1;
  }
}

size_t lin(int x, int y, int dim_x) {
#pragma hls inline
  return x + y * dim_x;
}

// Workaround for known axi protocol specific bugs on Xilinx IP cores (similar
// to https://zipcpu.com/formal/2019/05/13/axifull.html,
// https://github.com/esa-tu-darmstadt/tapasco/issues/234) Although performance
// is negatively impacted by the workaround, it has been shown to not change the
// relative speedup for the purpose of the publication
int load(int *arr, int idx) {
#pragma HLS inline off
  return arr[idx];
}

int store(int *arr, int idx, int val) {
#pragma HLS inline off
  return arr[idx] = val;
}

int solve(int num_items, int space, int *sizes, int *values, int *res) {
  int x = num_items + 1;
  for (int j = 0; j < space + 1; j++) {
    store(res, lin(0, j, x), 0);
  }
  for (int i = 1; i < num_items + 1; i++) {
    int size = load(sizes, i - 1);
    int value = load(values, i - 1);
    for (int j = 0; j < space + 1; j++) {
      int base = load(res, lin(i - 1, j, x));
      if (size <= j) {
        int opt = load(res, lin(i - 1, j - size, x)) + value;
        if (opt > base)
          store(res, lin(i, j, x), opt);
        else
          store(res, lin(i, j, x), base);
      } else {
        store(res, lin(i, j, x), base);
      }
    }
  }
  return load(res, lin(num_items, space, x));
}

void print_problem(int num_items, int space, int *sizes, int *values) {
  printf("num items %d\nspace %d\n", num_items, space);
  printf("values \n");
  for (int i = 0; i < num_items; i++) {
    printf("%d ", values[i]);
  }
  printf("\n");
  printf("sizes \n");
  for (int i = 0; i < num_items; i++) {
    printf("%d ", sizes[i]);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  if (argc == 2) {
    fprintf(stderr, "usage: %s [num_items space\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int num_items = 100;
  int space = 300;
  if (argc >= 3) {
    num_items = atoi(argv[1]);
    space = atoi(argv[2]);
  }
  srand(num_items + space * 17);
  int *values = malloc(sizeof(int) * num_items);
  int *sizes = malloc(sizeof(int) * num_items);
  int buf_len = (num_items + 1) * (space + 1);
  int *buf = malloc(sizeof(int) * buf_len);
  gen_input(values, sizes, num_items);
  int res_cpu = solve(num_items, space, sizes, values, buf);
  int res_fpga;
  int *res_fpgap = &res_fpga;

#pragma omp target map(to                                                      \
                       : sizes [0:num_items], values [0:num_items],            \
                         buf [0:buf_len], num_items, space)                    \
    map(tofrom                                                                 \
        : res_fpgap [0:1])
  { *res_fpgap = solve(num_items, space, sizes, values, buf); }

  // print_problem(num_items,space,sizes,values);
  if (res_cpu != res_fpga) {
    printf("result CPU: %d\n", res_cpu);
    printf("result FPGA: %d\n", res_fpga);
    fprintf(stderr, "MISMATCH!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("Computation Successfull!\nresult: %d\n", res_fpga);
  }

  free(values);
  free(sizes);
  free(buf);
}
