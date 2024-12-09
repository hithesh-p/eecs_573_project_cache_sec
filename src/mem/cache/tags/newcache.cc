/**
 * Newccache.cc based on original version written by Erik Hallnor.
 * This version is updated to utilize python 3 along with the newer release of gem5.
 */

#include "mem/cache/tags/newcache.hh"
#include "mem/cache/tags/base.hh"
#include "mem/cache/base.hh"

namespace gem5
{
    NewCache::NewCache(const Params &p)
            : BaseTags(p), assoc(p.assoc),
            numSets(p.size / (p.block_size * p.assoc)),
            neBit(p.neBit)
    {
        // Check parameters
        if (blkSize < 4 || !isPowerOf2(blkSize)) {
            fatal("Block size must be at least 4 and a power of 2");
        }
        if (numSets <= 0 || !isPowerOf2(numSets)) {
            fatal("# of sets must be non-zero and a power of 2");
        }
        assert(numSets == 1);
        if (assoc <= 0){
            fatal("associativity must be greater than zero");
        }
        if (lookupLatency <= 0) {
            fatal("access latency must be greater than zero");
        }
        if (neBit < 0) {
            fatal("number of extra index bits must be greater than zero");
        }

        blkMask = blkSize - 1;
        setShift = floorLog2(blkSize);
        setMask = numSets - 1;
        tagShift = setShift + floorLog2(numSets*assoc) + neBit;
        diMask = (1 << (floorLog2(numSets*assoc) + neBit)) - 1;
        // NOTE: There is a comment about making this percentage a parameter.
        warmedUp = false;
        // NOTE: It's const, so we need to pass this as a parameter for BaseTags.
        // warmupBound = numSets * assoc;

        sets = new SetType[numSets];
        blks = new BlkType[numSets * assoc];
        // Allocate data storage in one big chunk
        // TODO: This is also a const now
        // numBlocks = numSets * assoc;
        dataBlks = new uint8_t[numBlocks * blkSize];

        unsigned blkIdx = 0; // Index to blocks array
        for (unsigned i = 0; i < numSets; ++i)
        {
            sets[i].assoc = assoc;
            sets[i].blks = new BlkType*[assoc];

            // Link the data blocks
            for (unsigned j = 0; j < assoc; ++j)
            {
                // Locate next cache block
                BlkType *blk = &blks[blkIdx];
                blk->data = &dataBlks[blkSize * blkIdx];
                ++blkIdx;

                // Invalidate new block
                blk->invalidate();

                // Set tag to j to prevent long chains in hash table
                // Block is invalid, shouldn't matter anyways
                blk->tag = j;
                // Init lnreg
                blk->lnreg = 0;
                blk->P_bit = -1;
                blk->rmtid = -1;
                blk->whenReady = 0;
                blk->isTouched = false;
                blk->size = blkSize;
                sets[i].blks[j]=blk;
                blk->set = i;
            }
        }
    }

    NewCache::~NewCache()
    {
        delete [] dataBlks;
        delete [] blks;
        delete [] sets;
    }

    NewCache::BlkType* NewCache::accessBlock(PacketPtr pkt, Cycles &lat, int context_src)
    {
        Addr addr = pkt->getAddr();
        Addr tag = extractTag(addr);
        unsigned set = extractSet(addr);
        int lnreg = extractDI(addr);
        int rmtid = pkt->req->rmtid; 
        int P_bit = 0;
        if (pkt->req->isSecure())
        P_bit = 1;
        // kernel is shared by all the processes and the global translation has a default rmt_id (0)
        if (pkt->req->isGlobal())
        rmtid = 0;
        BlkType *blk = NULL;
        blk = sets[set].mapLookup(rmtid, P_bit, lnreg);
        if (blk && blk->isValid() && (blk->getTag() != tag)){
        // tag miss
        blk = NULL;
        }
        lat = lookupLatency;
        if (blk != NULL) {
            DPRINTF(CacheRepl, "set %x: moving blk %x to MRU\n",
                    set, regenerateBlkAddr(tag, set, lnreg));
            if (blk->whenReady > curTick()
                && cache->ticksToCycles(blk->whenReady - curTick()) > lookupLatency) {
                lat = cache->ticksToCycles(blk->whenReady - curTick());
            }
            // blk->refCount += 1;
            blk->increaseRefCount();
        }

        return blk;
    }

    NewCache::BlkType* NewCache::findBlock(PacketPtr pkt) const
    {
        Addr addr = pkt->getAddr();
        Addr tag = extractTag(addr);
        unsigned set = extractSet(addr);
        unsigned lnreg = extractDI(addr);
        BlkType *blk = NULL;
        int rmtid = pkt->req->getRmtid();
        int P_bit = 0;
        if (pkt->req->isSecure())
        P_bit = 1;
        if (pkt->req->isGlobal())
        rmtid = 0;

        blk = sets[set].findBlk(rmtid, P_bit, lnreg, tag);
        if (blk && blk->isValid() && (blk->getTag() != tag)){
        // tag miss
        blk = NULL;
        }

        return blk;
    }

