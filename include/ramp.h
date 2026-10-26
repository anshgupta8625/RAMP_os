#ifndef RAMP_H
#define RAMP_H

#include <vector>
#include "cache_line.h"

class RAMP {
private:
    std::vector<CacheLine> cache;

    int currentTime;
    int hits;
    int misses;
    int evictions;

    double calculateScore(const CacheLine& line) const;

public:
    explicit RAMP(int capacity);

    bool access(int blockId);

    int getHits() const;
    int getMisses() const;
    int getEvictions() const;
    int getAccesses() const;

    double getHitRate() const;
};

#endif