## Introduction
* This repository provides codes for bulding a strongly-connected approximete kNN graph and a distance-based outlier detection algorithm on it.
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
* Parameter configuration can be done via txt files in `parameter` directory.
* Data files have to be at `dataset` directory.
   * You can implement data input in as you like manner at input_data() function in data.hpp.
   * Now dataset directory contains a dummy file only.