    NewCache::BlkType* NewCache::findVictim(PacketPtr pkt, PacketList &writebacks)
    {
        Addr addr = pkt->getAddr();
        unsigned set = extractSet(addr);
        unsigned lnreg = extractDI(addr);
        Addr tag = extractTag(addr);
        BlkType *blk = NULL;
        BlkType *tmpblk = NULL;
        uint16_t rpl;
        int rmtid = pkt->req->getRmtid();
        int P_bit = 0;
        if (pkt->req->isSecure())
        P_bit = 1;
        if (pkt->req->isGlobal())
        rmtid = 0;

        // grab a replacement candidate
        tmpblk = sets[set].mapLookup(rmtid, P_bit, lnreg);
        if (tmpblk){
            assert(tmpblk->tag != tag);
            blk = tmpblk;
        }
        else {
        rpl = random_mt.random<uint64_t>(0,assoc-1);
        blk = sets[set].blks[rpl];

        }

        return blk;
    }

    void NewCache::insertBlock(PacketPtr pkt, BlkType *blk)
    {
        Addr addr = pkt->getAddr();
        RequestorID master_id = pkt->req->requestorId();
        Addr tag = extractTag(addr);
        unsigned lnreg = extractDI(addr);
        unsigned set = extractSet(addr);
    
        int rmtid = pkt->req->getRmtid();
        int P_bit = 0;
        if (pkt->req->isSecure())
        P_bit = 1;
        if (pkt->req->isGlobal())
        rmtid = 0; 

        // don't change the order of the code, make sure not invalidate block before inserting
        // Set tag for new block.  Caller is responsible for setting status.
        // tag miss
        if ((blk->rmtid == rmtid) && (blk->P_bit==P_bit) && (blk->lnreg==lnreg) && blk->isValid()){
            blk->tag = tag;
            blk->rmtid = rmtid;
            assert(sets[set].inMap(blk->rmtid, blk->P_bit, blk->lnreg));
        } else {
            //erase the mapping
            if (blk->isValid() && sets[set].inMap(blk->rmtid, blk->P_bit, blk->lnreg)){
            sets[set].lnregMap.erase(sets[set].setKey(blk->rmtid, blk->P_bit, blk->lnreg));
            }
            // update lnreg and hash table
            blk->tag = tag;
            blk->P_bit = P_bit;
            blk->lnreg = lnreg;
            blk->rmtid = rmtid;
            sets[set].lnregMap[sets[set].setKey(rmtid, P_bit, lnreg)] = blk;
        }

        if (!blk->isTouched) {
            tagsInUse++;
            blk->isTouched = true;
            if (!warmedUp && tagsInUse.value() >= warmupBound) {
                warmedUp = true;
                warmupCycle = curTick();
            }
        }

        // If we're replacing a block that was previously valid update
        // stats for it. This can't be done in findBlock() because a
        // found block might not actually be replaced there if the
        // coherence protocol says it can't be.
        if (blk->isValid()) {
            replacements[0]++;
            totalRefs += blk->getRefCount();
            ++sampledRefs;
            blk->refCount = 0;
            // deal with evicted block
            assert(blk->srcMasterId < cache->system->maxMasters());
            occupancies[blk->getSrcRequestorId()]--;

            blk->invalidate();
        }

        blk->isTouched = true;

        // deal with what we are bringing in
        assert(master_id < cache->system->maxMasters());
        occupancies[master_id]++;
        blk->srcMasterId = master_id;
    }

    void NewCache::invalidate(BlkType *blk)
    {
        assert(blk);
        assert(blk->isValid());
        tagsInUse--;
        assert(blk->srcMasterId < cache->system->maxMasters());
        occupancies[blk->getSrcRequestorId()]--;
        blk->srcMasterId = Request::invldRequestorId;

        if (sets[blk->getSet()].inMap(blk->rmtid, blk->P_bit, blk->lnreg))
            sets[blk->getSet()].lnregMap.erase(sets[blk->getSet()].setKey(blk->rmtid, blk->P_bit, blk->lnreg));
    }

    void NewCache::clearLocks()
    {
        for (int i = 0; i < numBlocks; i++)
        {
            blks[i].clearLoadLocks();
        }
    }

    // NOTE: This doesn't seem to exist anywhere...
    // NewCache *NewCacheParams::create()
    // {
    //     return new NewCache(this);
    // }

    std::string NewCache::print() const 
    {
        std::string cache_state;
        for (unsigned i = 0; i < numSets; ++i) {
            // link in the data blocks
            for (unsigned j = 0; j < assoc; ++j) {
                BlkType *blk = sets[i].blks[j];
                if (blk->isValid())
                    cache_state += csprintf("\tset: %d block: %d %s\n", i, j,
                            blk->print());
            }
        }
        if (cache_state.empty())
            cache_state = "no valid tags\n";
        return cache_state;
    }

    void NewCache::cleanupRefs()
    {
        for (unsigned i = 0; i < numSets*assoc; ++i)
        {
            if (blks[i].isValid())
            {
                totalRefs += blks[i].getRefCount();
                ++sampledRefs;
            }
        }
    }
    
} // namespace gem5
