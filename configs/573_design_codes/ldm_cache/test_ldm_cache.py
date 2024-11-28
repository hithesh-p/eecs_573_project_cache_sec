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
    mem_index:int
    tdid:int
    protected:bool

    def __init__(self, mem_index:int=None, tdid:int=None, protected:bool=None):
        self.mem_index = mem_index
        self.tdid = tdid
        self.protected = protected

    def __str__(self):
        return f"{self.mem_index}_{self.tdid}_{self.protected}"

# Init the random, get a new seed
random.seed(0)
for i in range(0, random.randint(0,15)):
    random.seed(random.randint(0, 65535))

# Generate a new seed, use a dummy value for now
def randomizeSeed():
    for i in range(0, random.randint(0,15)):
        random.seed(random.randint(0, 65535))

def print_decoder(decoder):
    print("Key\tValue")
    for key, value in decoder.items():
        # Print with space padding
        print(f"| {(key):<15} | \t{value:<10} |")
        
        
class ldm:
    SIZE:int
    tagArr:list
    dataArr:list
    decoder:dict

    def __init__(self, lineCount:int):
        # Set the number of lines in the decoder (sets total cache size)
        self.SIZE = lineCount

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

        # Initialize lineCount decoder entries
        for i in range(self.SIZE):
            tempDec = decoder_entry(mem_index=(0x10*i), tdid=(0x1*i), protected=False)
            # print(tempDec)
            self.decoder[str(tempDec)] = i
        

        # self.decoder = {
        #     0x10:0, 0x20:1, 0x30:2, 0x40:3,
        #     0x50:4, 0x60:5, 0x70:6, 0x80:7,
        #     0x90:8, 0xA0:9, 0xB0:10, 0xC0:11,
        #     0xD0:12, 0xE0:13, 0xF0:14, 0x00:15
        # }

    # Generate a new seed, use a dummy value for now
    def randomizeSeed(self):
        """
        Randomizes the seed of the random number generator by cycling through multiple seeds

        Parameters:
            N/A
        
        Returns:
            N/A
        """
        
        for i in range(0, random.randint(0,15)):
            random.seed(random.randint(0, 65535))

    # TODO: Function commented out since we don't see a use anywhere.
    # This is kept in case we do happen to need it
    # # Function to take a memory address in, returns index hit/miss and tag hit/miss
    # def dataExists(self, index:int, tag:int):

    #     # Shouldn't this be self.decoder?            
    #     if index in self.decoder:
    #         indexHit = True
    #         if tag == self.tagArr[lc.decoder[index]]:
    #             tagHit = True
    #         else:
    #             tagHit = False
    #     else:
    #         indexHit = False
    #         tagHit = False

    #     return indexHit, tagHit

    # Function to evict given line
    def evictLine(self, dec_line:decoder_entry):
        """
        Evicts a line that matches the exact entry.

        Parameters:
            dec_line: The exact decoder entry to match

        Returns:
            N/A
        """
        # TODO: Do we want this to return a success value?

        # Decoder finds the line to kill, then sends the message out
        lineNum = self.decoder[dec_line]
        self.tagArr[lineNum] = None
        self.dataArr[lineNum] = None
        self.decoder.pop(dec_line)

    # Add data into cache. Assuming one line is empty.
    # spaceFound boolean kept for future use
    def addLine(self, tag:int, dec_data:decoder_entry, data):
        """
        Finds a unused line in the cache and inputs the given data into it.

        Parameters:
            tag: The memory address tag to enter into the tag array
            dec_data: The meory address index, process TDID, and protected flag to enter into the decoder
            data: The cache data to enter into the data array

        Returns:
            N/A
        """
        # TODO: Should this return a success flag?
        # TODO: Should this return an error when the cache is full?
        # TODO: How do we want to handle eviction policy here?

        spaceFound:bool = False
        lineNum = None
        for i in range(len(self.tagArr)):
            if self.tagArr[i] == None:
                spaceFound = True
                lineNum = i
        if spaceFound:
            # Since the dict should be empty here, 
            self.decoder[dec_data] = lineNum
            self.tagArr[lineNum] = tag
            self.dataArr[lineNum] = data
        else:
            pass
            # We didn't put the memory anywhere. We should handle that.

    def evictRandom(self, dec_data:decoder_entry):
        """
        Evicts a random line of cache that matches the TDID of the process triggering this function

        Parameters:
            dec_data: The decoder data passed from the process used to match the TDID

        Returns:
            N/A
        """
        # TODO: Should we have a success flag returned for this?

        # TODO: Modify this to read the tdid and protected flag. Need to integrate once dictionary is changed over

        # Flag for if we found a line with the same TDID to evict
        foundValidLine = None
        # Only cycle through the cache once. This way we won't get stuck here if nothing is found
        for i in range(0, self.SIZE):
            # Pick a random line in the decoder
            randIdx = random.randint(0, len(self.decoder))
            # Get the key from that line
            decoderLine = list(self.decoder.keys())[randIdx]
            # Check if the key TDID matches the triggering process' TDID
            if decoderLine.tdid == dec_data.tdid:
                # If TDID matches, then get the tag index to wipe the line
                foundValidLine = self.decoder[randIdx]
                # Break out of the loop
                break
        # If we have a line to eliminate, then evict it
        if foundValidLine != None:
            self.evictLine(foundValidLine)
        # NOTE: This is here just in case something goes wrong later
        # # We don't need to check for blanks here, the dict only has filled in values
        # randIdx = random.randint(0, len(self.decoder))
        # clearedIndex = list(self.decoder.keys())[randIdx]
        # self.evictLine(clearedIndex)

    def fetchLine(self, index:int, tag:int, tdid:int):
        """
        Fetches a cache line base on the address index provided.
        
        Parameters:
            index: The memory address index to search the decoder for
            tag: The memory address tag to compare in the tag array
            tdid: The process TDID that is requesting the datas
        Returns:
            data: The cache line found. If no line found, then None is returned
        """

        # Check if the index is in the decoder
        if index in self.decoder:
            # Index hit, check tags
            pass
            if tag == self.tagArr[self.decoder[index]]:
                # Tag hit
                return self.dataArr[self.decoder[index]]
            else:
                # Tag miss
                self.evictLine(index)
                return None
        else:
            # Index miss
            self.evictRandom(tdid)
            return None



