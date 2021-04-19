#include <vector>
#include "file_input.hpp"
#include <unordered_map>
#include <cfloat>
#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>


// declaration of data
class data;

// definition of dataset
std::vector<data> dataset;

// compute distance
float compute_distance(const data &p, const data &q);

// definition of data class
class data {

public:

	// for numeric data
	std::vector<float> pt;

	// for string data
	std::vector<char> str;

	// identifier
	unsigned int identifier = 0;

	// kNN list
	std::multimap<float, unsigned int> knn_list;

	// kNN distance
	float knn_distance = FLT_MAX;

	// edge
	std::vector<std::pair<unsigned int, float>> edges;

	// degree-based threshold
	float degree_distance = FLT_MAX;

	// filter flag
	bool filter_flag = 0;


	/***************/
	/* constructor */
	/***************/

	// standard
	data() {}

	// with identifier
	data(const unsigned int id) { identifier = id; }

	/*******************/
	/* member function */
	/*******************/

	// point update
	void update_pt(std::vector<float>& point) { for (unsigned int i = 0; i < point.size(); ++i) pt.push_back(point[i]); }

	// word update
	void update_str(std::string s) { for (unsigned int i = 0; i < s.size(); ++i) str.push_back(s[i]); }

	// kNN update
	void update_knn(const float distance, const unsigned int id) {

		if (knn_list.size() < k) {

			knn_list.insert({distance, id});
		}
		else {

			if (distance < knn_distance) knn_list.insert({distance, id});
		}

		// erase k+1 idx
		if (knn_list.size() > k) {

			// erase k+1 idx
			auto it = knn_list.end();
			--it;
			knn_list.erase(it);
		}

		// update threshold
		if (knn_list.size() == k) {

			auto it = knn_list.end();
			--it;
			knn_distance = it->first;
		}
	}

	// init kNN
	void init_knn() {

		knn_list.clear();
		knn_distance = FLT_MAX;
	}

	// deg-based threshold update
	void update_degree_distance() {

		std::multiset<float> KNN;
		for (unsigned int i = 0; i < edges.size(); ++i) KNN.insert(edges[i].second);

		auto it = KNN.end();
		--it;
		
		// use only top-K (K = 25 by default)
		while (KNN.size() > 25) {
			KNN.erase(it);
			it = KNN.end();
			--it;
		}
		
		// set distance
		degree_distance = *it;
	}

	// filtering
	void filtering(const float threshold) {

		const unsigned int max_access = k * 3;

		// prepare visit nodes
		std::unordered_set<unsigned int> visit;

		// init by myself
		visit.insert(identifier);

		// group member
		//std::vector<unsigned int> member;
		//member.push_back(identifier);

		// init queue
		std::deque<unsigned int> queue;
		for (unsigned int i = 0; i < edges.size(); ++i) queue.push_back(edges[i].first);

		while (queue.size() > 0) {

			// get front
			const unsigned int id = queue[0];

			// pop front
			queue.pop_front();

			// un-visited node
			if (visit.find(id) == visit.end()) {

				// update visit
				visit.insert(id);

				// compute distance
				const float distance = compute_distance(dataset[identifier], dataset[id]);

				// update kNN
				update_knn(distance, id);

				// member update
				/*if (distance <= threshold / 2.0) {

					member.push_back(id);

					if (member.size() == k + 1) {

						for (unsigned int i = 0; i < member.size(); ++i) {
							dataset[member[i]].filter_flag = 1;
							dataset[member[i]].group_flag = 1;
						}
						break;	
					}
				}*/

				if (knn_distance <= threshold) {
					filter_flag = 1;
					break;
				}

				// if visited max nodes, break
				if (visit.size() >= max_access) break;

				// update queue
				for (unsigned int i = 0; i < dataset[id].edges.size(); ++i) {

					// get id
					const unsigned id_ = dataset[id].edges[i].first;

					queue.push_back(id_);
				}
			}
		}
	}
};


