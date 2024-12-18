
# BranchPredictor-CacheSimulator

The goal of this project is to measure the effectiveness of several branch direction predictors on a number of traces of conditional branch instructions (BranchPredictor) and to measure the effectiveness of cache subsystem organizations using traces of memory instructions obtained from realistic programs (CacheSimulator)


## BranchPredictor Outline

* Each line in the Branch Predictor trace contains the program counter (expressed as a word address), the actual outcome of the branch, and the target address of the branch
* The branch outcomes from the trace file are used to train the predictors used
*  Output text file of program consists of x,y pairs, where each x corresponds to number of correct predictions made by each of the predictors, and each y corresponds to total number of branches. First line provides results for number of correct predictions for the Always Taken, second line for Always Non-Taken, third line for all seven variations of the Bimodal Predictor with a single bit of history (table of size 16, 32 …. etc), fourth line for the two-bit saturating counter based Bimodal Predictor, fifth line for nine variations of Gshare Predictor, sixth line for Tournament Predictor, and seventh line for BTB where x corresponds to number of correct branch target predictions, and y corresponds to number of attempted predictions (since BTB only attempts to predict branch target when prediction is taken)

## BranchPredictor Implementation
* Always Taken: Always predicts branch will be taken 
* Always Not-Taken: Always predicts branch will not be taken
* Bimodal Predictor (Single Bit History): Uses table to track outcome of branches based on recenty history. Determines prediction accuracy with table size of 6, 32, 128, 256, 512, 1024 and 2048 entries. Initial state of all prediction counters is assumed as “Taken” (T), Single Bit History => Store “Taken” (T) or "Not Taken" (NT)
* Bimodal Predictor (2-bit Saturating Counters): Uses table to track outcome of branches based on recenty history. Determines prediction accuracy with table size of 6, 32, 128, 256, 512, 1024 and 2048 entries. Initial state of all prediction counters is assumed as “Strongly Taken” (ST), 2-bit => Store “Strongly Not-Taken” (SNT), "Weakly Not-Taken" (WNT), "Weakly Taken" (WT), or "Strongly Taken" (ST)
* Gshare Predictor: PC is XOR-ed with the global history bits to generate the index into the predictor table. Determnes prediction accuracy as function of the number of bits in the global history register. Table is fized size of 2048 entries, history length is varied from 3 bits to 11 bits in 1-bit increments. Initial state of all prediction counters is assumed as Strongly Taken” (ST). The global history register is initialized to contain all zeroes (where 0=NT and 1=T). The global history register is maintained in a way such that the least significant bit of the register represents the result of the most recent branch, and the most significant bit of the register represents the result of the least recent branch in the history
* Tournament Predictor: Selects between Gshare and bimodal predictor for every branch. Gshare is configured with table size of 2048 entries and 11 bits of global history. Bimodal is configured with table size of 2048 entries. For each entry in the selector, the two-bit counter encodes the following states: 00 (Prefer Gshare) , 01 (Weakly Prefer Gshare), 10 (Weakly Prefer Bimodal), 11 (Prefer Bimodal). If the two predictors provide the same prediction, then corresponding selector counter remains the same. If one of the predictors is correct and the other one is wrong, then the selector’s counter is decremented or incremented to move towards the predictor that was correct
* Branch Target Buffer (BTB): BTB is integrated with the Bimodal Predictor of size 512 entries. Size of the BTB is also 512 entries, where each entry contains predicted branch target. BTB is indexed using the PC of the branch instruction. If a prediction is “Taken”, then the predicted target address is read from the BTB. If the actual behavior is “Taken”, then the BTB is updated with the correct branch target

## BranchPredictor Results
The following results were based on the secret-trace.txt file, which contains ~10,000,000 branch instructions
* Always Taken: 3,634,666 correct predictions
* Always Not-Taken: 6,365,334 correct predictions
* Bimodal (One bit History): 7,323,526 correct predictions (Table size 16KB), 7,467,573 correct predictions (Table size 32KB), 8,561,861 correct predictions (Table size 64KB), 8,744,725 correct predictions (Table size 128KB), 8,903,997 correct predictions (Table size 256KB), 9,049,328 correct predictions (Table size 512KB), 9,129,557 correct predictions (Table size 1024KB)
* Bimodal (Two bit History): 7,933,716 correct predictions (Table size 16KB), 8,041,369 correct predictions (Table size 32KB), 8,805,438 correct predictions (Table size 64KB), 9,047,466 correct predictions (Table size 128KB), 9,188,009 correct predictions (Table size 256KB), 9,322,297 correct predictions (Table size 512KB), 9,392,829 correct predictions (Table size 1024KB)
* Gshare: 9,467,918 correct predictions (Table size 16KB), 9,427,395 correct predictions (Table size 32KB), 9,353,733 correct predictions (Table size 64KB), 9,373,370 correct predictions (Table size 128KB), 9,379,025 correct predictions (Table size 256KB), 9,388,446 correct predictions (Table size 512KB), 9,359,756 correct predictions (Table size 1024KB), 9,344,103 correct predictions (Table size 2048KB)
* Tournament: 9,657,332 correct predictions (Table size 2048KB)
* Branch Target Buffer (BTB): (Table size 2048KB) 2,973,497 correct branch target predicitions, and 3,635,021 attempted branch target predictions


