#ifndef LRU_H
#define LRU_H

#include <vector>
#include "cache_line.h"

class LRU {
private:
    std::vector<CacheLine> cache;

    int currentTime;
    int hits;
    int misses;
    int evictions;

public:
    explicit LRU(int capacity);

    bool access(int blockId);

    int getHits() const;
    int getMisses() const;
    int getEvictions() const;
    int getAccesses() const;

    double getHitRate() const;
};

#endif