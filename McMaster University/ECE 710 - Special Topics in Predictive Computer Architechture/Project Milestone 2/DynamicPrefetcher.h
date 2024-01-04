// DynamicPrefetcher.h
#ifndef DYNAMICPREFETCHER_H
#define DYNAMICPREFETCHER_H

#include "BaseFetcher.h"

class DynamicPrefetcher : public BaseFetcher {
private:
    // Attributes specific to adaptive fetching
    int dynamicHistoryBufferSize;

public:
    // Constructor to initialize history buffer size
    DynamicPrefetcher(int size = 0) : dynamicHistoryBufferSize(size){}

    void Prefetching(int address, int PrefetchSize)  { 
        
        lastFetchedAddress = address;
        dynamicBuffer.push_back(lastFetchedAddress);

        // Keep only the last 8 elements
        while (dynamicBuffer.size() > dynamicHistoryBufferSize) {
            dynamicBuffer.erase(dynamicBuffer.begin());
        }
        
        std::cout << "Dynamic Buffer: ";
        for (const int& value : dynamicBuffer) {
        std::cout << value << " | ";
        }

        std::cout << std::endl;
        

        // Clear the predictedAddresses vector before populating it again
        predictedAddresses.clear();
        
        for (int j = 0; j < PrefetchSize; j++) {
            int predictedAddress = ComputeNextAddress();
            predictedAddresses.push_back(predictedAddress);
            lastFetchedAddress = predictedAddress;
        }

        // Output the predicted addresses
        std::cout << "Dynamic Predicted Addresses: ";
        for (int predictedAddress : predictedAddresses) {
            std::cout << "0x" << std::hex << predictedAddress << " ";
        }
        std::cout << std::dec << std::endl;
        
    }

    int ComputeNextAddress() override {
        int predictedAddress = lastFetchedAddress + PredictOffset();
        return predictedAddress;

    }

    int PredictOffset() const override {
        int dynamicStride = 0;

        if (dynamicBuffer.size() > 1) {
            int sumOfDifferences = 0;
            for (size_t i = 1; i < dynamicBuffer.size(); ++i) {
                sumOfDifferences += dynamicBuffer[i] - dynamicBuffer[i - 1];
            }
            // Calculate the average
            dynamicStride = sumOfDifferences / (dynamicBuffer.size() - 1);
        }
        return dynamicStride;
    }


    // Implement other virtual functions declared in BaseFetcher
    std::string GetPrefetcherType() const override {
        return "Dynamic"; // Replace with an appropriate description
    }


    ~DynamicPrefetcher() override = default;
};

#endif // DYNAMICPREFETCHER_H
