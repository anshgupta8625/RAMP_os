#include "ramp.h"

#include <algorithm>
#include <stdexcept>

RAMP::RAMP(int capacity)
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

double RAMP::calculateScore(
    const CacheLine& line
) const {

    /*
        RAMP = Recency-Aware Momentum Priority

        The score consists of:

        1. Frequency:
           Indicates repeated reuse.

        2. Recency:
           Protects recently accessed blocks.

        3. Revival bonus:
           Protects a block that becomes active again
           after being inactive for a significant period.

        Higher score = stronger protection.
    */

    int age =
        currentTime - line.lastAccess;

    double recency =
        1.0 / (1.0 + age);

    double momentum =
        static_cast<double>(line.frequency);

    double revivalBonus = 0.0;

    /*
        A block has been revived if:

        - It has been accessed before.
        - There was a gap between its previous
          access and its most recent access.
        - That gap is at least 3 accesses.
    */

    if (line.previousAccess > 0) {

        int reuseGap =
            line.lastAccess - line.previousAccess;

        if (reuseGap >= 3) {

            revivalBonus = 2.0;
        }
    }

    /*
        RAMP score.

        Momentum is the main signal.
        Recency provides a small advantage.
        Revival provides temporary protection.
    */

    return
        momentum +
        recency +
        revivalBonus;
}

bool RAMP::access(int blockId) {

    currentTime++;

    // -----------------------------------------
    // Cache hit
    // -----------------------------------------

    for (auto& line : cache) {

        if (line.blockId == blockId) {

            hits++;

            /*
                Before updating lastAccess,
                preserve the previous access time.
            */

            line.previousAccess =
                line.lastAccess;

            line.lastAccess =
                currentTime;

            line.frequency++;

            return true;
        }
    }

    // -----------------------------------------
    // Cache miss
    // -----------------------------------------

    misses++;

    // Find empty slot.
    for (auto& line : cache) {

        if (line.blockId == -1) {

            line.blockId = blockId;

            line.lastAccess =
                currentTime;

            line.previousAccess = 0;

            line.frequency = 1;

            return false;
        }
    }

    // -----------------------------------------
    // Cache full.
    // Find lowest RAMP score.
    // -----------------------------------------

    auto victim = std::min_element(
        cache.begin(),
        cache.end(),
        [this](
            const CacheLine& a,
            const CacheLine& b
        ) {

            double scoreA =
                calculateScore(a);

            double scoreB =
                calculateScore(b);

            if (scoreA != scoreB) {
                return scoreA < scoreB;
            }

            /*
                If scores are equal,
                use LRU as tie-breaker.
            */

            return
                a.lastAccess <
                b.lastAccess;
        }
    );

    // -----------------------------------------
    // Replace victim
    // -----------------------------------------

    victim->blockId =
        blockId;

    victim->lastAccess =
        currentTime;

    victim->previousAccess = 0;

    victim->frequency = 1;

    evictions++;

    return false;
}

int RAMP::getHits() const {
    return hits;
}

int RAMP::getMisses() const {
    return misses;
}

int RAMP::getEvictions() const {
    return evictions;
}

int RAMP::getAccesses() const {
    return hits + misses;
}

double RAMP::getHitRate() const {

    if (getAccesses() == 0) {
        return 0.0;
    }

    return (
        static_cast<double>(hits) /
        getAccesses()
    ) * 100.0;
}