#ifndef PX_BENCH_H
#define PX_BENCH_H

#include <chrono>

typedef struct {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
  std::chrono::duration<double> elapsed_seconds;
} PxBenchData;

extern void px_bench_run(PxBenchData *a_bench_data);

extern void px_bench_stop(PxBenchData *a_bench_data);

extern void px_bench_print(PxBenchData *a_bench_data, const char *a_func_name);

#endif // PX_BENCH_H
