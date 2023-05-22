#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lin(int x, int y, int M) { return x + y * M; }

void matrix_mult(int *a, int *b, int *c, int M) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      c[lin(i, j, M)] = 0;
      for (int k = 0; k < M; k++) {
        int x = a[lin(i, k, M)];
        int y = b[lin(k, j, M)];
        c[lin(i, j, M)] += x * y;
      }
    }
  }
}

void gen_matrix(int *mat, int M) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      mat[lin(i, j, M)] = rand() % 100;
    }
  }
}

void print_matrix(int *mat, int M) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      printf("%d ", mat[lin(i, j, M)]);
    }
    printf("\n");
  }
}

char comp_matrix(int *a, int *b, int M) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      if (a[lin(i, j, M)] != b[lin(i, j, M)]) {
        return 1;
      }
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  int M = 20;
  char show = 0;
  if (argc > 1) {
    M = atoi(argv[1]);
  }
  if (argc > 2) {
    show = !strcmp(argv[2], "-s");
  }
  srand(M);
  int *a = malloc(M * M * sizeof(int));
  int *b = malloc(M * M * sizeof(int));
  int *c_cpu = malloc(M * M * sizeof(int));
  int *c_fpga = malloc(M * M * sizeof(int));

  gen_matrix(a, M);
  gen_matrix(b, M);

  matrix_mult(a, b, c_cpu, M);

#pragma omp target map(to                                                      \
                       : a [0:M * M], b [0:M * M], M) map(tofrom               \
                                                          : c_fpga [0:M * M])
  { matrix_mult(a, b, c_fpga, M); }

  if (show) {
    printf("Matrix a:\n");
    print_matrix(a, M);
    printf("Matrix b:\n");
    print_matrix(b, M);
    printf("Matrix c:\n");
    print_matrix(c_cpu, M);
  }

  if (comp_matrix(c_cpu, c_fpga, M)) {
    printf("Matrix c_cpu:\n");
    print_matrix(c_cpu, M);
    printf("Matrix c_fpga:\n");
    print_matrix(c_fpga, M);
    printf("Output Mismatch\n");
    exit(EXIT_FAILURE);
  }

  printf("Computation Successfull!\n");

  free(a);
  free(b);
  free(c_cpu);
  free(c_fpga);
}
