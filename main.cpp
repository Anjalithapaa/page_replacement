#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <fstream>

// Function to load addresses from the file
std::vector<int> loadAddresses(const std::string &filename)
{
    std::vector<int> addresses;
    std::ifstream file(filename); // Open the file for reading
    int address;
    while (file >> address)
    {                                 // Read each address from the file
        addresses.push_back(address); // Store the address in the vector
    }
    return addresses;
}

// FIFO (First In First Out) Page Replacement Algorithm
int fifo(const std::vector<int> &addresses, int frameCount)
{
    std::queue<int> frames;                 // Queue to maintain the order of pages in memory
    std::unordered_map<int, bool> inMemory; // Map to track which pages are currently in memory
    int pageFaults = 0;                     // Counter for the number of page faults

    for (int address : addresses)
    { // Loop through each address in the input
        if (!inMemory[address])
        { // Check if the page is not in memory
            pageFaults++;
            if (frames.size() == frameCount)
            {                                 // If all frames are full
                int toEvict = frames.front(); // Identify the oldest page
                frames.pop();                 // Remove it from the queue
                inMemory.erase(toEvict);      // Mark it as no longer in memory
            }
            frames.push(address);     // Add the new page to the memory queue
            inMemory[address] = true; // Mark it as in memory
        }
    }
    return pageFaults;
}

// LRU (Least Recently Used) Page Replacement Algorithm
int lru(const std::vector<int> &addresses, int frameCount)
{
    std::unordered_map<int, int> lastUsed; // Map to track the last used index of each page
    std::vector<int> frames;               // Vector to store pages in memory
    int pageFaults = 0;                    // Counter for the number of page faults

    for (int i = 0; i < addresses.size(); i++)
    { // Loop through each address
        int address = addresses[i];
        if (std::find(frames.begin(), frames.end(), address) == frames.end())
        {
            // If the page is not in memory
            pageFaults++; // Increment page fault count
            if (frames.size() == frameCount)
            {                     // If memory is full
                int lruIndex = 0; // Initialize index of the least recently used page
                for (int j = 1; j < frames.size(); j++)
                { // Find the LRU page
                    if (lastUsed[frames[j]] < lastUsed[frames[lruIndex]])
                    {
                        lruIndex = j; // Update index of the least recently used page
                    }
                }
                frames.erase(frames.begin() + lruIndex); // Remove the LRU page from memory
            }
            frames.push_back(address); // Add the new page to memory
        }
        lastUsed[address] = i; // Update the last used index for the page
    }
    return pageFaults;
}

// Function to analyze Belady's Anomaly
void beladyAnalysis(const std::vector<int> &addresses)
{
    std::cout << "\nBelady's Anomaly Analysis:\n";
    for (int frames = 1; frames <= 10; frames++)
    {                                         // Loop through different frame sizes
        int faults = fifo(addresses, frames); // Use FIFO to calculate page faults
        std::cout << "Frames: " << frames << ", Page Faults: " << faults << std::endl;
    }
}

int main()
{
    // Load addresses from the input file
    std::string filename = "address.txt";
    std::vector<int> addresses = loadAddresses(filename); // Load addresses into a vector

    if (addresses.empty())
    { // Check if the file was empty or not found
        std::cerr << "Error: Address file is empty or not found.\n";
        return 1;
    }

    // Ask the user for the number of frames
    int frameCount;
    std::cout << "Enter the number of frames: ";
    std::cin >> frameCount;

    if (frameCount <= 0)
    { // Validate the frame count
        std::cerr << "Error: Frame count must be greater than 0.\n";
        return 1;
    }

    // Run FIFO algorithm
    int fifoFaults = fifo(addresses, frameCount);
    std::cout << "\nFIFO Page Faults: " << fifoFaults << std::endl;

    // Run LRU algorithm
    int lruFaults = lru(addresses, frameCount);
    std::cout << "LRU Page Faults: " << lruFaults << std::endl;

    // Perform Belady's Analysis
    beladyAnalysis(addresses);

    return 0;
}