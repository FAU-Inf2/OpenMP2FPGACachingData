#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mov_dat(int *from, int *to, int n) { memcpy(to, from, n * sizeof(int)); }

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

void sort(int *array, int *buf, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = i % 2; j < n - 1; j += 2) {
#pragma HLS unroll factor = 64
      if (load(array, j) > load(array, j + 1)) {
        store(buf, j, load(array, j + 1));
        store(buf, j + 1, load(array, j));
      } else {
        store(buf, j, load(array, j));
        store(buf, j + 1, load(array, j + 1));
      }
    }
    mov_dat(buf, array, n);
  }
}

int ref_cmp(const void *a, const void *b) {
  int ai = *((const int *)a);
  int bi = *((const int *)b);
  return ai > bi;
}

int main(int argc, char **argv) {
  srand(0);
  int n = 15000;
  if (argc > 1)
    n = atoi(argv[1]);
  int *array = malloc(sizeof(int) * n);
  for (int i = 0; i < n; i++) {
    array[i] = rand();
  }
  int *ref = malloc(sizeof(int) * n);
  memcpy(ref, array, n * sizeof(int));
  qsort(ref, n, sizeof(int), ref_cmp);

  int *buf = malloc(sizeof(int) * n);

#pragma omp target map(tofrom : array [0:n], buf [0:n]) map(to : n)
  { sort(array, buf, n); }
  char wrong = 0;
  for (int i = 0; i < n; i++) {
    if (array[i] != ref[i])
      wrong = 1;
  }
  if (wrong) {
    for (int i = 0; i < n; i++) {
      printf("%d,%d\n", array[i], ref[i]);
    }
    printf("OUTPUT MISMATCH!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("COMPUTATION SUCCESFULL!\n");
  }
  free(array);
}
