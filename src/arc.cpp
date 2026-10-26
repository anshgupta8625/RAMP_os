#include "arc.h"

#include <algorithm>
#include <stdexcept>

ARC::ARC(int capacity)
    : capacity(capacity),
      p(0),
      hits(0),
      misses(0),
      evictions(0) {

    if (capacity <= 0) {
        throw std::invalid_argument(
            "Cache capacity must be greater than 0."
        );
    }
}

bool ARC::contains(
    const std::list<int>& list,
    int blockId
) const {

    return std::find(
        list.begin(),
        list.end(),
        blockId
    ) != list.end();
}

void ARC::moveToFront(
    std::list<int>& list,
    int blockId
) {

    list.remove(blockId);
    list.push_front(blockId);
}

void ARC::replace(int blockId) {

    // Prefer replacing from T1 when T1 is above
    // its adaptive target.
    if (!T1.empty() &&
        (static_cast<int>(T1.size()) > p ||
         (contains(B2, blockId) &&
          static_cast<int>(T1.size()) == p))) {

        int victim = T1.back();
        T1.pop_back();

        // Store only the block ID in the ghost list.
        B1.push_front(victim);

        evictions++;
    }
    else if (!T2.empty()) {

        int victim = T2.back();
        T2.pop_back();

        // Store only the block ID in the ghost list.
        B2.push_front(victim);

        evictions++;
    }
}

bool ARC::access(int blockId) {

    // ------------------------------------------------
    // Case 1: Block is in T1.
    // ------------------------------------------------

    if (contains(T1, blockId)) {

        hits++;

        // A repeated access promotes it to T2.
        T1.remove(blockId);
        T2.push_front(blockId);

        return true;
    }

    // ------------------------------------------------
    // Case 2: Block is in T2.
    // ------------------------------------------------

    if (contains(T2, blockId)) {

        hits++;

        // Refresh its position in T2.
        moveToFront(T2, blockId);

        return true;
    }

    // ------------------------------------------------
    // Case 3: Block is in B1.
    // ------------------------------------------------

    if (contains(B1, blockId)) {

        misses++;

        int b1Size = static_cast<int>(B1.size());
        int b2Size = static_cast<int>(B2.size());

        // Increase the target size of T1.
        if (b1Size >= b2Size) {
            p = std::min(capacity, p + 1);
        }
        else {
            p = std::min(
                capacity,
                p + std::max(1, b2Size / b1Size)
            );
        }

        replace(blockId);

        B1.remove(blockId);
        T2.push_front(blockId);

        return false;
    }

    // ------------------------------------------------
    // Case 4: Block is in B2.
    // ------------------------------------------------

    if (contains(B2, blockId)) {

        misses++;

        int b1Size = static_cast<int>(B1.size());
        int b2Size = static_cast<int>(B2.size());

        // Decrease the target size of T1.
        if (b2Size >= b1Size) {
            p = std::max(0, p - 1);
        }
        else {
            p = std::max(
                0,
                p - std::max(1, b1Size / b2Size)
            );
        }

        replace(blockId);

        B2.remove(blockId);
        T2.push_front(blockId);

        return false;
    }

    // ------------------------------------------------
    // Case 5: Completely new block.
    // ------------------------------------------------

    misses++;

    int residentBlocks =
        static_cast<int>(T1.size() + T2.size());

    int ghostBlocks =
        static_cast<int>(B1.size() + B2.size());

    // If the real cache is full, replace a resident block.
    if (residentBlocks >= capacity) {
        replace(blockId);
    }

    // Keep the total ghost metadata bounded.
    if (ghostBlocks >= capacity) {

        if (!B1.empty()) {
            B1.pop_back();
        }
        else if (!B2.empty()) {
            B2.pop_back();
        }
    }

    // A completely new block starts in T1.
    T1.push_front(blockId);

    return false;
}

int ARC::getHits() const {
    return hits;
}

int ARC::getMisses() const {
    return misses;
}

int ARC::getEvictions() const {
    return evictions;
}

int ARC::getAccesses() const {
    return hits + misses;
}

double ARC::getHitRate() const {

    if (getAccesses() == 0) {
        return 0.0;
    }

    return (
        static_cast<double>(hits) /
        getAccesses()
    ) * 100.0;
}