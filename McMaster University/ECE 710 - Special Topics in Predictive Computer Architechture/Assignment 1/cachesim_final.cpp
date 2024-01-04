/*
Neelanjan Goswami
400414867
Assignment 1
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <climits>

class Cache {
private:
    // Cache configuration parameters
    int blocksize;
    int cachesize;
    int associativity;
    char writePolicy;

    // Derived cache parameters
    int numSets;
    int numBlocks;
    int tagSize;
    int indexSize;
    int offsetSize;

    // Data structures to represent the cache and track LRU
    std::vector<std::vector<int> > cache;
    std::vector<std::vector<int> > lru;

    // Counters for cache statistics
    int hits;
    int misses;
    int reads;
    int writes;

    // Output stream for writing results to "output.txt"
    std::ofstream output;

    // Private methods for handling memory access and printing access details
    void processAccess(char op, int address);
    void printAccess(char op, int address, int tag, int index, int offset, int way, int uway);

public:
    // Constructor to initialize the cache and open the output file
    Cache(int blocksize, int cachesize, int associativity, char writePolicy);

    // Method to load and process a memory access trace
    void loadTrace(const std::string &filename);

    // Method to print cache simulation results
    void printResults(const std::string &outputFilename);
};

// Constructor for the Cache class
Cache::Cache(int blocksize, int cachesize, int associativity, char writePolicy)
    : blocksize(blocksize), cachesize(cachesize), associativity(associativity), writePolicy(writePolicy) {
    // Calculate cache parameters
    numBlocks = cachesize / blocksize;
    numSets = numBlocks / associativity;
    tagSize = 24 - static_cast<int>(log2(blocksize)) - static_cast<int>(log2(numSets));
    indexSize = static_cast<int>(log2(numSets));
    offsetSize = static_cast<int>(log2(blocksize));

    // Initialize cache and LRU matrix
    cache.resize(numSets, std::vector<int>(associativity, -1));
    lru.resize(numSets, std::vector<int>(associativity, 0));
    hits = misses = reads = writes = 0;
    output.open("output.txt"); // Open "output.txt" for writing

    // Display cache configuration details
    int cachesizeKB = cachesize / 1024;
    std::cout << cachesizeKB << "KB " << associativity << "-way associative cache:" << std::endl;
    std::cout << "\tBlock size = " << blocksize << " bytes" << std::endl;
    std::cout << "\tNumber of [sets,blocks] = [" << numSets << "," << numBlocks << "]" << std::endl;
    std::cout << "\tExtra space for tag storage = " << (tagSize * numBlocks / 8) << " bytes ("
           << std::fixed << std::setprecision(2) << static_cast<double>(tagSize * numBlocks / 8) / cachesize * 100 << "%)" << std::endl;
    std::cout << "\tBits for [tag,index,offset] = [" << tagSize << ", " << indexSize << ", " << offsetSize << "] = 24" << std::endl;
    std::cout << "\tWrite policy = " << (writePolicy == 'b' ? "Write-through" : "Write-back") << std::endl;
    std::cout << "\nHex  Binary Address                      Set   Blk        Memory" << std::endl;
    std::cout << "Address (tag/index/offset)               Tag Index off  Way UWay Read Write" << std::endl;
    std::cout << "======= ========================== ======== ===== === ==== ==== ==== ====" << std::endl;

    output << cachesizeKB << "KB " << associativity << "-way associative cache:" << std::endl;
    output << "\tBlock size = " << blocksize << " bytes" << std::endl;
    output << "\tNumber of [sets,blocks] = [" << numSets << "," << numBlocks << "]" << std::endl;
    output << "\tExtra space for tag storage = " << (tagSize * numBlocks / 8) << " bytes ("
           << std::fixed << std::setprecision(2) << static_cast<double>(tagSize * numBlocks / 8) / cachesize * 100 << "%)" << std::endl;
    output << "\tBits for [tag,index,offset] = [" << tagSize << ", " << indexSize << ", " << offsetSize << "] = 24" << std::endl;
    output << "\tWrite policy = " << (writePolicy == 'b' ? "Write-through" : "Write-back") << std::endl;
    output << "\nHex  Binary Address                      Set   Blk        Memory" << std::endl;
    output << "Address (tag/index/offset)               Tag Index off  Way UWay Read Write" << std::endl;
    output << "======= ========================== ======== ===== === ==== ==== ==== ====" << std::endl;
}


// Method to process a memory access (read or write)
void Cache::processAccess(char op, int address) {
    int tag = address >> (indexSize + offsetSize);
    int index = (address >> offsetSize) & ((1 << indexSize) - 1);
    int offset = address & ((1 << offsetSize) - 1);
    int way = -1;
    int uway = -1;

    // Check if the tag is in the cache
    for (int i = 0; i < associativity; i++) {
        if (cache[index][i-1] == tag) {
            way = i;
            break;
        }
    }

    if (way != -1) { // Cache hit
        hits++;
        lru[index][way] = 0;
        uway = -2; // Only updated LRU bits

        // If it's a write operation, update the cache
        if (writePolicy == 'b' && op == 'W') {
            writes++;
            uway = way; // Updated the cache
        }
        if (op == 'R') reads++; // Increment read count for cache hit
    } else { // Cache miss
        misses++;
        int min_lru = INT_MAX;

        // Find the way with the minimum LRU value
        for (int i = 0; i < associativity; i++) {
            if (lru[index][i] < min_lru) {
                min_lru = lru[index][i];
                uway = i;
            }
        }

        // Update cache and LRU for the new entry
        cache[index][uway] = tag;
        lru[index][uway] = 0;

        if (writePolicy == 'b' && op == 'W') {
            writes++;
        }
        if (op == 'R') {
            reads++;
        }
    }

    // Update LRU for the other ways in the set
    for (int i = 0; i < associativity; i++) {
        if (i != uway) lru[index][i]++;
    }

    // Print the details of the memory access
    printAccess(op, address, tag, index, offset, way, uway);
}

// Method to print details of a memory access
void Cache::printAccess(char op, int address, int tag, int index, int offset, int way, int uway) {
    std::cout << std::hex << std::setw(6) << address << op << " ";
    std::cout << std::bitset<10>(tag) << " ";
    std::cout << std::bitset<8>(index) << " ";
    std::cout << std::bitset<5>(offset) << " ";
    std::cout << std::dec << std::setw(4) << tag << " ";
    std::cout << std::dec << std::setw(4) << index << " ";
    std::cout << std::dec << std::setw(4) << offset << " ";
    std::cout << std::setw(4) << way << " ";
    std::cout << std::setw(4) << uway << " ";
    std::cout << (op == 'R' ? "1" : "0") << " ";
    std::cout << (op == 'W' ? "1" : "0") << std::endl;

    // Also write to "output.txt"
    output << std::hex << std::setw(6) << address << op << " ";
    output << std::bitset<10>(tag) << " ";
    output << std::bitset<8>(index) << " ";
    output << std::bitset<5>(offset) << " ";
    output << std::dec << std::setw(4) << tag << " ";
    output << std::dec << std::setw(4) << index << " ";
    output << std::dec << std::setw(4) << offset << " ";
    output << std::setw(4) << way << " ";
    output << std::setw(4) << uway << " ";
    output << (op == 'R' ? "1" : "0") << " ";
    output << (op == 'W' ? "1" : "0") << std::endl;
}

// Method to load a memory access trace from a file
void Cache::loadTrace(const std::string &filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            char op = line[0];
            int address = std::stoi(line.substr(2), nullptr, 16);
            processAccess(op, address);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

// Method to print cache simulation results
void Cache::printResults(const std::string &outputFilename) {
    int totalAccesses = hits + misses + writes; // Total accesses include reads and writes
    int totalHits = hits + writes; // Total hits include reads and writes

    std::cout << "nref=" << totalAccesses << ", nread=" << reads << ", nwrite=" << writes << std::endl;
    std::cout << "hits = " << totalHits << ", hit rate = " << static_cast<double>(totalHits) / totalAccesses << std::endl;
    std::cout << "misses = " << (totalAccesses - totalHits) << ", miss rate = " << 1.0 - (static_cast<double>(totalHits) / totalAccesses) << std::endl;
    std::cout << "main memory reads=" << reads << ", main memory writes=" << writes << std::endl;

    // Write the results to "output.txt"
    output << "nref=" << totalAccesses << ", nread=" << reads << ", nwrite=" << writes << std::endl;
    output << "hits = " << totalHits << ", hit rate = " << static_cast<double>(totalHits) / totalAccesses << std::endl;
    output << "misses = " << (totalAccesses - totalHits) << ", miss rate = " << 1.0 - (static_cast<double>(totalHits) / totalAccesses) << std::endl;
    output << "main memory reads=" << reads << ", main memory writes=" << writes << std::endl;

    output.close(); // Close the "output.txt" file
}



int main(int argc, char *argv[]) {
    int blocksize, cachesize, associativity;
    char writePolicy;

    for (int i = 1; i < argc; i += 2) {
        if (argv[i][0] == '-' && i + 1 < argc) {
            switch (argv[i][1]) {
                case 'b':
                    blocksize = std::stoi(argv[i + 1]);
                    break;
                case 'c':
                    cachesize = std::stoi(argv[i + 1]);
                    break;
                case 'a':
                    associativity = std::stoi(argv[i + 1]);
                    break;
                case 'w':
                    writePolicy = argv[i + 1][0];
                    break;
                default:
                    std::cerr << "Invalid option: " << argv[i] << std::endl;
                    return 1;
            }
        } else {
            std::cerr << "Invalid arguments" << std::endl;
            return 1;
        }
    }

    // Create an instance of the Cache class
    Cache cache(blocksize, cachesize, associativity, writePolicy);

    // Define the input trace filename
    std::string filename = "memory_trace.txt"; // Make sure to replace this with your actual trace file
    std::string outputFilename = "output.txt";

    // Load the memory access trace, simulate the cache, and print results
    cache.loadTrace(filename);
    cache.printResults(outputFilename);

    return 0;
}
