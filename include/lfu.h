#ifndef LFU_H
#define LFU_H

#include <vector>
#include "cache_line.h"

class LFU {
private:
    std::vector<CacheLine> cache;

    int currentTime;
    int hits;
    int misses;
    int evictions;

public:
    explicit LFU(int capacity);

    bool access(int blockId);

    int getHits() const;
    int getMisses() const;
    int getEvictions() const;
    int getAccesses() const;

    double getHitRate() const;
};

#endif