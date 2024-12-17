#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;
unsigned long long hits = 0;
unsigned long long accesses = 0;

// [10%] Direct-Mapped Cache. Assume that each cache line has a size of 32
// bytes and model the caches sized at 1KB, 4KB, 16KB and 32KB
void directMappedCache(string filePath, ofstream &fout, int cacheSize) {
    hits = 0;
    accesses = 0;
    int cacheLineSize = 32;
    int numBlocks = cacheSize * 1024 / cacheLineSize;
    vector<unsigned long long> cache(numBlocks, 0);

    ifstream infile(filePath);
    string line;
    while (getline(infile, line)) {
        accesses++;
        stringstream s(line);
        char action;
        unsigned long long address;
        s >> action >> hex >> address;

        int cacheIdx = (address / cacheLineSize) % numBlocks;
        unsigned long long tag = address / (cacheLineSize * numBlocks);
        if (cache[cacheIdx] == tag)
            hits++;
        else
            cache[cacheIdx] = tag;
    }
    fout << hits << "," << accesses << "; ";
}

// [20%] Set-Associative Cache. Again, assume that the cache line size is 32
// bytes and model a 16KB cache with associativity of 2, 4, 8 and 16. Assume
// that the least recently used (LRU) replacement policy is implemented.
void setAssociativeCache(string filePath, ofstream &fout, int associativity) {
    struct CacheBlock {
        unsigned long long tag;
        int age;
    };
    hits = 0;
    accesses = 0;
    int numBlocks = 512;
    int cacheLineSize = 32;
    int numOfSets = numBlocks / associativity;
    // Two-dimensional vector where each element represents a set, and each set 
    // contains a number of blocks equal to the cache's associativity. 
    vector<vector<CacheBlock>> cache(numOfSets, vector<CacheBlock>(associativity));

    ifstream infile(filePath);
    string line;
    while (getline(infile, line)) {
        accesses++;
        stringstream s(line);
        char action;
        unsigned long long address;
        s >> action >> hex >> address;

        int setIdx = (address / cacheLineSize) % numOfSets;
        unsigned long long tag = address / (cacheLineSize * numOfSets);
        bool hit = false;
        int hitIdx = -1;
        for (int i = 0; i < associativity; i++) {
            if (cache[setIdx][i].tag == tag) {
                hit = true;
                hitIdx = i;
                hits++;
                break;
            }
        }

        if (!hit) {
            // Find the least recently used block in the set, i.e
            // the block with smallest age value
            int lruIdx = 0;
            int minAge = cache[setIdx][0].age;
            for (int i = 1; i < associativity; i++) {
                if (cache[setIdx][i].age < minAge) {
                    minAge = cache[setIdx][i].age;
                    lruIdx = i;
                }
            }
            cache[setIdx][lruIdx].tag = tag;
            cache[setIdx][lruIdx].age = accesses;
        } else {
            cache[setIdx][hitIdx].age = accesses;
        }
    }
    fout << hits << "," << accesses << "; ";
}