lc = ldm(16)
lc.test_resetCache()

# Tests are in if statements to disable them as needed.
# Just set them to True / False as needed

# if False:
#     # Test the data exists function
#     print("START: Data exists")
#     print(lc.dataExists(0xA0, 0xA00)) # Both hit
#     print(lc.dataExists(0xF8, 0xFF8)) # Both miss
#     print(lc.dataExists(0xD0, 0xB00)) # Index hits, not tag
#     print("END: Data exists")

if True:
    # Testing the eviction process
    print("START: Evict line")
    lc.test_resetCache()
    print_decoder(lc.decoder)
    lc.evictLine(str(decoder_entry(0xA0, 0xA, False)))
    print_decoder(lc.decoder)
    lc.evictLine(str(decoder_entry(0xF0, 0xF, False)))
    print_decoder(lc.decoder)
    lc.evictLine(str(decoder_entry(0x20, 0x2, False)))
    print_decoder(lc.decoder)
    print("END: Evict line")

if False:
    # Testing the add line. Start with full cache, evict a few lines before filling again
    print("START: Add line")
    lc.test_resetCache()
    print("reset cache")
    print(lc.decoder)
    print(lc.dataArr)
    # Evict three lines
    lc.evictLine(str(decoder_entry(0xA0, 0xA, False)))
    lc.evictLine(str(decoder_entry(0x10, 0x1, False)))
    lc.evictLine(str(decoder_entry(0xF0, 0xF, False)))
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

if False:
    # Evicting a random line from the cache
    print("START: Evict random")
    lc.test_resetCache()
    print("full cache")
    print_decoder(lc.decoder)
    # print(lc.decoder)
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