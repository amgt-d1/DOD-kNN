#include "progrand.hpp"


int main() {

    // input parameter
    input_parameter();

    // input dataset
    input_data();

    // input graph
    memory = process_mem_usage();
    input_graph();
    memory = process_mem_usage() - memory;

    // print the current time
    get_current_time();

    std::cout << " --------------------\n";
	std::cout << " dataset id: " << dataset_id << "\n";
	std::cout << " dataset cardinality: " << dataset.size() << "\n";
	std::cout << " dataset dimensionality: " << dimensionality << "\n";
    std::cout << " N: " << n << "\n";
    std::cout << " k: " << k << "\n";
    std::cout << " number of threads: " << thread_num << "\n";
	std::cout << " --------------------\n\n";


    // pre-processing
    pre_processing();

    // online processing
    online_processing();

    // output result
    output_result();

    return 0;
}