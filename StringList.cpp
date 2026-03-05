#include "StringList.h"

// ======================= DEFAULT CONSTRUCTOR =======================

// Initialize empty StringList with no allocated memory
StringList::StringList() : data_(nullptr), size_(0), capacity_(0) {}

// ======================= DESTRUCTOR =======================

// Clean up dynamically allocated array of strings
StringList::~StringList()
{
    // Step 1: Delete dynamically allocated string array
    // Note: std::string destructors called automatically for each element
    delete[] data_;
}

// ======================= COPY CONSTRUCTOR =======================

// Create new StringList as deep copy of another StringList
// Each string is copied, not referenced (full independence)
StringList::StringList(const StringList& other) : data_(nullptr), size_(0), capacity_(0)
{
    // Step 1: Check if source list has any elements
    if (other.size_ > 0)
    {
        // Step 2: Set capacity to match source (avoids extra reallocations)
        capacity_ = other.capacity_;

        // Step 3: Allocate new array with same capacity
        data_ = new std::string[capacity_];

        // Step 4: Copy size from source
        size_ = other.size_;

        // Step 5: Loop through each element in source list
        for (std::size_t i = 0; i < size_; ++i)
        {
            // Step 5a: Copy each string from source to new list
            // std::string copy constructor handles deep copy of string content
            data_[i] = other.data_[i];
        }
    }
    // Step 6: If source is empty, new list remains empty with nullptr data
}

// ======================= ASSIGNMENT OPERATOR =======================

// Replace current list contents with copy of another list
// Properly handles self-assignment (a = a) to avoid data corruption
StringList& StringList::operator=(const StringList& other)
{
    // Step 1: Check for self-assignment (a = a case)
    if (this == &other) return *this;

    // Step 2: Delete existing data to prevent memory leak
    delete[] data_;

    // Step 3: Reset pointers and counters to empty state
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;

    // Step 4: Check if source list has any elements
    if (other.size_ > 0)
    {
        // Step 5: Set capacity to match source
        capacity_ = other.capacity_;

        // Step 6: Allocate new array with same capacity
        data_ = new std::string[capacity_];

        // Step 7: Copy size from source
        size_ = other.size_;

        // Step 8: Loop through each element in source list
        for (std::size_t i = 0; i < size_; ++i)
        {
            // Step 8a: Copy each string from source to this list
            data_[i] = other.data_[i];
        }
    }

    // Step 9: Return reference to this object (enables chaining: a = b = c)
    return *this;
}

// ======================= PUSH_BACK =======================

// Add string to end of list, dynamically growing array if needed
// Uses exponential growth strategy (double capacity) for efficiency
void StringList::push_back(const std::string& s)
{
    // Step 1: Check if list is at full capacity
    if (size_ == capacity_)
    {
        // Step 2: Calculate new capacity
        // Step 2a: If empty, start with capacity 8
        // Step 2b: If not empty, double current capacity (exponential growth)
        std::size_t newCap = capacity_ == 0 ? 8 : capacity_ * 2;

        // Step 3: Allocate new array with larger capacity
        std::string* newData = new std::string[newCap];

        // Step 4: Copy existing strings to new array
        // Loop through each element in current list
        for (std::size_t i = 0; i < size_; ++i)
        {
            // Step 4a: Copy string from old location to new location
            newData[i] = data_[i];
        }

        // Step 5: Delete old array (it's been copied)
        delete[] data_;

        // Step 6: Update pointer to new array
        data_ = newData;

        // Step 7: Update capacity to reflect new size
        capacity_ = newCap;
    }

    // Step 8: Add new string at current size position
    data_[size_] = s;

    // Step 9: Increment size to reflect new element
    ++size_;
}

// ======================= CLEAR =======================

// Remove all elements from list without deallocating memory
// Keeps allocated capacity for future reuse (efficient if list will be refilled)
void StringList::clear()
{
    // Step 1: Set size to 0 (logically empty)
    // Note: data_ and capacity_ remain unchanged, allowing reuse
    // strings destructors NOT called (size_ = 0, so loop doesn't execute on later use)
    size_ = 0;
}

// ======================= SUBSCRIPT OPERATOR =======================

// Access string at given index (read-only)
// Does not perform bounds checking (caller responsibility)
std::string StringList::operator[](std::size_t idx) const
{
    // Step 1: Return copy of string at index
    // Note: This returns by value, not reference (caller gets copy)
    return data_[idx];
}

// ======================= SIZE =======================

// Return number of elements currently in list
// Marked noexcept because it never throws exceptions
std::size_t StringList::size() const noexcept
{
    // Step 1: Return current size (number of elements)
    return size_;
}

// ======================= EMPTY =======================

// Check if list contains no elements
// Marked noexcept because it never throws exceptions
bool StringList::empty() const noexcept
{
    // Step 1: Return true if size is 0 (no elements)
    // Step 2: Return false otherwise (has elements)
    return size_ == 0;
}
