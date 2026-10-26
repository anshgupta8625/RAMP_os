#ifndef FIFO_H
#define FIFO_H

#include <vector>
#include "cache_line.h"

class FIFO {
private:
    std::vector<CacheLine> cache;

    int nextVictim;
    int hits;
    int misses;
    int evictions;

public:
    explicit FIFO(int capacity);

    bool access(int blockId);

    int getHits() const;
    int getMisses() const;
    int getEvictions() const;
    int getAccesses() const;

    double getHitRate() const;
};

#endif