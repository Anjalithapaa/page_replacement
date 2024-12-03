#include <cstdio>
#include <limits>
#include <climits>

// Page Replacement Simulation Program
// Implements three page replacement algorithms:
// 1. FIFO (First-In-First-Out)
// 2. LRU (Least Recently Used)
// 3. Optimal (Belady's Algorithm)

// Constant definitions
#define PAGE_SIZE 100 // Size of each page
#define MAX_FRAMES 5  // Number of memory frames available

// Algorithm mode identifiers
#define FIFO 1 // First-In-First-Out replacement
#define LRU 2  // Least Recently Used replacement
#define OPT 3  // Optimal replacement algorithm

// Structure to represent page data in memory frames
typedef struct
{
    int pageNum;   // Page number stored in this frame
    int usage;     // Number of times page has been used
    int lastUsed;  // Index when page was last accessed
    int timeStamp; // Time of page insertion into frame
} pageData;

// Global arrays to store address stream and memory frames
int adds[100];               // Array to store memory address stream
pageData frames[MAX_FRAMES]; // Array representing memory frames

// Function prototypes
int readAddressStream(const char *filename);    // Read addresses from input file
void showAdds(int numAdds);                     // Display address stream
int pageReplace(int numAdds, char mode);        // Main page replacement simulation
int searchFrameTable(int pageNum, int nFrames); // Check if page exists in frames
void showFrameTable(int nFrames);               // Display current frame contents

// Replacement strategy selector functions
int getIndexOfOldestPage(int nFrames);                   // FIFO: Find oldest page in frames
int getIndexOfLRUPage(int nFrames);                      // LRU: Find least recently used page
int getIndexOfBeladyPage(int nFrames, int currentIndex); // Optimal: Find page to replace

int main()
{
    int numAdds;    // Total number of addresses
    int pageFaults; // Page fault counter

    // User greeting
    printf("YO HO HO !\n");

    // Read address stream from file
    const char *filename = "address.txt";
    numAdds = readAddressStream(filename);
    printf("Total Addresses Read: %d \n", numAdds);

    // Display input address stream
    showAdds(numAdds);

    // Simulate FIFO Page Replacement
    printf("\n--- FIFO Page Replacement ---\n");
    pageFaults = pageReplace(numAdds, FIFO);
    printf("FIFO Page Faults: %d \n", pageFaults);

    // Simulate LRU Page Replacement
    printf("\n--- LRU Page Replacement ---\n");
    pageFaults = pageReplace(numAdds, LRU);
    printf("LRU Page Faults: %d \n", pageFaults);

    // Simulate Optimal Page Replacement
    printf("\n--- Optimal Page Replacement ---\n");
    pageFaults = pageReplace(numAdds, OPT);
    printf("Optimal Page Faults: %d \n", pageFaults);

    return 0;
}

// Read memory addresses from input file
int readAddressStream(const char *filename)
{
    FILE *in;
    int address;
    int j = 0;

    // Open file with error checking
    in = fopen(filename, "r");
    if (in == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }

    // Read addresses into global array
    while (fscanf(in, "%d", &address) != EOF)
    {
        adds[j++] = address;
    }
    fclose(in);
    return j;
}

// Display all memory addresses read from input file
void showAdds(int numAdds)
{
    printf("Address Stream: \n");
    for (int j = 0; j < numAdds; j++)
    {
        printf("%d \n", adds[j]);
    }
}

// Search if page exists in memory frames
int searchFrameTable(int pageNum, int nFrames)
{
    // Linear search through frames
    for (int j = 0; j < nFrames; j++)
    {
        if (frames[j].pageNum == pageNum)
        {
            return j; // Page found, return frame index
        }
    }
    return -1; // Page not found
}

// FIFO: Find frame with oldest timestamp
int getIndexOfOldestPage(int nFrames)
{
    int oldIndex = 0;
    for (int j = 1; j < nFrames; j++)
    {
        // Compare timestamps to find earliest inserted page
        if (frames[j].timeStamp < frames[oldIndex].timeStamp)
        {
            oldIndex = j;
        }
    }
    return oldIndex;
}

// LRU: Find page with least usage
int getIndexOfLRUPage(int nFrames)
{
    int leastRU = frames[0].usage;
    int lIndex = 0;

    // Find page with minimum usage count
    for (int j = 1; j < nFrames; j++)
    {
        if (frames[j].usage < leastRU)
        {
            leastRU = frames[j].usage;
            lIndex = j;
        }
    }
    return lIndex;
}

// Optimal (Belady's) Algorithm: Predict future page usage
int getIndexOfBeladyPage(int nFrames, int currentIndex)
{
    int maxFutureDistance = -1;
    int replaceIndex = 0;

    // For each frame in memory
    for (int j = 0; j < nFrames; j++)
    {
        int futureDistance = INT_MAX;

        // Look ahead to find next occurrence of this page
        for (int k = currentIndex + 1; k < 100; k++)
        {
            if (adds[k] / PAGE_SIZE == frames[j].pageNum)
            {
                futureDistance = k - currentIndex;
                break;
            }
        }

        // If page won't be used again, immediately replace it
        if (futureDistance == INT_MAX)
        {
            return j;
        }

        // Find page that will be used furthest in the future
        if (futureDistance > maxFutureDistance)
        {
            maxFutureDistance = futureDistance;
            replaceIndex = j;
        }
    }

    return replaceIndex;
}

// Main page replacement simulation
int pageReplace(int numAdds, char mode)
{
    int nFrames = 0;     // Current number of frames in use
    int pageFaults = 0;  // Counter for page faults
    int timeCounter = 0; // Global timestamp for page insertion

    // Process each memory address
    for (int j = 0; j < numAdds; j++)
    {
        // Calculate page number and offset
        int pageNum = adds[j] / PAGE_SIZE;
        int offset = adds[j] % PAGE_SIZE;

        // Check if page already in memory
        int frameNum = searchFrameTable(pageNum, nFrames);

        // Page not in memory (Page Fault)
        if (frameNum == -1)
        {
            // Room available in frames
            if (nFrames < MAX_FRAMES)
            {
                frames[nFrames].pageNum = pageNum;
                frames[nFrames].usage = 1;
                frames[nFrames].lastUsed = j;
                frames[nFrames].timeStamp = timeCounter++;
                nFrames++;
                pageFaults++;
            }
            // No room, need page replacement
            else
            {
                // Select replacement strategy
                int repFrame;
                switch (mode)
                {
                case FIFO:
                    repFrame = getIndexOfOldestPage(MAX_FRAMES);
                    break;
                case LRU:
                    repFrame = getIndexOfLRUPage(MAX_FRAMES);
                    break;
                case OPT:
                    repFrame = getIndexOfBeladyPage(MAX_FRAMES, j);
                    break;
                }

                // Replace selected frame
                frames[repFrame].pageNum = pageNum;
                frames[repFrame].usage = 1;
                frames[repFrame].lastUsed = j;
                frames[repFrame].timeStamp = timeCounter++;

                pageFaults++;
            }
        }
        // Page already in memory
        else
        {
            // Update usage statistics
            frames[frameNum].usage++;
            frames[frameNum].lastUsed = j;
        }

        // Display current frame state
        showFrameTable(nFrames);
    }
    return pageFaults;
}

// Display current memory frame contents
void showFrameTable(int nFrames)
{
    printf("Frame Table - ");
    for (int j = 0; j < MAX_FRAMES; j++)
    {
        if (j < nFrames)
        {
            printf("%d ", frames[j].pageNum);
        }
        else
        {
            printf("# "); // Unused frames
        }
    }
    printf("\n");
}