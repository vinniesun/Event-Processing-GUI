// Don't need to use pragma once if include guard is used, as they offer the same functionality
// #pragma once

#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <bitset> // This is used to convert integert to a string of binary value
#include <algorithm>
#include <cmath>

class bloomFilter {
public:
    bloomFilter(int no_of_elements, uint8_t no_hash);
    ~bloomFilter();

    void insert(int value);
    uint8_t query(int value, bool dec, int ktos);
    void reset();
    int h3_hash(int value, std::vector<int> hash_param);
    void generateH3Hash(int hash_no, std::vector<std::string> filenames);
    std::vector<int> read_hash_param(std::string filename);
    void print();

    std::vector<std::vector<uint8_t>> filter;   // This is the Bloom Filter used to store our data
    uint8_t no_of_hash;                         // This value represents the number of hashes used for the Bloom Filter

private:
    int size;                                   // This is the width of our Bloom Filter
    std::vector<std::vector<int>> hashes;       // This contains all of the hashes used in the Bloom Filter 
};

class bloomFilterMurmur3 {
public:
    bloomFilterMurmur3(int no_of_elements, uint8_t no_hash);
    ~bloomFilterMurmur3();

    void insert(int value);
    uint8_t query(int value, bool dec, int ktos);
    void reset();
    uint32_t hash(int value, uint32_t hash_seed);
    void print();
    inline uint32_t rotl32(uint32_t x, int8_t r);
    inline uint32_t mix32(uint32_t k, uint32_t hash);
    inline uint32_t fmix32(uint32_t h);

    std::vector<std::vector<uint8_t>> filter;   // This is the Bloom Filter used to store our data
    uint8_t no_of_hash;                         // This value represents the number of hashes used for the Bloom Filter

private:
    int size;                                   // This is the width of our Bloom Filter
    std::vector<uint32_t> hash_seeds = {10273,21371,2343,84238};       // This contains all of the hash seeds used by murmur3
};

class bloomFilterQuantised {
public:
    bloomFilterQuantised(int no_of_elements, uint8_t no_hash, uint8_t ttos_threshold);
    ~bloomFilterQuantised();

    void insert(int value);
    uint8_t query(int value, bool dec);
    void reset();
    int h3_hash(int value, std::vector<int> hash_param);
    void generateH3Hash(int hash_no, std::vector<std::string> filenames);
    std::vector<int> read_hash_param(std::string filename);
    void print();

    std::vector<std::vector<uint8_t>> filter;   // This is the Bloom Filter used to store our data
    uint8_t no_of_hash;                         // This value represents the number of hashes used for the Bloom Filter
    uint8_t ttos;

private:
    int size;                                   // This is the width of our Bloom Filter
    std::vector<std::vector<int>> hashes;       // This contains all of the hashes used in the Bloom Filter 
};

std::vector<std::string> load_hash_files(int width, int sensor_size, int hash_number);

#endif