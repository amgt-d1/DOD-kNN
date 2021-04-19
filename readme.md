## Introduction
* This repository provides codes for a distance-based outlier detection algorithm on a strongly-connected approximate kNN graph.
   * Outliers are defined as objects whose distance to their k-th NN is top-N among a set of object.
* Our algorithm supports metric space.
    * Our codes implement L2 (Euclidean distance), L1 (Manhattan distance), Edit distance, angular distance, and L4 distance by default.
    * The other distance functions are free to add.

## Requirement
* Linux OS (Ubuntu).
   * The others have not been tested.
* `g++ 7.4.0` (or higher version) and `Openmp`.

## How to use
* Before running our DOD algorithm, build a strongly-connected approximete kNN graph.
  * For example, this can be obtained from https://github.com/amgt-d1/DOD, and uncomment `connect_graph()`, `remove_detour()`, and `remove_edges()` in `mrpg.hpp`.
  * Update input_graph() in `data.hpp`.
* Parameter configuration can be done via txt files in `parameter` directory.
* Data files have to be at `dataset` directory.
   * You can implement data input in as you like manner at input_data() function in `data.hpp`.
   * Now dataset directory contains a dummy file only.
* Compile: `g++ -O3 -o progrand.out main.cpp -std=c++11 -fopenmp -Wall`.
* Run: `./progrand.out`.


## Citation
If you use our implementation, please cite the following paper.
``` 

``` 

## License
Copyright (c) 2021 Daichi Amagata  
This software is released under the [MIT license](https://github.com/amgt-d1/DOD-kNN/blob/main/license.txt).
