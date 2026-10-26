#include "fifo.h"

#include <stdexcept>

FIFO::FIFO(int capacity)
    : cache(capacity),
      nextVictim(0),
      hits(0),
      misses(0),
      evictions(0) {

    if (capacity <= 0) {
        throw std::invalid_argument(
            "Cache capacity must be greater than 0."
        );
    }
}

bool FIFO::access(int blockId) {

    // Check for a cache hit.
    for (const auto& line : cache) {

        if (line.blockId == blockId) {
            hits++;
            return true;
        }
    }

    // Cache miss.
    misses++;

    // Find an empty slot before evicting anything.
    for (auto& line : cache) {

        if (line.blockId == -1) {

            line.blockId = blockId;
            return false;
        }
    }

    // Cache is full.
    // Replace the oldest entry.
    cache[nextVictim].blockId = blockId;

    nextVictim = (nextVictim + 1) % cache.size();

    evictions++;

    return false;
}

int FIFO::getHits() const {
    return hits;
}

int FIFO::getMisses() const {
    return misses;
}

int FIFO::getEvictions() const {
    return evictions;
}

int FIFO::getAccesses() const {
    return hits + misses;
}

double FIFO::getHitRate() const {

    if (getAccesses() == 0) {
        return 0.0;
    }

    return (static_cast<double>(hits) / getAccesses()) * 100.0;
}