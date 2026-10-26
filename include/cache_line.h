#ifndef CACHE_LINE_H
#define CACHE_LINE_H

struct CacheLine {
    int blockId = -1;

    int lastAccess = 0;

    int previousAccess = 0;

    int frequency = 0;
};

#endif