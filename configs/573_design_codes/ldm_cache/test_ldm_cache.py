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
import operator
import random
import re
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
    print("--------------------------------------")
    print(f"|      Number of filled lines: {len(decoder)}    |")
    print("--------------------------------------")
    print("Key\tValue")
    for key, value in decoder.items():
        # Print with space padding
        print(f"| {(key):<15} | \t{value:<10} |")

def getDecoderMemIndex(inputLine:str):
    """
    Gets the memory index from the decoder line string
    
    Parameters:
        inputLine: The strign to read the memory index from
        
    Returns:
        memIndex: The memory index of the dict key as an int
    """

    return int(re.match("(^\d+)_\d+_\w+$", inputLine).group(1))

def getDecoderTDID(inputLine:str):
    """
    Gets the TDID from the decoder line string
    
    Parameters:
        inputLine: The string to read the TDID from
        
    Returns:
        tdid: The TDID of the dict key as an int
    """

    return int(re.match("^\d+_(\d+)_\w+$", inputLine).group(1))

def getDecoderProtected(inputLine:str):
    """
    Gets the protected flag status from the decoder line string
    
    Parameters:
        inputLine: The string to read the protected flag from
        
    Returns:
        protected: The protected flag status of the dict key as a boolean
    """

    return bool(re.match("^\d+_\d+_(\w+$)", inputLine).group(1))
        
        
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
        print("Initializing cache")
        self.tagArr = [
            0x100, 0x200, 0x300, 0x400,
            0x500, 0x600, 0x700, 0x800,
            0x900, 0xA00, 0xB00, 0xC00,
            0xD00, 0xE00, 0xF00, 0x000
        ]

        # Initialize the data array with some dummy data in the format "MEM[0x{random hexadecimal number that is different for each entry}]"
        # Randomly initialize an array of 16 entries with random hexadecimal numbers

        self.dataArr = [f"MEM[0x{random.randint(0, 0xFF):X}]" for _ in range(self.SIZE)]

        # Initialize lineCount decoder entries
        for i in range(self.SIZE):
            tempDec = decoder_entry(mem_index=(0x10*i), tdid=(i%2), protected=False)
            # print(tempDec)
            self.decoder[str(tempDec)] = i
        
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
        lineNum = self.decoder[str(dec_line)]
        self.tagArr[lineNum] = None
        self.dataArr[lineNum] = None
        self.decoder.pop(str(dec_line))

    def replaceLine(self, dec_data:decoder_entry):
        """
        Replaces a line that matches the exact entry.
        """
        print(">>>> Replacing line with some line from cache")
        # self.decoder[str(dec_data)] = f"MEM[0x{random.randint(0, 0xFFF):X}]"
        self.decoder[str(dec_data)] = random.randint(1, self.SIZE-1)

        # self.addLine(self.tagArr[self.decoder[str(dec_data)]] + random.randint(0,10), dec_data,f"dummy{random.randint(0,100)}")
          

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
        attemptCounter:int = 0
        while spaceFound == False:
            attemptCounter = attemptCounter + 1
            if attemptCounter > 5:
                # Just kills a random line, it's obvious we're not matching a random eviction
                # TODO: This should not be in the final version
                self.evictFullRandom()
            for i in range(len(self.tagArr)):
                if self.tagArr[i] == None:
                    spaceFound = True
                    lineNum = i
            if spaceFound:
                # Since the dict should be empty here, just add the line in
                self.decoder[str(dec_data)] = lineNum
                self.tagArr[lineNum] = tag
                self.dataArr[lineNum] = data
            else:
                # If we can't find space, we'll need to clear some out.
                # TODO: If the given TDID doesn't have any other lines, then we're stuck in an infinite loop
                self.evictRandom(dec_data)

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
            randIdx = random.randint(0, len(self.decoder)-1)
            # Get the key from that line
            decoderLine = list(self.decoder.keys())[randIdx]
            # Check if the key TDID matches the triggering process' TDID
            if getDecoderTDID(decoderLine) == dec_data.tdid:
                # If TDID matches, then get the tag index to wipe the line
                foundValidLine = self.decoder[decoderLine]
                # Break out of the loop
                break
        # If we have a line to eliminate, then evict it
        if foundValidLine != None:
            print(f">>>> Randomly evicted {decoderLine}")
            self.evictLine(decoderLine)
        # NOTE: This is here just in case something goes wrong later

    def evictFullRandom(self):
        """
        Just evicts a completely random line from the cache. Does not matter what it is
        
        Parameters:
            N/A
            
        Returns:
            N/A
        """
        randIdx = random.randint(0, len(self.decoder)-1)
        decoderLine = list(self.decoder.keys())[randIdx]
        self.evictLine(decoderLine)

    def fetchLine(self, decoderLine:decoder_entry, tag:int):
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
        if str(decoderLine) in self.decoder:
            # Index hit, check tags
            pass
            if tag == self.tagArr[self.decoder[str(decoderLine)]]:
                # Tag hit
                return self.dataArr[self.decoder[str(decoderLine)]]
            else:
                # Tag miss
                print(f">> Tag miss! Replacing line associated with {decoderLine}")                    
                self.replaceLine(decoderLine)
                return None
        else:
            # Index miss
            print(f">> Index miss on {decoderLine}! Evicting random line...")                    
            self.evictRandom(decoderLine)
            return None
        
    def randomizeOnce(self):
        """
        Randomizes the cache once, uses current random seed.
        
        Parameters:
            N/A
            
        Returns:
            N/A
        """

        # Create a list of random entries to fill with data
        # This will have enough for a full cache, we will only pull enough for the current data
        idxList = random.sample(range(self.SIZE), self.SIZE)

        self.dataArr = list(list(zip(*sorted(zip(idxList, self.dataArr), key=operator.itemgetter(0))))[1])
        self.tagArr = list(list(zip(*sorted(zip(idxList, self.tagArr), key =operator.itemgetter(0))))[1])

        for idx, key in enumerate(list(self.decoder.keys())):
            self.decoder[key] = idxList[idx]

    def randomizeMultiple(self):
        """
        Randomizes the cache multiple times. The number of times randomized is also randomized.
        After each randomization the seed is changed to make it harder to follow
        
        Parameters:
            N/A
            
        Returns:
            N/A
        """

        for i in range(0, random.randint(1, 8)):
            self.randomizeOnce()
            # TODO: Do we want this in here? Is this overkill / latency inducing?
            # self.randomizeSeed()


