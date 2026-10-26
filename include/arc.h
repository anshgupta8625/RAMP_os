#ifndef ARC_H
#define ARC_H

#include <list>

class ARC {
private:
    int capacity;
    int p;

    std::list<int> T1;
    std::list<int> T2;
    std::list<int> B1;
    std::list<int> B2;

    int hits;
    int misses;
    int evictions;

    bool contains(const std::list<int>& list, int blockId) const;

    void moveToFront(std::list<int>& list, int blockId);

    void replace(int blockId);

public:
    explicit ARC(int capacity);

    bool access(int blockId);

    int getHits() const;
    int getMisses() const;
    int getEvictions() const;
    int getAccesses() const;

    double getHitRate() const;
};

#endif