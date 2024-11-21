# from m5.objects import Cache

# from dataclasses import dataclass

# @dataclass
# class decoder_entry:
#     tdid:str
#     protected:bool
#     index:str

# """

# """

# Start here, we'll implement the cache stuff later. This is just using lists and dictionaries.
import random
from dataclasses import dataclass

# Since these are going to be in a dictionary, the index is not needed since that is the other part of the key-value pair
@dataclass
class decoder_entry:
    tdid:str
    protected:bool
    tag_index:int

# Init the random, get a new seed
random.seed(0)
for i in range(0, random.randint(0,15)):
    random.seed(random.randint(0, 65535))

# Generate a new seed, use a dummy value for now
def randomizeSeed():
    for i in range(0, random.randint(0,15)):
        random.seed(random.randint(0, 65535))

class ldm:
    SIZE:int
    tagArr:list
    dataArr:list
    decoder:dict

    def __init__(self, lineSize):
        # Set the number of lines in the decoder (sets total cache size)
        self.SIZE = lineSize

        # Inits all of these to be blank
        self.tagArr = []
        self.dataArr = []
        self.decoder = {}

        # Start with a random seed
        random.seed(0)
        for i in range(0, random.randint(0,15)):
            random.seed(random.randint(0, 65535))

    def test_resetCache(self):
        # Put some dummy data into the cache lines. Size = 16
        self.tagArr = [
            0x100, 0x200, 0x300, 0x400,
            0x500, 0x600, 0x700, 0x800,
            0x900, 0xA00, 0xB00, 0xC00,
            0xD00, 0xE00, 0xF00, 0x000
        ]

        self.dataArr = [
            "dummy1", "dummy2", "dummy3", "dummy4",
            "dummy5", "dummy6", "dummy7", "dummy8",
            "dummy9", "dummy10", "dummy11", "dummy12",
            "dummy13", "dummy14", "dummy15", "dummy16"
        ]

        self.decoder = {
            0x10:0, 0x20:1, 0x30:2, 0x40:3,
            0x50:4, 0x60:5, 0x70:6, 0x80:7,
            0x90:8, 0xA0:9, 0xB0:10, 0xC0:11,
            0xD0:12, 0xE0:13, 0xF0:14, 0x00:15
        }

    # Generate a new seed, use a dummy value for now
    def randomizeSeed(self):
        for i in range(0, random.randint(0,15)):
            random.seed(random.randint(0, 65535))

    # Function to take a memory address in, returns index hit/miss and tag hit/miss
    def dataExists(self, index:int, tag:int):
            
        if index in lc.decoder:
            indexHit = True
            if tag == lc.tagArr[lc.decoder[index]]:
                tagHit = True
            else:
                tagHit = False
        else:
            indexHit = False
            tagHit = False

        return indexHit, tagHit

    # Function to evict given line
    def evictLine(self, index:int):
        # Decoder finds the line to kill, then sends the message out
        lineNum = lc.decoder[index]
        lc.tagArr[lineNum] = None
        lc.dataArr[lineNum] = None
        lc.decoder.pop(index)

    # Add data into cache. Assuming one line is empty.
    # spaceFound boolean kept for future use
    def addLine(self, tag:int, index:int, data):
        spaceFound:bool = False
        lineNum = None
        for i in range(len(lc.tagArr)):
            if lc.tagArr[i] == None:
                spaceFound = True
                lineNum = i
        if spaceFound:
            # Since the dict should be empty here, 
            lc.decoder[index] = lineNum
            lc.tagArr[lineNum] = tag
            lc.dataArr[lineNum] = data
        else:
            pass
            # We didn't put the memory anywhere. We should handle that.

    def evictRandom(self, tdid):
        # TODO: Modify this to read the tdid and protected flag. Need to integrate once dictionary is changed over

        # We don't need to check for blanks here, the dict only has filled in values
        randIdx = random.randint(0, len(self.decoder))
        clearedIndex = list(self.decoder.keys())[randIdx]
        self.evictLine(clearedIndex)


lc = ldm(16)
lc.test_resetCache()

# Tests are in if statements to disable them as needed.
# Just set them to True / False as needed

if False:
    # Test the data exists function
    print("START: Data exists")
    print(lc.dataExists(0xA0, 0xA00)) # Both hit
    print(lc.dataExists(0xF8, 0xFF8)) # Both miss
    print(lc.dataExists(0xD0, 0xB00)) # Index hits, not tag
    print("END: Data exists")

if False:
    # Testing the eviction process
    print("START: Evict line")
    lc.test_resetCache()
    print(lc.decoder)
    lc.evictLine(0xA0)
    print(lc.decoder)
    lc.evictLine(0xF0)
    print(lc.decoder)
    lc.evictLine(0x20)
    print(lc.decoder)
    print("END: Evict line")

if False:
    # Testing the add line. Start with full cache, evict a few lines before filling again
    print("START: Add line")
    lc.test_resetCache()
    print("reset cache")
    print(lc.decoder)
    print(lc.dataArr)
    # Evict three lines
    lc.evictLine(0xA0)
    lc.evictLine(0x10)
    lc.evictLine(0xF0)
    print("evict lines")
    print(lc.decoder)
    print(lc.dataArr)
    # Add two lines
    lc.addLine(0x180, 0xF8, "dummy51")
    lc.addLine(0xFF8, 0xFF, "dummy52")
    print("add lines")
    print(lc.decoder)
    print(lc.dataArr)
    print("END: Add line")

if True:
    # Evicting a random line from the cache
    print("START: Evict random")
    lc.test_resetCache()
    print("full cache")
    print(lc.decoder)
    print(lc.tagArr)
    # Evict three items
    # TODO: The TDID will need to change when ready
    lc.evictRandom(0x000)
    lc.evictRandom(0x000)
    lc.evictRandom(0x000)
    print("modified cache")
    print(lc.decoder)
    print(lc.tagArr)
    print("END: Evict random")