# Create a new cache object
lc = ldm(16)

if False:
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
    print_decoder(lc.decoder)
    # print(lc.dataArr)
    # Evict three lines
    lc.evictLine(str(decoder_entry(0xA0, 0xA, False)))
    lc.evictLine(str(decoder_entry(0x10, 0x1, False)))
    lc.evictLine(str(decoder_entry(0xF0, 0xF, False)))
    print("evict lines")
    print_decoder(lc.decoder)
    # print(lc.dataArr)
    # Add two lines
    lc.addLine(0x180, decoder_entry(0xF8, 0xA, False), "dummy51")
    lc.addLine(0xFF8, decoder_entry(0xFF, 0xB, False), "dummy52")
    print("add lines")
    print_decoder(lc.decoder)
    # print(lc.dataArr)
    print("END: Add line")

if False:
    # Try to flood the cache with a new process, there should only ever be 16 entires in here.
    print("START: Flood cache (new processes)")
    lc.test_resetCache()
    print("Full cache")
    print_decoder(lc.decoder)
    for i in range(0, 20):
        lc.addLine(tag=0xBB, dec_data=decoder_entry(0x02*i, 0x02, True), data="dummy126.5")
    print("First 20 items passed in (TDID = 2)")
    print_decoder(lc.decoder)
    for i in range(0, 20):
        lc.addLine(tag=0x11, dec_data=decoder_entry(0x04*i, 0x06, True), data="dummy723.87")
    print("Second 20 items passed in (TDID = 6)")
    print_decoder(lc.decoder)
    print("END: Flood cache (new processes)")

