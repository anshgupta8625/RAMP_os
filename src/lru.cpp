#include "lru.h"

#include <algorithm>
#include <stdexcept>

LRU::LRU(int capacity)
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

bool LRU::access(int blockId) {

    currentTime++;

    // Check whether the block is already in the cache.
    for (auto& line : cache) {

        if (line.blockId == blockId) {

            // Cache hit.
            hits++;

            // This block was just accessed.
            line.lastAccess = currentTime;

            return true;
        }
    }

    // Cache miss.
    misses++;

    // First try to find an empty cache slot.
    for (auto& line : cache) {

        if (line.blockId == -1) {

            line.blockId = blockId;
            line.lastAccess = currentTime;

            return false;
        }
    }

    // Cache is full.
    // Find the least recently used block.
    auto victim = std::min_element(
        cache.begin(),
        cache.end(),
        [](const CacheLine& a, const CacheLine& b) {
            return a.lastAccess < b.lastAccess;
        }
    );

    // Replace the LRU block.
    victim->blockId = blockId;
    victim->lastAccess = currentTime;

    evictions++;

    return false;
}

int LRU::getHits() const {
    return hits;
}

int LRU::getMisses() const {
    return misses;
}

int LRU::getEvictions() const {
    return evictions;
}

int LRU::getAccesses() const {
    return hits + misses;
}

double LRU::getHitRate() const {

    if (getAccesses() == 0) {
        return 0.0;
    }

    return (static_cast<double>(hits) / getAccesses()) * 100.0;
}