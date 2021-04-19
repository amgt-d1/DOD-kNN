#include "data.hpp"
#include <chrono>
#include <unistd.h>


// variable for time measure
std::chrono::system_clock::time_point start, end, init;

// computation time
double time_vptree_build = 0;
double time_threshold = 0;
double time_threshold_2= 0;
double time_filter = 0;
double time_verify = 0;

// #verified objects
unsigned int count_verify = 0;

// memory usage
double memory = 0;

// top-n threshold
float threshold = 0;


// compute memory usage
double process_mem_usage() {

    double resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    resident_set = rss * page_size_kb;

	return resident_set / 1000;
}

// result output
void output_result() {

    std::string f_name = "result/id(" + std::to_string(dataset_id) + ")_N(" + std::to_string(n) + ")_k(" + std::to_string(k) + ")_thread_num(" + std::to_string(thread_num) + ").csv";

    std::ofstream file;
    file.open(f_name.c_str(), std::ios::out | std::ios::app);

    if (file.fail()) {
        std::cout << " cannot open the result file.\n";
        file.clear();
        return;
    }

    file << "Threasholding time [sec]" << "," << time_threshold / 1000 << "\n";
    file << "Filtering time [sec]" << "," << time_filter / 1000 << "," << "threshold" << "," << threshold << "\n";
    file << "Verification time [sec]" << "," << time_verify / 1000 << "\n";
    file << "Running time [sec]" << "," << (time_threshold + time_filter + time_verify) / 1000 << "\n";
    file << "Index size [MB]" << "," << memory << "\n";
    file << "Verified count: " << "," << count_verify << "," << "Pruning ratio" << "," << 1.0 - (double)(count_verify + n) / dataset.size() << "\n\n"; 
}