if False:
    # Try to flood the cache, there should only ever be 16 entires in here.
    print("START: Flood cache (same processes)")
    lc.test_resetCache()
    print("Full cache")
    print_decoder(lc.decoder)
    for i in range(0, 20):
        lc.addLine(tag=0xBB, dec_data=decoder_entry(0x02*i, 0x00, True), data="dummy126.5")
    print("First 20 items passed in (TDID = 0)")
    print_decoder(lc.decoder)
    for i in range(0, 20):
        lc.addLine(tag=0x11, dec_data=decoder_entry(0x04*i, 0x01, True), data="dummy723.87")
    print("Second 20 items passed in (TDID = 1)")
    print_decoder(lc.decoder)
    print("END: Flood cache (same processes)")

if False:
    # Evicting a random line from the cache
    print("START: Evict random")
    lc.test_resetCache()
    print("full cache")
    print_decoder(lc.decoder)
    # print(lc.decoder)
    # print(lc.tagArr)
    # Evict three items
    # TODO: The TDID will need to change when ready
    lc.evictRandom(decoder_entry(0x00, 0xA, False))
    lc.evictRandom(decoder_entry(0x00, 0xB, False))
    lc.evictRandom(decoder_entry(0x00, 0xD, False))
    print("modified cache")
    print_decoder(lc.decoder)
    # print(lc.tagArr)
    print("END: Evict random")

if True: 
    # Test the NewCache Functionality
    """
    Series of cache accesses to test the behaviour upon index hit, index miss, tag hit and tag miss
    """
    lc.test_resetCache()
    print_decoder(lc.decoder)
    print(f"Load: {lc.fetchLine(decoder_entry(0, 0, False), 0x100)}")
    print(f"Load: {lc.fetchLine(decoder_entry(96, 0, False), 0x900)}") # Tag Miss
    print(f"Load: {lc.fetchLine(decoder_entry(96, 1, False), 0x900)}")
    print_decoder(lc.decoder)
    print(f"Load: {lc.fetchLine(decoder_entry(160, 0, False), 0xB00)}")
    
    # print("START: Fetch line (bad)")
    print_decoder(lc.decoder)
    lc.fetchLine(decoder_entry(0, 0, True), 0x100) # Protected miss
    lc.fetchLine(decoder_entry(176, 10, False), 0xA00) # TDID miss
    lc.fetchLine(decoder_entry(200, 5, False), 0x500) # Index miss
    lc.fetchLine(decoder_entry(240, 15, False), 0xD00) # Tag miss
    print_decoder(lc.decoder)
    print("END: Fetch line")

if False:
    # Randomize the cache once, see what happens
    print("START: Randomize cache once")
    lc.test_resetCache()
    print("New cache")
    print_decoder(lc.decoder)
    lc.randomizeOnce()
    print("Randomize once")
    print_decoder(lc.decoder)
    print("Remove some items in the cache")
    lc.test_resetCache()
    for i in range(0, 4):
        lc.evictFullRandom()
    print_decoder(lc.decoder)
    print("Randomize it")
    lc.randomizeOnce()
    print_decoder(lc.decoder)
    print("END: Randomize cache once")

if False:
    # Randomize the cache multiple times in a row
    print("START: Randomize cache multiple times")
    lc.test_resetCache()
    print("New cache")
    print_decoder(lc.decoder)
    lc.randomizeMultiple()
    print("Randomized cache")
    print_decoder(lc.decoder)
    print("Remove some items in the cache")
    lc.test_resetCache()
    for i in range(0, 4):
        lc.evictFullRandom()
    print_decoder(lc.decoder)
    print("Randomize it")
    lc.randomizeOnce()
    print_decoder(lc.decoder)
    print("END: Randomize cache multiple times")