## CacheSimulator Outline

* Each trace contains memory instructions with two values provided for each instruction: a flag indicating whether this is a load or a store (L stands for a load, S stands for a store), and the byte memory address targeted by this instruction
* The traces are used to measure the cache hit rate of various data cache organizations and prefetching techniques (not estimating instruction cache performance, only the data cache)
* Output text file of program consists of x,y pairs, where each x corresponds to the number of cache hits for each policy, and each y corresponds to total number of accesses for each cache configuration. First line provides the results for Direct Mapped, second line for Set-Associative, third line for the Fully-Associative with LRU replacement, fourth line for Fully-Associative with hot-cold replacement, fifth for the associative caches without store allocation, sixth line for associative caches with next line prefetching, and seventh line for associative caches with next line prefetching only for cache misses
## CacheSimulator Implentation
* Direct Mapped Cache (DMC): Assumes that each cache line has a size of 32 bytes, and simulates storing caches with size 1KB, 4KB, 16KB and 32KB
* Set-Associative Cache (SAC): Assumes that the cache line size is 32 bytes and simulates a 16KB cache with associativity of 2, 4, 8 and 16. Least Recently Used (LRU) replacement policiy is implemented (Item accessed least recently is replaced from cache)
* Set-Associative Cache (SAC) with no Allocation on a Write Miss: Assumes that the cache line size is 32 bytes and simulates a 16KB cache with associativity of 2, 4, 8 and 16. In this design, if store instruction misses into cache, then missing line is not written into cache, but instead written directly to memory
* Set-Associative Cache (SAC) with Next-line Prefetching: Assumes that the cache line size is 32 bytes and simulates a 16KB cache with associativity of 2, 4, 8 and 16. In this design, next cache line is brought into cache with every cache access. For example, if current access is to line X, then line (x+1) is also brought into the cache, replacing the cache’s previous content. The prefetched blocks updates the LRU order of the corresponding set. I.E the prefetched block becomes the most recently used block in its set
* Set-Associative Cache (SAC) with Prefetch-on-a-Miss: Same as Set-Associative Cache (SAC) with Next-line Prefetching, except that prefetching is only triggered on cache miss
* Fully-Associative Cache (FAC): Assumes each cache line is 32 bytes and the total cache size is 16KB. Least Recently Used (LRU) and hot-cold LRU approximation policies are implemented. For the hot-cold LRU approximation policy, initial state of all hot-cold bits are 0 (Case where the left child is “hot” and the right child is “cold”). The policy is utilized and updated for all accesses, including placing initial blocks into the cache as well as replacements once cache is full
## CacheSimulator Results
The following results were based on the extra-trace.txt file, which contains ~1,000,000 traces
* Direct Mapped: 508,669 hits (cache sized at 1KB), 698,608 hits(cache sized at 4KB), 804,127 hits (cache sized at 16KB), 831,169 (cache sized at 32KB)
* Set-Associative: 825,553 hits (cache sized at 1KB), 831,495 hits(cache sized at 4KB), 833,981 hits (cache sized at 16KB), 835,233 (cache sized at 32KB)
* Fully-Associative LRU: 835,907 hits (cache sized at 16KB)
* Fully-Associative Hot-Cold: 835,907 hits (cache sized at 16KB)
* Set-Associative with no Allocation on Write Miss: 823,922 hits (cache sized at 1KB), 830,569 hits(cache sized at 4KB), 833,152 (cache sized at 16KB), 834,382 (cache sized at 32KB)
* Set-Associative with Next-line Prefetching: 825,110 hits (cache sized at 1KB), 830,592 hits(cache sized at 4KB), 833,086 (cache sized at 16KB), 834,575 (cache sized at 32KB)
* Set-Associative with Prefetch-on-a-Miss: 818,027 hits (cache sized at 1KB), 824,299 hits(cache sized at 4KB), 826,933 (cache sized at 16KB), 828,353 (cache sized at 32KB)