// [20%] Fully-Associative cache. Assume that each cache line is 32 bytes and
// the total cache size is 16KB. Implement Least Recently Used (LRU) and
// hot-cold LRU approximation policies. For the hot-cold LRU approximation
// policy the initial state of all hot-cold bits should be 0 corresponding to
// the case where the left child is “hot” and the right child is “cold”.
// Furthermore, the policy should be utilized (and updated) for all accesses,
// including placing the initial blocks into the cache as well as replacements
// once the cache is full.
void fullyAssociativeCache(string filePath, ofstream &fout, bool hotColdLRU) {
    struct CacheBlock {
        unsigned long long tag; //Stores tag
        int age;  //Used to determine how recently block was accessed   
        bool hotColdBit;  //Indicates hot or cold 
    };
    int hits = 0;
    int accesses = 0;
    int cacheLineSize = 32;
    int numBlocks = 16 * 1024 / cacheLineSize;
    vector<CacheBlock> cache(numBlocks);
	ifstream infile(filePath);
    string line;
    while (getline(infile, line)) {
        accesses++;
        stringstream s(line);
        char action;
        unsigned long long address;
        s >> action >> hex >> address;

        unsigned long long tag = address / cacheLineSize;
        bool hit = false;
        int hitIdx = -1;
        for (int i = 0; i < numBlocks; i++) {
            if (cache[i].tag == tag) {
                hit = true;
                hitIdx = i;
                hits++;
                break;
            }
        }

        // When there is no hit, replace the block based on policy
        if (!hit) {
            int replaceIdx = 0;
            if (hotColdLRU) {
                // "cache[i].hotColdBit < cache[replaceIdx].hotColdBit" implements
                // hot cold policy that checks what half of set is hot (0 = left half is cold, 1 = right half is cold)
                for (int i = 1; i < numBlocks; i++) {
                    if (cache[i].age < cache[replaceIdx].age 
                    || (cache[i].age == cache[replaceIdx].age && cache[i].hotColdBit < cache[replaceIdx].hotColdBit)) {
                        replaceIdx = i;
                    }
                }
            } else {
                for (int i = 1; i < numBlocks; i++) {
                    //Regular LRU policy, we find the least accessed block by
                    //comparing the lruCounts
                    if (cache[i].age < cache[replaceIdx].age) {
                        replaceIdx = i;
                    }
                }
            }
            cache[replaceIdx].tag = tag;
            cache[replaceIdx].age = accesses;
            cache[replaceIdx].hotColdBit = false;
        } else {
            cache[hitIdx].age = accesses;
            if (hotColdLRU) {
                cache[hitIdx].hotColdBit = true;
            }
        }
    }
    fout << hits << "," << accesses << ";" << endl;
}

// [10%] Set-Associative Cache with no Allocation on a Write Miss. In this
// design, if a store instruction misses into the cache, then the missing line
// is not written into the cache, but instead is written directly to memory.
// Evaluate this design for the same configurations as in question (2) above.
void setAssociativeNoWriteCache(string filePath, ofstream &fout, int associativity) {
    struct CacheBlock {
        unsigned long long tag;
        int age;
    };
    int hits = 0;
    int accesses = 0;
    int numBlocks = 512;
    int cacheLineSize = 32;
    int numOfSets = numBlocks / associativity;
    vector<vector<CacheBlock>> cache(numOfSets, vector<CacheBlock>(associativity));

	ifstream infile(filePath);
    string line;
    while (getline(infile, line)) {
        accesses++;
        stringstream s(line);
        char action;
        unsigned long long address;
        s >> action >> hex >> address;

        int setIdx = (address / cacheLineSize) % numOfSets;
        unsigned long long tag = address / (cacheLineSize * numOfSets);
        bool hit = false;
        int hitIdx = -1;
        for (int i = 0; i < associativity; i++) {
            if (cache[setIdx][i].tag == tag) {
                hit = true;
                hitIdx = i;
                hits++;
                break;
            }
        }

        // Write to memory = do nothing in our case
        if (!hit && action == 'S') {
            
        } else if (!hit) {
            // Find the least recently used block
            int lruIdx = 0;
            int minAge = cache[setIdx][0].age;
            for (int i = 1; i < associativity; i++) {
                if (cache[setIdx][i].age < minAge) {
                    minAge = cache[setIdx][i].age;
                    lruIdx = i;
                }
            }
            // Replace the least recently used block
            cache[setIdx][lruIdx].tag = tag;
            cache[setIdx][lruIdx].age = accesses;
        } else {
            cache[setIdx][hitIdx].age = accesses;
        }
    }
    fout << hits << "," << accesses << "; ";
}

