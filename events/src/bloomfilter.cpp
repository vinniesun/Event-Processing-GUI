//g++ -std=c++11 -w -I include -o bloom src/bloomfilter.cpp
#include "bloomfilter.hpp"

bloomFilter::bloomFilter(int no_of_elements, uint8_t no_hash) : filter(no_of_elements, std::vector<uint8_t>(2, 0)) {
    size = no_of_elements;
    no_of_hash = no_hash;
}

bloomFilter::~bloomFilter() {

}

void bloomFilter::insert(int value) {
    for (int i = 0; i < no_of_hash; i++) {
        int hash_val = h3_hash(value, hashes[i]);
        filter[hash_val][0] = 1;
        filter[hash_val][1] = 255;
    }
}

uint8_t bloomFilter::query(int value, bool dec, int ktos) {
    int count = 0;
    int result = 0;
    int ttos = 2*(2*ktos + 1);

    for (int i = 0; i < no_of_hash; i++) {
        int hash_val = h3_hash(value, hashes[i]);
        if (filter[hash_val][0] == 0) return 0;
        else count++;

        if (count == no_of_hash && dec) {
            for (int h = 0; h < no_of_hash; h++) {
                int hash_val_dec = h3_hash(value, hashes[h]);
                if (filter[hash_val_dec][1] < 255-ttos) {
                    filter[hash_val_dec][0] = 0;
                    filter[hash_val_dec][1] = 0;
                } else {
                    filter[hash_val_dec][1]--;
                }
            }
            result = filter[hash_val][1];
        } else if (count == no_of_hash && !dec) {
            result = filter[hash_val][1];
        }
    }

    return result;
}

void bloomFilter::reset() {
    for (auto &row: filter) {
        std::fill(row.begin(), row.end(), 0);
    }
}

int bloomFilter::h3_hash(int value, std::vector<int> hash_param) {
    int bit_width = hash_param.size();
    std::string bin = "";

    while(value) {
        bin += (value%2) + '0';
        value /= 2;
    }
    std::reverse(bin.begin(), bin.end());

    if (bin.size() < bit_width) {
        while (bin.size() < bit_width) {
            bin = "0" + bin;
        }
    } else if (bin.size() > bit_width) {
        while (bin.size() > bit_width) {
            bin = bin.substr(1, bin.size());
        }
    }

    int h3_val = 0;
    
    for (int i = 0; i < bit_width; i++) {
        if (bin[i] == '1') h3_val ^= hash_param[i];
    }

    return h3_val;
}

void bloomFilter::generateH3Hash(int hash_no, std::vector<std::string> filenames) {
    for (int i = 0; i < hash_no; i++) {
        hashes.push_back(read_hash_param(filenames[i]));
    }
}

std::vector<int> bloomFilter::read_hash_param(std::string filename) {
    std::fstream file;
    std::vector<int> output;

    file.open(filename, std::ios::in);
    if (file.is_open()) {
        std::string temp;
        while (std::getline(file, temp)) {
            std::stringstream ss(temp);
            std::string s;
            while (ss.good()) {
                std::getline(ss, s, ' ');
                if (s == "\n" || s == "") continue;
                output.push_back(std::stoi(s));
            }
        }
    }
    file.close();

    return output;
}

