/*

Neelanjan Goswami
400414867
Dec 2023
Project: Data Prefetchers

*/
#include "FetcherController.h"
#include <vector>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <chrono>

int main() {
    // Memory access array
    std::vector<int> memoryAccesses;
    std::ifstream traceFile("trace.txt");

    if (traceFile.is_open()) {
        std::string hexValue;
        while (traceFile >> hexValue) {
            int intValue;
            std::stringstream ss(hexValue);
            ss >> std::hex >> intValue;
            memoryAccesses.push_back(intValue);
        }

        traceFile.close();

        std::cout << "Memory accesses are: " << std::endl;
        for (int val : memoryAccesses) {
            std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << val << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Unable to open trace file." << std::endl;
    }
    std::cout << "" << std::endl;

    // Cache characteristics
    int associativity = 4;
    int cacheSizeKB = 64;
    int maxCacheSizeKB = 256;

    std::cout << "Cache Properties: " << std::endl;
    std::cout << "Cache Size: " << std::dec << cacheSizeKB << std::hex << "KB" << std::endl;
    std::cout << "Associativity: " << associativity << "\n" << std::endl;

    //Aggressivenes of a Prefetcher
    int PrefetchSize = cacheSizeKB / 32 ;
    int maxPrefetchSize = maxCacheSizeKB / 32 ;
    std::cout << "Initial Number of values it can Prefetch: " << PrefetchSize << std::endl;
    std::cout << "Max Prefetch Cache Size Allotted: " << std::dec << maxCacheSizeKB << std::hex << "KB" << " , Hence, it can Prefetch: " << maxPrefetchSize << "\n" << std::endl;

    // Stride
    int stride = 16;

    // Convert stride to hexadecimal format
    std::cout << "Stride value for Static Prefetcher: 0x" << std::hex << stride << std::dec;
    std::cout << "\n" << std::endl;

    // Dynamic history buffer size
    int dynamicHistoryBufferSize = 8;
    std::cout << "Dynamic Prefetcher History Buffer Size: " << dynamicHistoryBufferSize << "\n" << std::endl;

    // Create FetcherController with StaticPrefetcher and DynamicPrefetcher
    FetcherController controller;

    // Create instances of your fetchers
    StaticPrefetcher staticPrefetcher(stride);
    DynamicPrefetcher dynamicPrefetcher(dynamicHistoryBufferSize);

    // Add fetchers to the controller
    controller.AddPrefetcher(&staticPrefetcher);
    controller.AddPrefetcher(&dynamicPrefetcher);


    // Hit and miss tracking for each fetcher
    std::unordered_set<int> staticHits;
    std::unordered_set<int> dynamicHits;
    std::unordered_set<int> staticMisses;
    std::unordered_set<int> dynamicMisses;

    std::vector<int> durations_static;
    std::vector<int> durations_dynamic;    

    // Demonstrate the fetchers in action
    for (int i = 0; i < memoryAccesses.size()-1; ++i) {
        std::cout << "Accessing address: 0x" << std::hex << memoryAccesses[i] << ", \n";

        auto start = std::chrono::high_resolution_clock::now();

        // Operate StaticPrefetcher first
        std::cout << "Operating Static Prefetcher:" << std::endl;
        controller.OperateFetcher(1); 
        BaseFetcher* staticPrefetcher = controller.getCurrentFetcher();

        if (staticPrefetcher) {
            // Call the StaticPrefetching method
            staticPrefetcher->Prefetching(memoryAccesses[i], PrefetchSize);
        }

        // Access predictedAddresses using the getter
        const std::vector<int>& predictedAddresses = staticPrefetcher->GetPredictedAddresses();

        // Check if the value is present in predictedAddresses
        int valueToCheck = memoryAccesses[i+1];

        // Convert valueToCheck to hexadecimal format
        std::cout << "Checking for value: 0x" << std::hex << valueToCheck << std::dec << std::endl;

        auto it = std::find(predictedAddresses.begin(), predictedAddresses.end(), valueToCheck);

        if (it != predictedAddresses.end()) {
            std::cout << "Value 0x" << std::hex << valueToCheck << " is in Static Prefetched Addresses." << std::dec << std::endl;
            staticHits.insert(memoryAccesses[i]);
            //Minimum prefetch size is 2
            if(PrefetchSize>2){
                std::cout << std::endl;
                std::cout << "Updating the Prefetch Size, i.e. the Aggressiveness of the Prefetcher " << std::endl;
                std::cout << "Old Prefetch size: " << PrefetchSize << std::endl;
                PrefetchSize-=1;
                std::cout << "New Prefetch size: " << PrefetchSize << std::endl;
            }
        } else {
            std::cout << "Value 0x" << std::hex << valueToCheck << " is not in Static Prefetched Addresses." << std::dec << std::endl;
            staticMisses.insert(memoryAccesses[i]);
            if(PrefetchSize<maxPrefetchSize){
                std::cout << std::endl;
                std::cout << "Updating the Prefetch Size, i.e. the Aggressiveness of the Prefetcher " << std::endl;
                std::cout << "Old Prefetch size: " << PrefetchSize << std::endl;
                PrefetchSize+=1;
                std::cout << "New Prefetch size: " << PrefetchSize << std::endl;
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        std::cout << "Time taken by Static Prefetcher: "<< duration.count() << " microseconds" << std::endl;
        durations_static.push_back(duration.count());


        auto start2 = std::chrono::high_resolution_clock::now();
        
        // Operate DynamicPrefetcher 
        std::cout << " " << std::endl;
        std::cout << "Accessing address: 0x" << std::hex << memoryAccesses[i] << ", \n";
        std::cout << "Operating Dynamic Fetcher:" << std::endl;
        controller.OperateFetcher(0); 
        BaseFetcher* dynamicPrefetcher = controller.getCurrentFetcher();

        if (dynamicPrefetcher) {
            // Call the DynamicPrefetching method
            dynamicPrefetcher->Prefetching(memoryAccesses[i], PrefetchSize);
        }

        // Access predictedAddresses using the getter
        const std::vector<int>& predictedAddresses2 = dynamicPrefetcher->GetPredictedAddresses();

        // Convert valueToCheck to hexadecimal format
        std::cout << "Checking for value: 0x" << std::hex << valueToCheck << std::dec << std::endl;

        auto it2 = std::find(predictedAddresses2.begin(), predictedAddresses2.end(), valueToCheck);

        if (it2 != predictedAddresses2.end()) {
            std::cout << "Value 0x" << std::hex << valueToCheck << " is in Dynamic Prefetched Addresses." << std::dec << std::endl;
            dynamicHits.insert(memoryAccesses[i]);
            //Minimum prefetch size is 2
            if(PrefetchSize>2){
                std::cout << std::endl;
                std::cout << "Updating the Prefetch Size, i.e. the Aggressiveness of the Prefetcher " << std::endl;
                std::cout << "Old Prefetch size: " << PrefetchSize << std::endl;
                PrefetchSize-=1;
                std::cout << "New Prefetch size: " << PrefetchSize << std::endl;
            }
        } else {
            std::cout << "Value 0x" << std::hex << valueToCheck << " is not in Dynamic Prefetched Addresses." << std::dec << std::endl;
            dynamicMisses.insert(memoryAccesses[i]);
            if(PrefetchSize<maxPrefetchSize){
                std::cout << std::endl;
                std::cout << "Updating the Prefetch Size, i.e. the Aggressiveness of the Prefetcher " << std::endl;
                std::cout << "Old Prefetch size: " << PrefetchSize << std::endl;
                PrefetchSize+=1;
                std::cout << "New Prefetch size: " << PrefetchSize << std::endl;
            }
        }

        auto stop2 = std::chrono::high_resolution_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);

        std::cout << "Time taken by Dynamic Prefetcher: "<< duration2.count() << " microseconds" << std::endl;
        durations_dynamic.push_back(duration2.count());

        std::cout << "-----------------------" << std::endl;
        std::cout << "\n" << std::endl;
    }

    int total_static = std::accumulate(durations_static.begin(), durations_static.end(), 0);
    double average_static = static_cast<double>(total_static) / durations_static.size();

    std::cout << "Total time taken by Static Prefetcher: " << total_static << " microseconds" << std::endl;
    std::cout << "Average time taken by Static Prefetcher: " << average_static << " microseconds" << std::endl;

    int total_dynamic = std::accumulate(durations_dynamic.begin(), durations_dynamic.end(), 0);
    double average_dynamic = static_cast<double>(total_dynamic) / durations_dynamic.size();

    std::cout << "Total time taken by Dynamic Prefetcher: " << total_dynamic << " microseconds" << std::endl;
    std::cout << "Average time taken by Dynamic Prefetcher: " << average_dynamic << " microseconds" << std::endl;

    std::cout << "\n" << std::endl;

    // Calculate hit rate and miss rate
    double staticHitRate = static_cast<double>(staticHits.size()) / (memoryAccesses.size()-1);
    double adaptiveHitRate = static_cast<double>(dynamicHits.size()) / (memoryAccesses.size()-1);
    double staticMissRate = static_cast<double>(staticMisses.size()) / (memoryAccesses.size()-1);
    double adaptiveMissRate = static_cast<double>(dynamicMisses.size()) / (memoryAccesses.size()-1);
    
    std::cout << "Static Prefetcher Hit Rate: " << staticHitRate * 100 << "%" << std::endl;
    std::cout << "Static Prefetcher Miss Rate: " << staticMissRate * 100 << "%" << std::endl;
    std::cout << "Adaptive Fetcher Hit Rate: " << adaptiveHitRate * 100 << "%" << std::endl;
    std::cout << "Adaptive Fetcher Miss Rate: " << adaptiveMissRate * 100 << "%" << std::endl;

    return 0;
}