// [20%] Set-Associative Cache with Next-line Prefetching. In this design, the
// next cache line will be brought into the cache with every cache access. For
// example, if current access is to line X, then line (x+1) is also brought into
// the cache, replacing the cache’s previous content. Evaluate this design for
// the same configurations as in question (2) above. Note that prefetched blocks
// should update the LRU order of the corresponding set meaning that the
// prefetched block should become the most recently used block in its set.
void setAssociativeNextLinePrefetchingCache(string filePath, ofstream &fout) {
    bool hit = false;
    int cacheSize = 16384;
    vector<int> assoc = {2, 4, 8, 16};
    struct CacheBlock {
        unsigned long long tag;
        int age;
    };

    for (int i = 0; i < 4; i++) {
        int tag = 0;
        hits = 0;
        accesses = 0;

        int cacheLineSize = 32;
        int numBlocks = cacheSize / cacheLineSize;
        int associativity = assoc[i];
        int numOfSets = numBlocks / associativity;
        vector<vector<CacheBlock>> cache(numOfSets, vector<CacheBlock>(assoc[i]));
        int indexBits = log2(numOfSets);  

        string line;
        ifstream infile(filePath);
        while (getline(infile, line)) {
            accesses++;
            stringstream s(line);
            char action;
            unsigned long long address;
            s >> action >> hex >> address;

            // Calculate index, tag, and block adress to find which set we'll put data into
            int blockAddr = address / cacheLineSize;
            int setIdx = blockAddr % numOfSets;
            tag = address / (cacheLineSize * numOfSets);
            hit = false;
            int hitIdx = -1;

            for (int j = 0; j < associativity; j++) {
                if ((cache[setIdx][j].tag == tag)) {
                    hit = true;
                    hitIdx = j;
                    hits++;
                    cache[setIdx][hitIdx].age = accesses;
                    break;
                }
            }
            if (!hit) {
                int LRU = cache[setIdx][0].age;
                int lruIdx = 0;
                for (int j = 1; j < associativity; j++) {
                    if (cache[setIdx][j].age < LRU) {
                        LRU = cache[setIdx][j].age;
                        lruIdx = j;
                    }
                }
                // place the data into the set and update the age
                cache[setIdx][lruIdx].age = accesses;
                cache[setIdx][lruIdx].tag = tag;
            }

            // get the next address for prefetching
            unsigned long long nextAddr = address + cacheLineSize;
            int nextBlockAddr = nextAddr / cacheLineSize;
            int nextSetIdx = nextBlockAddr % numOfSets;
            unsigned long long nextTag = nextAddr / (cacheLineSize * numOfSets);
            bool nextHit = false;
            int nextHitIdx = -1;
            for (int j = 0; j < associativity; j++) {
                if (cache[nextSetIdx][j].tag == nextTag) {
                    nextHit = true;
                    cache[nextSetIdx][j].age = accesses;
                    nextHitIdx = j;
                    break;
                }
            }

            if (!nextHit) {
                // find the LRU in the set
                int nextLRUIdx = 0;
                int nextLRU = cache[nextSetIdx][0].age;
                for (int j = 1; j < associativity; j++) {
                    if (cache[nextSetIdx][j].age < nextLRU) {
                        nextLRU = cache[nextSetIdx][j].age;
                        nextLRUIdx = j;
                    }
                }
                // replace LRU with new cache line
                cache[nextSetIdx][nextLRUIdx].tag = nextTag;
                cache[nextSetIdx][nextLRUIdx].age = accesses;
            }
        }
        fout << hits << "," << accesses << ";" << " ";
        hits = 0;
        accesses = 0;
    }
}