void bloomFilter::print() {
    for (int row = 0; row < filter.size(); row++) {
        for (int col = 0; col < filter[0].size(); col++) {
            std::cout << (int)filter[row][col] << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::string> load_hash_files(int width, int sensor_size, int hash_number) {
    std::vector<std::string> param_file_list;
    std::string path = "/Users/vincent/Desktop/CityUHK/Event_Process/Hashing/Code/Python/hash_keys/";

    for (int i = 0; i < hash_number; i++) {
        std::string filename = path + "param_matrix_" + std::to_string(i+1) + "_" + std::to_string(width) + "_" + std::to_string(sensor_size) + ".txt";
        param_file_list.push_back(filename);
    }

    return param_file_list;
}

bloomFilterMurmur3::bloomFilterMurmur3(int no_of_elements, uint8_t no_hash) : filter(no_of_elements, std::vector<uint8_t>(2, 0)) {
    size = no_of_elements;
    no_of_hash = no_hash;
}

bloomFilterMurmur3::~bloomFilterMurmur3() {

}

void bloomFilterMurmur3::insert(int value) {
    for (int i = 0; i < no_of_hash; i++) {
        int hash_val = hash(value, hash_seeds[i]) % size;

        filter[hash_val][0] = 1;
        filter[hash_val][1] = 255;
    }
}

uint8_t bloomFilterMurmur3::query(int value, bool dec, int ktos) {
    int count = 0;
    int result = 0;
    int ttos = 2*(2*ktos + 1);

    for (int i = 0; i < no_of_hash; i++) {
        int hash_val = hash(value, hash_seeds[i]) % size;
        if (filter[hash_val][0] == 0) return 0;
        else count++;

        if (count == no_of_hash && dec) {
            for (int h = 0; h < no_of_hash; h++) {
                int hash_val_dec = hash(value, hash_seeds[h]) % size;
                if (filter[hash_val_dec][1] < 255-ttos) {
                    filter[hash_val_dec][0] = 0;
                    filter[hash_val_dec][1] = 0;
                } else {
                    filter[hash_val_dec][1]--;
                }
            }
            result = filter[hash_val][1];
        } else if (count == no_of_hash && !dec) {
            result = filter[hash_val][1];
        }
    }

    return result;
}

void bloomFilterMurmur3::reset() {
    for (auto &row: filter) {
        std::fill(row.begin(), row.end(), 0);
    }
}

inline uint32_t bloomFilterMurmur3::rotl32(uint32_t x, int8_t r) {
    return (x << r) | (x >> (32 - r));
}

// Finalisation Mix. This forces all bits of a hash block to avalanche.
// The avalanche effect is defined as:
// For a given transformation to exhibit the avalanche, effect, an average
// of one half of the output bits should change whenever a single input bit is complemented.
// i.e. Each bit should have 50% chances to change if you change 1 bit of the input.
inline uint32_t bloomFilterMurmur3::fmix32(uint32_t h) {
    h ^= (h >> 16);
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);

    return h;
}

inline uint32_t bloomFilterMurmur3::mix32(uint32_t k, uint32_t hash) {
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;

    k *= c1;
    k = rotl32(k, 15);
    k *= c2;
    hash ^= k;
    hash = rotl32(hash, 13);
    hash = hash*5 + 0xe6546b64;

    return hash;
}

uint32_t bloomFilterMurmur3::hash(int value, uint32_t hash_seed) {
    // Implementation of the Murmur3 Hash
    uint32_t hash = hash_seed;
    long data = (long)value;

    // Reverse Bytes in data
    int length = sizeof(long);
    long r0 = 0;
    for (int i = 0; i < length; i++) {
        uint8_t temp = (data >> (64 - ((i+1)*8))) & 0xFF; // Bit-Shift Left to get the current Most Significant Byte.
        r0 = (temp << (i*8)) | r0;
    }
    
    hash = mix32((uint32_t) r0, hash);
    hash = mix32((uint32_t) (r0 >> 32), hash);

    hash ^= length;
    hash = fmix32(hash);

    return hash;
}

void bloomFilterMurmur3::print() {
    for (int row = 0; row < filter.size(); row++) {
        for (int col = 0; col < filter[0].size(); col++) {
            std::cout << (int)filter[row][col] << " ";
        }
        std::cout << std::endl;
    }
}

bloomFilterQuantised::bloomFilterQuantised(int no_of_elements, uint8_t no_hash, uint8_t ttos_threshold) : filter(no_of_elements, std::vector<uint8_t>(2, 0)) {
    size = no_of_elements;
    no_of_hash = no_hash;
    ttos = ttos_threshold;
}

bloomFilterQuantised::~bloomFilterQuantised() {

}

void bloomFilterQuantised::insert(int value) {
    for (int i = 0; i < no_of_hash; i++) {
        int hash_val = h3_hash(value, hashes[i]);
        filter[hash_val][0] = 1;
        filter[hash_val][1] = ttos;
    }
}

uint8_t bloomFilterQuantised::query(int value, bool dec) {
    int count = 0;
    int result = 0;

    for (int i = 0; i < no_of_hash; i++) {
        int hash_val = h3_hash(value, hashes[i]);
        if (filter[hash_val][0] == 0) return 0;
        else count++;

        if (count == no_of_hash && dec) {
            for (int h = 0; h < no_of_hash; h++) {
                int hash_val_dec = h3_hash(value, hashes[h]);
                if (filter[hash_val_dec][1] <= 0) {
                    filter[hash_val_dec][0] = 0;
                    filter[hash_val_dec][1] = 0;
                } else {
                    filter[hash_val_dec][1]--;
                }
            }
            result = filter[hash_val][1];
        } else if (count == no_of_hash && !dec) {
            result = filter[hash_val][1];
        }
    }

    return result;
}

void bloomFilterQuantised::reset() {
    for (auto &row: filter) {
        std::fill(row.begin(), row.end(), 0);
    }
}

int bloomFilterQuantised::h3_hash(int value, std::vector<int> hash_param) {
    int bit_width = hash_param.size();
    std::string bin = "";

    while(value) {
        bin += (value%2) + '0';
        value /= 2;
    }
    std::reverse(bin.begin(), bin.end());

    if (bin.size() < bit_width) {
        while (bin.size() < bit_width) {
            bin = "0" + bin;
        }
    } else if (bin.size() > bit_width) {
        while (bin.size() > bit_width) {
            bin = bin.substr(1, bin.size());
        }
    }

    int h3_val = 0;
    
    for (int i = 0; i < bit_width; i++) {
        if (bin[i] == '1') h3_val ^= hash_param[i];
    }

    return h3_val;
}

void bloomFilterQuantised::generateH3Hash(int hash_no, std::vector<std::string> filenames) {
    for (int i = 0; i < hash_no; i++) {
        hashes.push_back(read_hash_param(filenames[i]));
    }
}

std::vector<int> bloomFilterQuantised::read_hash_param(std::string filename) {
    std::fstream file;
    std::vector<int> output;

    file.open(filename, std::ios::in);
    if (file.is_open()) {
        std::string temp;
        while (std::getline(file, temp)) {
            std::stringstream ss(temp);
            std::string s;
            while (ss.good()) {
                std::getline(ss, s, ' ');
                if (s == "\n" || s == "") continue;
                output.push_back(std::stoi(s));
            }
        }
    }
    file.close();

    return output;
}

void bloomFilterQuantised::print() {
    for (int row = 0; row < filter.size(); row++) {
        for (int col = 0; col < filter[0].size(); col++) {
            std::cout << (int)filter[row][col] << " ";
        }
        std::cout << std::endl;
    }
}

// int main() {
//     bloomFilterAlternative test(20, 4);
//     //std::vector<std::string> param_file_list = load_hash_files(8, 18, test.no_of_hash);

//     //test.generateH3Hash(4, param_file_list);

//     test.insert(3);

//     //std::cout << (int)test.query(3, false, 3) << std::endl;
//     std::cout << "Current Bloom Filter Content:" << std::endl;
//     test.print();

//     test.reset();
//     std::cout << "After Resetting, the content of Bloom Filter is:" << std::endl;
//     test.print();

//     // std::vector<bloomFilter> tests;
    
//     // for (int i = 0; i < 10; i++) {
//     //     tests.push_back(bloomFilter(8, 4));
//     // }

//     // std::cout << tests.size() << std::endl;

//     return 0;
// }