/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Erik Hallnor
 */

/**
 * @file
 * Declaration of a NEWCACHE tag store.
 */

#ifndef __MEM_CACHE_TAGS_NEWCACHE_HH__
#define __MEM_CACHE_TAGS_NEWCACHE_HH__

#include <cassert>
#include <cstring>
#include <list>

#include "mem/cache/tags/base.hh"
#include "base_dep/cacheset.hh"
#include "base_dep/blk.hh"
#include "mem/packet.hh"
#include "params/NEWCACHE.hh"

class BaseCache;


/**
 * A NEWCACHE cache tag store.
 * @sa  \ref gem5MemorySystem "gem5 Memory System"
 */
class NEWCACHE : public gem5::BaseTags
{
  public:
    /** Typedef the block type used in this tag store. */
    typedef CacheBlk BlkType;
    /** Typedef for a list of pointers to the local block class. */
    typedef std::list<BlkType*> BlkList;
    typedef CacheSet<CacheBlk> SetType;

  protected:
    /** The associativity of the cache. */
    const unsigned assoc;
    /** The number of sets in the cache. */
    const unsigned numSets;
    /** The extra number of index bits. Fangfei added*/
    const unsigned nebit;

    /** The cache sets. */
    SetType *sets;

    /** The cache blocks. */
    BlkType *blks;
    /** The data blocks, 1 per cache block. */
    uint8_t *dataBlks;

    /** The amount to shift the address to get the set. */
    int setShift;
    /** The amount to shift the address to get the tag. */
    int tagShift;
    /** Mask out all bits that aren't part of the set index. */
    unsigned setMask;
    /** Mask out all bits that aren't part of the block offset. */
    unsigned blkMask;
    /** Mask out all bits that aren't part of di bits Fangfei added */
    unsigned diMask;

public:
    /**
     * Construct and initialize this tag store.
     * @param _numSets The number of sets in the cache.
     * @param _blkSize The number of bytes in a block.
     * @param _assoc The associativity of the cache.
     * @param _hit_latency The latency in cycles for a hit.
     */
    // use parameter to construct newcache
    typedef NEWCACHEParams Params;


    NEWCACHE(const Params *p);

    /**
     * Destructor
     */
    virtual ~NEWCACHE();

    /**
     * Return the block size.
     * @return the block size.
     */
    unsigned
    getBlockSize() const
    {
        return blkSize;
    }

    /**
     * Return the subblock size. In the case of LRU it is always the block
     * size.
     * @return The block size.
     */
    unsigned
    getSubBlockSize() const
    {
        return blkSize;
    }

    /**
     * Invalidate the given block.
     * @param blk The block to invalidate.
     */
    void invalidate(BlkType *blk);

    /**
     * Access block and update replacement data.  May not succeed, in which case
     * NULL pointer is returned.  This has all the implications of a cache
     * access and should only be used as such. Returns the access latency as a side effect.
     * @param addr The address to find.
     * @param asid The address space ID.
     * @param lat The access latency.
     * @return Pointer to the cache block if found.
     */
    BlkType* accessBlock(gem5::PacketPtr pkt, gem5::Cycles &lat, int context_src);

    /**
     * Finds the given address in the cache, do not update replacement data.
     * i.e. This is a no-side-effect find of a block.
     * @param addr The address to find.
     * @param asid The address space ID.
     * @return Pointer to the cache block if found.
     */
    BlkType* findBlock(gem5::PacketPtr pkt) const;


    /**
     * Find a block to evict for the address provided.
     * @param addr The addr to a find a replacement candidate for.
     * @param writebacks List for any writebacks to be performed.
     * @return The candidate block.
     */
    BlkType* findVictim(gem5::PacketPtr pkt, gem5::PacketList &writebacks);

    /**
     * Insert the new block into the cache.  For LRU this means inserting into
     * the MRU position of the set.
     * @param addr The address to update.
     * @param blk The block to update.
     */
    void insertBlock(gem5::PacketPtr pkt, BlkType *blk);

    /**
     * Generate the tag from the given address.
     * @param addr The address to get the tag from.
     * @return The tag of the address.
     */
    unsigned getnebit() const
    {
        return nebit;
    }

    gem5::Addr extractTag(gem5::Addr addr) const
    {
        return (addr >> tagShift);
    }

    /**
     * Calculate the di index from the address.
     * @param addr The address to get the di bits from.
     * @return The di index of the address.
     */
    int extractDI(gem5::Addr addr) const
    {
        return ((addr >> setShift) & diMask);
    }

    /**
     * Calculate the set index from the address.
     * @param addr The address to get the set from.
     * @return The set index of the address.
     */
    int extractSet(gem5::Addr addr) const
    {
        return ((addr >> setShift) & setMask);
    }

    /**
     * Get the block offset from an address.
     * @param addr The address to get the offset of.
     * @return The block offset.
     */
    int extractBlkOffset(gem5::Addr addr) const
    {
        return (addr & blkMask);
    }

    /**
     * Align an address to the block size.
     * @param addr the address to align.
     * @return The block address.
     */
    gem5::Addr blkAlign(gem5::Addr addr) const
    {
        return (addr & ~(gem5::Addr)blkMask);
    }

    /**
     * Regenerate the block address from the tag.
     * @param tag The tag of the block.
     * @param set The set of the block.
     * @return The block address.
     */
    gem5::Addr regenerateBlkAddr(gem5::Addr tag, unsigned set, int lnreg) const
    {
       return ((tag << tagShift) | ((gem5::Addr)lnreg << setShift));
    }

    /**
     * Return the hit latency.
     * @return the hit latency.
     */
    gem5::Cycles getHitLatency() const
    {
        return lookupLatency;
    }
    /**
     *iterated through all blocks and clear all locks
     *Needed to clear all lock tracking at once
     */
    virtual void clearLocks();

    /**
     * Called at end of simulation to complete average block reference stats.
     */
    virtual void cleanupRefs();

    /**
     * Print all tags used
     */
    virtual std::string print() const;

    /**
     * Visit each block in the tag store and apply a visitor to the
     * block.
     *
     * The visitor should be a function (or object that behaves like a
     * function) that takes a cache block reference as its parameter
     * and returns a bool. A visitor can request the traversal to be
     * stopped by returning false, returning true causes it to be
     * called for the next block in the tag store.
     *
     * \param visitor Visitor to call on each block.
     */
    template <typename V>
    void forEachBlk(V &visitor) {
        for (unsigned i = 0; i < numSets * assoc; ++i) {
            if (!visitor(blks[i]))
                return;
        }
    }

};

#endif // __MEM_CACHE_TAGS_NEWCACHE_HH__
