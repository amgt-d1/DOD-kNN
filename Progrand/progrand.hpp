#include "vptree.hpp"
#include <omp.h>


// top-n list
std::multimap<float, unsigned int, std::greater<float>> top;

// top-n update
void update_top(const float distance, const unsigned int id) {

    if (top.size() < n) {
		top.insert({distance, id});
	}
	else {
		if (distance > threshold) top.insert({distance, id});
	}

	// erase n+1 idx
	if (top.size() > n) {
		auto it = top.end();
		--it;
		top.erase(it);
	}

	// update threshold
	if (top.size() == n) {
		auto it = top.end();
		--it;
		threshold = it->first;
	}
}

// virtual dataset sorted by degree-based threshold
std::vector<std::pair<float, unsigned int>> pair_of_id_ths;

// random partition
std::vector<unsigned int> random_partition;

// vp-tree
vp_tree vpt;


// pre-processing
void pre_processing() {

    const unsigned int thread_max = 48;

    // set degree-based threshold
	#pragma omp parallel num_threads(thread_max)
    {
        #pragma omp for schedule(static) 
		for (unsigned int i = 0; i < dataset.size(); ++i) {
			dataset[i].update_degree_distance();
		}
	}

	// sort by degree-based threshold
    pair_of_id_ths.resize(dataset.size());
    #pragma omp parallel num_threads(thread_max)
    {
        #pragma omp for schedule(static) 
		for (unsigned int i = 0; i < dataset.size(); ++i) {
			pair_of_id_ths[i].first = dataset[i].degree_distance;
            pair_of_id_ths[i].second = i;
		}
    }
	std::sort(pair_of_id_ths.begin(), pair_of_id_ths.end(), std::greater<std::pair<float, unsigned int>>());

    // build vp-tree
    if (mode == 1) vpt.build();
}

// get threshold
void get_threshold() {

    start = std::chrono::system_clock::now();

    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static) 
        for (unsigned int i = 0; i < n; ++i) {

            // get idx
            const unsigned int idx = pair_of_id_ths[i].second;

            dataset[idx].filter_flag = 1;

            // kNN search
            if (mode == 0) {
                for (unsigned int j = 0; j < dataset.size(); ++j) {

                    if (j != idx) {

                        const float distance = compute_distance(dataset[idx], dataset[j]);
                        dataset[idx].update_knn(distance, j);
                    }
                }
            }
            else {
                vpt.kNN_search(dataset[idx], idx);
            }

            // get the first top-n entries
            #pragma omp critical
            {
                update_top(dataset[idx].knn_distance, dataset[idx].identifier);
            }
        }
    }

    end = std::chrono::system_clock::now();
	time_threshold = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " threshold: " << threshold << "\n";
    std::cout << " get threshold time: " << time_threshold / 1000 << "[sec]\n\n";
}

// filtering phase
void filtering_phase() {

    // get threshold
    get_threshold();

    start = std::chrono::system_clock::now();

    // filtering
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(dynamic) 
        for (unsigned int i = 0; i < dataset.size(); ++i) {
            dataset[i].filtering(threshold);
        }
    }

    end = std::chrono::system_clock::now();
	time_filter = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " filtering time: " << time_filter / 1000 << "[sec]\n\n";
}

// compute the exact kNN by a linear scan
void scan(const std::vector<std::pair<float, unsigned int>> &candidate) {

    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(dynamic) reduction(+:count_verify)
        for (unsigned int i = 0; i < candidate.size(); ++i) {

            if (candidate[i].first > threshold) {

                bool flag = 1;

                count_verify = count_verify + 1;

                // get idx
                const unsigned int idx = candidate[i].second;

                if (mode == 0) {
                    for (unsigned int j = 0; j < dataset.size(); ++j) {

                        if (idx != j) {

                            // distance computation
                            const float distance = compute_distance(dataset[idx], dataset[j]);

                            dataset[idx].update_knn(distance, j);

                            if (dataset[idx].knn_distance <= threshold){
                                flag =0;
                                break;
                            }
                        }
                    }
                }
                else {
                    vpt.kNN_search(dataset[idx], idx);
                }

                if (flag) {

                    #pragma omp critical
                    {
                        // top-n update
                        if (flag) update_top(dataset[idx].knn_distance, dataset[idx].identifier);
                    }
                }
            }
        }
    }
}

// verification phase
void verification_phase() {

    start = std::chrono::system_clock::now();

    // extract non-filtered objects
    std::vector<std::pair<float, unsigned int>> candidate;
    for (unsigned int i = 0; i < dataset.size(); ++i) {

        if (dataset[i].filter_flag == 0) {
            candidate.push_back({dataset[i].knn_distance, i});
            dataset[i].init_knn();
        }
    }

    // sort by AkNN (descending order)
    std::sort(candidate.begin(), candidate.end(), std::greater<std::pair<float, unsigned int>>());

    // verify the result
    scan(candidate);

    end = std::chrono::system_clock::now();
	time_verify = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " count: " << count_verify << "\n\n";
    std::cout << " verification time: " << time_verify / 1000 << "[sec]\n\n";
}

// online processing
void online_processing() {

    // filtering phase
    filtering_phase();

    // verification phase
    verification_phase();
}