void setAssociativePrefetchOnMissCache(string filePath, ofstream &fout) {
    struct CacheBlock {
        unsigned long long tag;
        int age;
    };
    
    int cacheSize = 16384;
    vector<int> assoc = {2, 4, 8, 16};

    for (int i = 0; i < 4; i++) {
        int hits = 0;
        int accesses = 0;
        int cacheLineSize = 32;
        int numBlocks = cacheSize / cacheLineSize;
        int associativity = assoc[i];
        int numOfSets = numBlocks / associativity;
        vector<vector<CacheBlock>> cache(numOfSets, vector<CacheBlock>(associativity));

        ifstream infile(filePath);
        string line;
        while (getline(infile, line)) {
            accesses++;
            stringstream s(line);
            char action;
            unsigned long long address;
            s >> action >> hex >> address;

            int setIdx = (address / cacheLineSize) % numOfSets;
            unsigned long long tag = address / (cacheLineSize * numOfSets);
            bool hit = false;
            //When theres a hit, set the age (recentcy) to accesses which is a very
            //large number, to show that it is most recently accessed
            for (int j = 0; j < associativity; j++) {
                if (cache[setIdx][j].tag == tag) {
                    hit = true;
                    hits++;
                    cache[setIdx][j].age = accesses;
                    break;
                }
            }

            if (!hit) {
                // Find the least recently used block in the set
                int lruIdx = 0;
                int minAge = cache[setIdx][0].age;
                for (int j = 1; j < associativity; j++) {
                    if (cache[setIdx][j].age < minAge) {
                        minAge = cache[setIdx][j].age;
                        lruIdx = j;
                    }
                }

                // Replace the least recently used block
                cache[setIdx][lruIdx].tag = tag;
                cache[setIdx][lruIdx].age = accesses;

                // Prefetch the next cache line
                unsigned long long nextAddr = address + cacheLineSize;
                int nextSetIdx = (nextAddr / cacheLineSize) % numOfSets;
                unsigned long long nextTag = nextAddr / (cacheLineSize * numOfSets);

                //Checks if next line is in cache, if so we update age of this line
                //to most recently used, indicating no prefetching is needed
                bool nextHit = false;
                for (int j = 0; j < associativity; j++) {
                    if (cache[nextSetIdx][j].tag == nextTag) {
                        nextHit = true;
                        cache[nextSetIdx][j].age = accesses;
                        break;
                    }
                }

                //f the next line's tag is not found (!nextHit), the cache must prefetch this line.
                if (!nextHit) {
                    // Find the least recently used block in the next set and replace it
                    int nextLRUIdx = 0;
                    int nextLRU = cache[nextSetIdx][0].age;
                    for (int j = 1; j < associativity; j++) {
                        if (cache[nextSetIdx][j].age < nextLRU) {
                            nextLRU = cache[nextSetIdx][j].age;
                            nextLRUIdx = j;
                        }
                    }
                    cache[nextSetIdx][nextLRUIdx].tag = nextTag;
                    cache[nextSetIdx][nextLRUIdx].age = accesses;
                }
            }
        }
        fout << hits << "," << accesses << "; ";
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Invalid Arguemnts" << endl;
        return 1;
    }
    ofstream fout(argv[2]);

    directMappedCache(argv[1], fout, 1);
    directMappedCache(argv[1], fout, 4);
    directMappedCache(argv[1], fout, 16);
    directMappedCache(argv[1], fout, 32);
    fout << endl;

    setAssociativeCache(argv[1], fout, 2);
    setAssociativeCache(argv[1], fout, 4);
    setAssociativeCache(argv[1], fout, 8);
    setAssociativeCache(argv[1], fout, 16);
    fout << endl;

    fullyAssociativeCache(argv[1], fout, false);
    fullyAssociativeCache(argv[1], fout, true);

    setAssociativeNoWriteCache(argv[1], fout, 2);
    setAssociativeNoWriteCache(argv[1], fout, 4);
    setAssociativeNoWriteCache(argv[1], fout, 8);
    setAssociativeNoWriteCache(argv[1], fout, 16);
    fout << endl;

    setAssociativeNextLinePrefetchingCache(argv[1], fout);
    fout << endl;

    setAssociativePrefetchOnMissCache(argv[1], fout);
    fout << endl;

    fout.close();
    return 0;
}
