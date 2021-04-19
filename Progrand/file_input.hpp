#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <time.h>


// dataset identifier
unsigned int dataset_id = 0;

// data dimensionality
unsigned int dimensionality = 2;

// distance function type
unsigned int type = 0;

// top-n
unsigned int n = 0;

// knn
unsigned int k = 0;

// #threads
unsigned int thread_num = 1;

// sys parameter (vp-tree pr not)
unsigned int mode = 0;


// get current time
void get_current_time() {

	time_t t = time(NULL);
	printf(" %s\n", ctime(&t));
}

// parameter input
void input_parameter() {

	std::ifstream ifs_dataset_id("parameter/dataset_id.txt");
	std::ifstream ifs_n("parameter/n.txt");
	std::ifstream ifs_k("parameter/k.txt");
	std::ifstream ifs_thread("parameter/thread_num.txt");

	if (ifs_dataset_id.fail()) {
		std::cout << " dataset_id.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_n.fail()) {
		std::cout << " n.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_k.fail()) {
		std::cout << " k.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_thread.fail()) {
		std::cout << " thread_num.txt does not exist." << std::endl;
		std::exit(0);
	}

	while (!ifs_dataset_id.eof()) { ifs_dataset_id >> dataset_id; }
	while (!ifs_n.eof()) { ifs_n >> n; }
	while (!ifs_k.eof()) { ifs_k >> k; }
	while (!ifs_thread.eof()) { ifs_thread >> thread_num; }

	// type 0: L2
	// type 1: L1
	// type 2: edit distance
	// type 3: angular distance
	// type 4: L4

	// set dimensionality & distance function: for example...
	//if (dataset_id == 0) {
	//	dimensionality = 128;
	//}

	std::cout << " File input done\n";
}
