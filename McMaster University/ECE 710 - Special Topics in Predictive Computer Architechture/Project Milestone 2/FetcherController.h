// FetcherController.h
#ifndef FETCHERCONTROLLER_H
#define FETCHERCONTROLLER_H

#include "StaticPrefetcher.h"
#include "DynamicPrefetcher.h"
#include "BaseFetcher.h"
#include <vector>

class FetcherController {
private:
    std::vector<BaseFetcher*> prefetchers; 
    size_t currentFetcherIndex;

public:
    FetcherController() : currentFetcherIndex(0) {
        // You can add fetchers here if needed
    }

    // Method to add a fetcher to the controller
    void AddPrefetcher(BaseFetcher* prefetcher) {
        prefetchers.push_back(prefetcher);
    }

    void OperateFetcher(int index) {
    if (prefetchers.empty()) {
        // Invalid index
        std::cerr << "Invalid fetcher index." << std::endl;
        return;
    } else {
        // Get the fetcher based on the provided index
        BaseFetcher* currentPrefetcher = prefetchers[index];

       
        // Increment the counter and switch to the next prefetcher
        currentFetcherIndex = (index + 1) % prefetchers.size();
    }
}

    BaseFetcher* getCurrentFetcher() {
        // Get the current fetcher
        return prefetchers[currentFetcherIndex];
    }

    // Destructor to release allocated memory
    ~FetcherController() {
        for (BaseFetcher* fetcher : prefetchers) {
            delete fetcher;
        }
    }
};

#endif // FETCHERCONTROLLER_H
