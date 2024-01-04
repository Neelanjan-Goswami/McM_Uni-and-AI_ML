/*
Neelanjan Goswami
400414867
Assignment 1
*/

#include <iostream>
#include <vector>
#include <climits>

const int associativity = 4;

const int CACHE_SIZE = 256;
const int BLOCK_SIZE = 16;
const int NUM_SETS = CACHE_SIZE / (4 * BLOCK_SIZE);
int cacheMemory[NUM_SETS][associativity];
int lruBits[NUM_SETS][associativity];
int hitCount = 0;
int missCount = 0;
void simulateCacheAccess(int memoryAddress) {
    int setIndex = (memoryAddress / BLOCK_SIZE) % NUM_SETS;
    int tag = memoryAddress / (NUM_SETS * BLOCK_SIZE);

    int way = -1;
    for (int i = 0; i < associativity; i++) {
        if (cacheMemory[setIndex][i] == tag) {
            way = i;
            break;
        }
    }

    if (way != -1) {
        // Cache hit
        hitCount++;
    } else {
        // Cache miss
        missCount++;
        int minLru = INT_MAX;
        for (int i = 0; i < associativity; i++) {
            if (lruBits[setIndex][i] < minLru) {
                minLru = lruBits[setIndex][i];
                way = i;
            }
        }
        cacheMemory[setIndex][way] = tag;
    }

    // Update LRU bits
    for (int i = 0; i < associativity; i++) {
        if (i != way) {
            lruBits[setIndex][i]++;
        }
    }
}
void initializeCacheMemory() {
    for (int i = 0; i < NUM_SETS; i++) {
        for (int j = 0; j < associativity; j++) {
            cacheMemory[i][j] = -1;
            lruBits[i][j] = j;
        }
    }
}
int main() {
    const int NUM_DATA_ELEMENTS = 128;
    const int STRIDE = 1;
    const int NUM_ACCESS_ITERATIONS = 100;

    initializeCacheMemory();

    for (int iteration = 0; iteration < NUM_ACCESS_ITERATIONS; iteration++) {
        for (int dataIndex = 0; dataIndex < NUM_DATA_ELEMENTS; dataIndex += STRIDE) {
            simulateCacheAccess(dataIndex * sizeof(int));
        }
    }
    std::cout << "Total count: " << hitCount+missCount << std::endl;
    std::cout << "Total Hits: " << hitCount << std::endl;
    std::cout << "Total Misses: " << missCount << std::endl;
    std::cout << "Hit Rate: " << ((double)hitCount / (hitCount + missCount)) * 100 << "%" << std::endl;
    std::cout << "Miss Rate: " << ((double)missCount / (hitCount + missCount)) * 100 << "%" << std::endl;

    return 0;
}