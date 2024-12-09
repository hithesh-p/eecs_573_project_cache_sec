/**
 * Newccache.hh based on original version written by Erik Hallnor.
 * This version is updated to utilize python 3 along with the newer release of gem5.
 */

#ifndef __MEM_CACHE_TAGS_NEWCACHE_HH__
#define __MEM_CACHE_TAGS_NEWCACHE_HH__

#include "mem/cache/tags/base.hh"
#include "mem/cache/tags/cacheset.hh"
#include "params/NewCache.hh"

namespace gem5
{
    class BaseCache;

    // template <class TagStore>
    class NewCache : public BaseTags
    {
        public:
            /** Typedef the block type used in this tag store */
            typedef CacheBlk BlkType;
            /** Typedef for a list of pointers to the local block class */
            typedef std::list<BlkType*> BlkList;
            typedef CacheSet<CacheBlk> SetType;
        protected:
            /** Associativity of the cache */
            const unsigned assoc;
            /** Number of sets in cache */
            const unsigned numSets;
            /** Extra number of index bits */
            const unsigned neBit;

            /** The Cache sets */
            SetType *sets;
            /** The cache blocks */
            BlkType *blks;
            /** The data blocks (1 per cache block) */
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
             * @param numSets The number of sets in the cache.
             * @param blkSize The number of bytes in a block.
             * @param assoc The associativity of the cache.
             * @param hit_latency The latency in cycles for a hit.
             */
            // use parameter to construct newcache
            typedef NewCacheParams Params;

            NewCache(const Params &p);

            // Destructor      
            virtual ~NewCache() {};

            /**
             * Return the block size.
             * @return the block size.
             */
            unsigned getBlockSize() const
            {
                return blkSize;
            }

            /**
             * Return the subblock size.
             * In the case of LRU it is always the block size.
             * @return The block size.
             */
            unsigned getSubBlockSize() const
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
             * @param pkt The address to find.
             * @param lat The access latency.
             * @param context_src The address space ID.
             * @return Pointer to the cache block if found.
             */
            BlkType* accessBlock(PacketPtr pkt, Cycles &lat, int context_src);

            /**
             * Finds the given address in the cache, do not update replacement data.
             * i.e. This is a no-side-effect find of a block.
             * PACKET CONTAINS BOTH PARAMETERS
             * @param addr The address to find.
             * @param asid The address space ID.
             * @return Pointer to the cache block if found.
             */
            BlkType* findBlock(PacketPtr pkt) const;

            /**
             * Find a block to evict for the address provided.
             * @param pkt The addr to a find a replacement candidate for.
             * @param writebacks List for any writebacks to be performed.
             * @return The candidate block.
             */
            BlkType* findVictim(PacketPtr pkt, PacketList &writebacks);


            /**
             * Insert the new block into the cache.  For LRU this means inserting into
             * the MRU position of the set.
             * @param pkt The address to update.
             * @param blk The block to update.
             */
            void insertBlock(PacketPtr pkt, BlkType *blk);

            /**
             * Generate the tag from the given address.
             * @param addr The address to get the tag from.
             * @return The tag of the address.
             */
            Addr extractTag(Addr addr) const
            {
                return (addr >> tagShift);
            }

            /**
             * Calculate the di index from the address.
             * @param addr The address to get the di bits from.
             * @return The di index of the address.
             */
            int extractDI(Addr addr) const
            {
                return ((addr >> setShift) & diMask);
            }

            /**
             * Calculate the set index from the address.
             * @param addr The address to get the set from.
             * @return The set index of the address.
             */
            int extractSet(Addr addr) const
            {
                return ((addr >> setShift) & setMask);
            }

            /**
             * Get the block offset from an address.
             * @param addr The address to get the offset of.
             * @return The block offset.
             */
            int extractBlkOffset(Addr addr) const
            {
                return (addr & blkMask);
            }

            /**
             * Align an address to the block size.
             * @param addr the address to align.
             * @return The block address.
             */
            Addr blkAlign(Addr addr) const
            {
                return (addr & ~(Addr)blkMask);
            }

            /**
             * Regenerate the block address from the tag.
             * @param tag The tag of the block.
             * @param set The set of the block.
             * @param lnreg The di index of the address
             * @return The block address.
             */
            Addr regenerateBlkAddr(Addr tag, unsigned set, int lnreg) const
            {
            return ((tag << tagShift) | ((Addr)lnreg << setShift));
            }

            /**
             * Return the hit latency.
             * @return the hit latency.
             */
            Cycles getHitLatency() const
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
} // namespace gem5

# endif // __MEM_CACHE_TAGS_NEWCACHE_HH__