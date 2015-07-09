/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
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
