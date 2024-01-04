// BaseFetcher.h
#ifndef BASEFETCHER_H
#define BASEFETCHER_H

#include <iostream>

class BaseFetcher {
protected:
    int lastFetchedAddress = 0;
    std::vector<int> predictedAddresses;
    std::vector<int> dynamicBuffer;

public:
    // Virtual function to get the type of the prefetcher
    virtual std::string GetPrefetcherType() const = 0;

    virtual void Prefetching(int address, int PrefetchSize) = 0;
    virtual int ComputeNextAddress() = 0;
    virtual int PredictOffset() const = 0;

    // Public getter for predictedAddresses
    const std::vector<int>& GetPredictedAddresses() const {
        return predictedAddresses;
    }

    virtual ~BaseFetcher() = default;
};

#endif // BASEFETCHER_H
