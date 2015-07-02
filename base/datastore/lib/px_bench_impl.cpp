#include "px_bench.h"

#include <iostream>

void px_bench_run(PxBenchData *a_bench_data) {
  a_bench_data->start_time = std::chrono::high_resolution_clock::now();
}

void px_bench_stop(PxBenchData *a_bench_data) {
  a_bench_data->end_time = std::chrono::high_resolution_clock::now();
  a_bench_data->elapsed_seconds =
      (a_bench_data->end_time - a_bench_data->start_time);
}

void px_bench_print(PxBenchData *a_bench_data, const char *a_func_name) {
  std::cout << a_func_name << "Duration -> "
            << std::chrono::duration_cast<std::chrono::microseconds>(
                   a_bench_data->elapsed_seconds).count() << std::endl;
}
