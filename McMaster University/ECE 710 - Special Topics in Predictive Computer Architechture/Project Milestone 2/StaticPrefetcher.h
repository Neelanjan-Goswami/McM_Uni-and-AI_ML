// StaticPrefetcher.h
#ifndef STATICPREFETCHER_H
#define STATICPREFETCHER_H

#include "BaseFetcher.h"

class StaticPrefetcher : public BaseFetcher {
private:
    // Attributes specific to predictive fetching
    int prefetchDistance;

public:
    // Constructor with a default value for prefetch distance
    StaticPrefetcher(int distance = 0) : prefetchDistance(distance) {}

    void Prefetching(int address, int PrefetchSize) override {

        lastFetchedAddress = address;

        // Clear the predictedAddresses vector before populating it again
        predictedAddresses.clear();
        
        for (int j = 0; j < PrefetchSize; j++) {
            int predictedAddress = ComputeNextAddress();
            predictedAddresses.push_back(predictedAddress);
            lastFetchedAddress = predictedAddress;
        }

        // Output the predicted addresses
        std::cout << "Static Predicted Addresses: ";
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
        return prefetchDistance;
    }

    std::string GetPrefetcherType() const override {
        return "Static"; // Replace with an appropriate description
    }


    ~StaticPrefetcher() override = default;
};

#endif // STATICPREFETCHER_H