// input data
void input_data() {

	// id variable
	unsigned int id = 0;

	// point coordinates variable
	std::vector<float> point;
	point.resize(dimensionality);


	// sample probability
	std::mt19937 mt(0);
	std::uniform_real_distribution<> rnd(0, 1.0);

	if (dataset_id == 0) {

		// input deep10M
		std::string f_name = "sift_base.fvecs";

		// file input
		std::ifstream ifs_file(f_name, std::ios::in | std::ios::binary);

		// error check
		if (ifs_file.fail()) {
			std::cout << " SIFT does not exist." << std::endl;
			std::exit(0);
		}

		float t;
		unsigned int cnt = 0;
		std::vector<float> temp;

		while (!ifs_file.eof()) {

			// read
			ifs_file.read((char*)& t, sizeof(float));

			if (cnt > 0) {
				
				temp.push_back(t);
			
				if (cnt % 129 == 0) {

					temp.pop_back();

					//if (rnd(mt) <= sampling_rate) {

						// make a data object
						data d(id);
						d.update_pt(temp);

						// insert into dataset
						dataset.push_back(d);

						// increment identifier
						++id;
					//}

					temp.clear();
					cnt = 0;
				}
			}
			++cnt;
		}
	}
	

	std::cout << " Data input done\n";
}

// input graph
void input_graph() {

	// input graph (example)
	std::string f_name = "graph.txt";
	
	// file input
	std::ifstream ifs_file(f_name);

	// error check
	if (ifs_file.fail()) {
		std::cout << " graph file does not exist." << std::endl;
		std::exit(0);
	}

	unsigned int id = 0;
	unsigned int id_ = 0;

	// file read
	while (!ifs_file.eof()) {

		ifs_file >> id >> id_;

		// edge insertion
		dataset[id].edges.push_back({id_, 0});
	}

	std::cout << " Graph input done\n";

	#pragma omp parallel
    {
        #pragma omp for schedule(static) 
		for (unsigned int i = 0; i < dataset.size(); ++i) {

			for (unsigned int j = 0; j < dataset[i].edges.size(); ++j) {	
				const unsigned int id = dataset[i].edges[j].first;
				dataset[i].edges[j].second = compute_distance(dataset[i], dataset[id]);
			}			
		}
	}

	std::cout << " Distance comp. done\n\n";
}

// distance computation
float compute_distance(const data& d1, const data& d2) {

	float distance = 0;

	if (type == 0) {

		// L2 norm
		for (unsigned int i = 0; i < dimensionality; ++i) {
			const float temp = d1.pt[i] - d2.pt[i];
			distance += temp * temp;
		}
		distance = sqrt(distance);
	}
	else if (type == 1) {

		// L1 norm
		for (unsigned int i = 0; i < dimensionality; ++i) distance += fabsf(d1.pt[i] - d2.pt[i]);
	}
	else if (type == 2) {

		// edit distance
		const size_t len1 = d1.str.size();
		const size_t len2 = d2.str.size();
		std::vector<std::vector<size_t>> d(len1 + 1, std::vector<size_t>(len2 + 1));

		d[0][0] = 0;
		for (unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
		for (unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

		for (unsigned int i = 1; i <= len1; ++i) {
			for (unsigned int j = 1; j <= len2; ++j) d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (d1.str[i - 1] == d2.str[j - 1] ? 0 : 1) });
		}

		distance = (float)d[len1][len2];
	}
	else if (type == 3) {

		// angular distance

		// inner product
		float ip = 0;

		// L2 norm
		float l2norm_1 = 0;
		float l2norm_2 = 0;

		for (unsigned int i = 0; i < dimensionality; ++i) {

			ip += d1.pt[i] * d2.pt[i];

			l2norm_1 += powf(d1.pt[i], 2.0);
			l2norm_2 += powf(d2.pt[i], 2.0);
		}

		l2norm_1 = sqrt(l2norm_1);
		l2norm_2 = sqrt(l2norm_2);

		float cosine_sim = ip / (l2norm_1 * l2norm_2);

		distance = acos(cosine_sim) / 3.1416;
	}
	else if (type == 4) {

		// L4 norm
		for (unsigned int i = 0; i < dimensionality; ++i) {
			const float temp = d1.pt[i] - d2.pt[i];
			distance += temp * temp * temp * temp;
		}
		distance = powf(distance, 0.25);
	}

	return distance;
}
