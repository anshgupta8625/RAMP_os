#include "lfu.h"

#include <algorithm>
#include <stdexcept>

LFU::LFU(int capacity)
    : cache(capacity),
      currentTime(0),
      hits(0),
      misses(0),
      evictions(0) {

    if (capacity <= 0) {
        throw std::invalid_argument(
            "Cache capacity must be greater than 0."
        );
    }
}

bool LFU::access(int blockId) {

    currentTime++;

    // Check for a cache hit.
    for (auto& line : cache) {

        if (line.blockId == blockId) {

            hits++;

            line.frequency++;
            line.lastAccess = currentTime;

            return true;
        }
    }

    // Cache miss.
    misses++;

    // Find an empty slot.
    for (auto& line : cache) {

        if (line.blockId == -1) {

            line.blockId = blockId;
            line.frequency = 1;
            line.lastAccess = currentTime;

            return false;
        }
    }

    // Cache is full.
    // Find the least frequently used block.
    //
    // If multiple blocks have the same frequency,
    // use LRU as the tie-breaker.
    auto victim = std::min_element(
        cache.begin(),
        cache.end(),
        [](const CacheLine& a, const CacheLine& b) {

            if (a.frequency != b.frequency) {
                return a.frequency < b.frequency;
            }

            return a.lastAccess < b.lastAccess;
        }
    );

    // Replace the LFU block.
    victim->blockId = blockId;
    victim->frequency = 1;
    victim->lastAccess = currentTime;

    evictions++;

    return false;
}

int LFU::getHits() const {
    return hits;
}

int LFU::getMisses() const {
    return misses;
}

int LFU::getEvictions() const {
    return evictions;
}

int LFU::getAccesses() const {
    return hits + misses;
}

double LFU::getHitRate() const {

    if (getAccesses() == 0) {
        return 0.0;
    }

    return (static_cast<double>(hits) / getAccesses()) * 100.0;
}