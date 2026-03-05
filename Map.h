#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <cstddef>      // for std::size_t
#include <stdexcept>    // for std::out_of_range

//==================================DECLARATION PART==================================

/**
 * @file Map.h
 * @brief A minimal hash table (map) implementation using separate chaining.
 *
 * @details This map associates keys with values using a hash table with linked lists
 * for collision resolution. It provides O(1) average-case insertion, lookup,
 * and deletion operations.
 *
 * Design Strategy:
 * - Hash table with separate chaining for collision handling
 * - 16 buckets (dynamic sizing not needed for this use case)
 * - Each bucket maintains a linked list of colliding entries
 * - Used for O(1) deduplication in weather data loading
 *
 * @tparam K Key type (must support == and hash via static_cast to size_t)
 * @tparam V Value type
 *
 */

template <class K, class V>
class Map
{
public:
    //============================== LIFECYCLE MANAGEMENT ==============================

    /**
     * @brief Default constructor. Initializes an empty map with 16 hash buckets.
     *
     * @details Creates a new Map with default capacity of 16 buckets.
     * Each bucket is initialized to nullptr (empty linked list).
     * Initial size is 0 (no entries).
     *
     * Time Complexity: O(1)
     * Space Complexity: O(capacity) = O(16) = O(1)
     *
     * Usage:
     * @code
     *   Map<unsigned long long, bool> timestamps;  // Deduplication set
     * @endcode
     *
     * @see Map(std::size_t capacity)
     */
    Map();

    /**
     * @brief Constructor with custom capacity.
     * @param capacity Number of hash table buckets (defaults to 16 if <= 0)
     *
     * @details Allows initialization with a specific number of buckets.
     * Useful for balancing memory vs. collision frequency.
     *
     * Time Complexity: O(capacity)
     * Space Complexity: O(capacity)
     *
     * Usage:
     * @code
     *   Map<int, unsigned long long> stats(32);  // Month statistics with 32 buckets
     * @endcode
     */
    explicit Map(std::size_t capacity);

    /**
     * @brief Destructor. Frees all allocated memory.
     *
     * @details Recursively deletes all nodes in all buckets.
     * Deallocates the bucket array.
     * Essential for preventing memory leaks.
     *
     * Time Complexity: O(n) where n = number of entries
     * Space Complexity: O(1)
     */
    ~Map();

    /**
     * @brief Copy constructor. Creates a deep copy of another map.
     * @param other The source map to copy
     *
     * @details Creates a completely independent copy of another Map.
     * All nodes and entries are duplicated (deep copy).
     * Prevents shallow copy issues with pointer members (buckets_).
     *
     * Time Complexity: O(n) where n = other.size()
     * Space Complexity: O(n)
     *
     * Example:
     * @code
     *   Map<int, int> original;
     *   original.put(1, 100);
     *   Map<int, int> copy = original;  // Deep copy created
     *   copy.put(2, 200);  // Doesn't affect 'original'
     * @endcode
     */
    Map(const Map& other);

    /**
     * @brief Copy assignment operator.
     * @param other The source map to assign from
     * @return Reference to this map (for chaining: a = b = c)
     *
     * @details Assigns contents of another map to this map.
     * Performs deep copy (all entries are duplicated).
     * Self-assignment safe (checks this == &other).
     * Frees old memory before copying.
     *
     * Time Complexity: O(n) where n = other.size()
     * Space Complexity: O(n)
     *
     * Usage:
     * @code
     *   Map<int, bool> map1, map2;
     *   map2 = map1;  // Copies all entries from map1 to map2
     * @endcode
     */
    Map& operator=(const Map& other);

    //============================== CORE OPERATIONS ==============================

    /**
     * @brief Associate a key with a value (insert or update).
     * @param key The key to insert/update
     * @param value The value to associate with the key
     *
     * PRIMARY METHOD: Implements core "key-value association" behaviour.
     * - If key doesn't exist: creates new entry and inserts it
     * - If key exists: updates its value (no duplicate keys)
     *
     * Algorithm:
     * 1. Hash the key to find bucket index
     * 2. Search linked list in that bucket for matching key
     * 3. If found: update value and return
     * 4. If not found: create new node, link at bucket head, increment size
     *
     * Time Complexity: O(1) average, O(n) worst case (all keys collide)
     * Space Complexity: O(1) per insertion
     *
     * Usage in program:
     * @code
     *   seenTimestamps_.put(timestamp_key, true);     // Mark as seen
     *   skippedByMonth.put(year_month_code, count);   // Track statistics
     * @endcode
     *
     * @see contains()
     */
    void put(const K& key, const V& value);

    /**
     * @brief Check if a key is associated with a value in this map.
     * @param key The key to check
     * @return true if key exists in map, false otherwise
     *
     * PRIMARY METHOD: Implements key membership testing.
     * Used for deduplication: "Have we seen this timestamp before?"
     *
     * Algorithm:
     * 1. Hash the key to find bucket index
     * 2. Traverse linked list in that bucket
     * 3. Compare each node's key with search key
     * 4. Return true if found, false if end of list reached
     *
     * Time Complexity: O(1) average, O(n) worst case
     * Space Complexity: O(1)
     *
     * Usage in program:
     * @code
     *   if (!seenTimestamps_.contains(key)) {
     *       readings_.insert(reading);        // Only insert if new
     *       seenTimestamps_.put(key, true);  // Mark as seen
     *   }
     * @endcode
     *
     * @see put()
     */
    bool contains(const K& key) const;

    /**
     * @brief Get the value associated with a key (mutable reference).
     * @param key The key to look up
     * @return Reference to the value (can be modified)
     * @throws std::out_of_range if key not found
     *
     * Retrieves value for modification.
     * Throws exception if key doesn't exist (safe lookup).
     *
     * Algorithm:
     * 1. Call findNode(key) to locate entry
     * 2. If not found: throw out_of_range exception
     * 3. If found: return reference to value
     *
     * Time Complexity: O(1) average, O(n) worst case
     * Space Complexity: O(1)
     *
     * Usage in program:
     * @code
     *   unsigned long long count = skippedByMonth.get(ym);
     *   skippedByMonth.put(ym, count + 1);  // Increment count
     * @endcode
     *
     * @see contains()
     */
    V& get(const K& key);

    /**
     * @brief Get the value associated with a key (const reference).
     * @param key The key to look up
     * @return Const reference to the value (cannot be modified)
     * @throws std::out_of_range if key not found
     *
     * Same as get() but returns const reference.
     * Prevents accidental modification of values.
     * Used for const Map objects.
     *
     * Time Complexity: O(1) average, O(n) worst case
     * Space Complexity: O(1)
     */
    const V& get(const K& key) const;

    /**
     * @brief Remove a key-value pair from the map.
     * @param key The key to remove
     * @return true if key was found and removed, false if key not found
     *
     * Deletes the entry associated with a key.
     * Returns false if key doesn't exist (safe deletion).
     * Decrements size counter.
     *
     * Algorithm:
     * 1. Hash key to find bucket index
     * 2. Traverse linked list, tracking previous node
     * 3. If key found:
     *    a. Unlink node from list (connect prev to next)
     *    b. Delete node memory
     *    c. Decrement size
     *    d. Return true
     * 4. If not found: return false
     *
     * Time Complexity: O(1) average, O(n) worst case
     * Space Complexity: O(1)
     *
     * @see contains()
     */
    bool remove(const K& key);

    //============================== UTILITY OPERATIONS ==============================

    /**
     * @brief Get the number of key-value associations currently stored.
     * @return The number of entries in the map
     *
     * Returns current size (number of put() calls minus remove() calls).
     * Useful for checking map occupancy and statistics.
     *
     * Algorithm: Return size_ member variable (maintained by put/remove)
     *
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     *
     * Usage in program:
     * @code
     *   std::cout << "Duplicates found: " << skippedByMonth.size() << "\n";
     * @endcode
     */
    std::size_t size() const noexcept;

    /**
     * @brief Check if the map contains any entries.
     * @return true if map has no entries, false otherwise
     *
     * Convenience method: equivalent to (size() == 0).
     *
     * Algorithm: Return (size_ == 0)
     *
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     */
    bool empty() const noexcept;

    /**
     * @brief Remove all key-value pairs from the map.
     *
     * Deletes all entries and resets map to empty state.
     * Used when reloading data: seenTimestamps_.clear()
     *
     * Algorithm:
     * 1. Call destroyAll() to delete all nodes and bucket array
     * 2. Create new empty bucket array (16 buckets)
     * 3. Initialize all buckets to nullptr
     * 4. Set size to 0
     *
     * Time Complexity: O(n) where n = current number of entries
     * Space Complexity: O(capacity)
     *
     * Usage in program:
     * @code
     *   seenTimestamps_.clear();  // Reset deduplication for new file
     * @endcode
     */
    void clear();

    /**
     * @brief Bracket operator for convenient array-like access.
     * @param key The key to access
     * @return Reference to the value (mutable)
     *
     * Provides C++ map-like syntax: map[key] = value
     * AUTO-INSERTS with default value if key not found!
     * Different from get(): get() throws exception, operator[] creates entry.
     *
     * Algorithm:
     * 1. Search for key using findNode()
     * 2. If found: return reference to value
     * 3. If not found:
     *    a. Create default value: V()
     *    b. Insert via put(key, default_value)
     *    c. Find and return reference to newly inserted value
     *
     * Time Complexity: O(1) average for both found and not-found cases
     * Space Complexity: O(1) per insertion
     *
     * Usage:
     * @code
     *   map[key] = new_value;     // Update if exists, create if not
     *   auto val = map[key];       // Retrieve (or create if missing)
     * @endcode
     *
     * @warning Inserts default value if key doesn't exist!
     *          Use contains() first if you only want to check existence.
     */
    V& operator[](const K& key);

private:
    //============================== PRIVATE DATA STRUCTURE ==============================

    /**
     * @struct Node
     * @brief Internal node structure for hash table collision resolution via chaining
     *
     * @details
     * Each Node represents a single entry in the hash table's collision chain.
     * When hash collisions occur (multiple keys hash to same bucket), nodes are
     * linked together in a chain using the `next` pointer.
     *
     * @tparam K The key type stored in the node
     * @tparam V The value type associated with the key
     *
     * @note
     * Nodes intentionally disable copy constructor and assignment operator
     * because they manage pointer chains. Shallow copies would cause
     * dangling pointers and memory leaks. Each node should be unique
     * within its chain.
     */
    struct Node
    {
        /**
         * @brief The key stored in this node
         * @details Used for lookups and comparisons during get/contains operations
         */
        K key;

        /**
         * @brief The value associated with the key
         * @details Retrieved when key is found during map lookup
         */
        V value;

        /**
         * @brief Pointer to next node in the collision chain
         * @details nullptr if this is the last node in the chain
         */
        Node* next;

        /**
         * @brief Constructor - creates node with key-value pair
         * @param [in] k The key to store in this node
         * @param [in] v The value to store in this node
         *
         * @details Initializes @c next to nullptr since new nodes
         * are always inserted at chain beginning
         */
        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}

        /**
         * @brief Deleted copy constructor
         *
         * @details Copy construction is disabled because nodes contain
         * pointers to other nodes. Shallow copying would create multiple
         * nodes pointing to same chain, breaking map invariants.
         * Use move semantics or explicit node recreation instead.
         */
        Node(const Node&) = delete;

        /**
         * @brief Deleted assignment operator
         *
         * @details Assignment is disabled because nodes contain pointers
         * to other nodes. Shallow assignment would break chain structure
         * and cause memory issues. Each node must maintain its own chain.
         */
        Node& operator=(const Node&) = delete;
    };

    Node** buckets_;        ///< Array of bucket heads (points to linked lists)
    std::size_t capacity_;  ///< Number of buckets in hash table
    std::size_t size_;      ///< Current number of key-value pairs stored

    /**
     * @brief Compute hash index for a key.
     * @param key The key to hash
     * @return Bucket index (0 to capacity_-1)
     *
     * Algorithm: Simple modulo hashing
     * - Cast key to size_t
     * - Take modulo capacity_
     * - Result: evenly distributed bucket index
     */
    std::size_t hash(const K& key) const;

    /**
     * @brief Find a node with matching key in a bucket.
     * @param key The key to search for
     * @return Pointer to node if found, nullptr if not found
     *
     * Private helper: searches linked list at bucket for given key.
     * Used by contains(), get(), remove(), operator[]
     */
    Node* findNode(const K& key) const;

    /**
     * @brief Helper for copy constructor and assignment operator.
     * @param other The map to copy from
     *
     * Copies all entries from 'other' map to this map.
     * First initializes buckets, then iterates through all entries
     * and inserts them via put().
     */
    void copyFrom(const Map& other);

    /**
     * @brief Cleanup: delete all nodes and deallocate bucket array.
     *
     * Deletes all Node objects in all buckets.
     * Deallocates the buckets_ array.
     * Sets buckets_ to nullptr.
     */
    void destroyAll();
};

//==================================IMPLEMENTATION PART ==================================

//============================== CONSTRUCTORS & DESTRUCTORS ==============================

template <class K, class V>
inline Map<K, V>::Map() : buckets_(nullptr), capacity_(16), size_(0)
{
    // Allocate array of 16 bucket pointers
    buckets_ = new Node*[capacity_];

    // Initialize all buckets to nullptr (empty linked lists)
    for (std::size_t i = 0; i < capacity_; ++i)
    {
        buckets_[i] = nullptr;
    }
    // After initialization: Map is empty, ready for insertions
}

template <class K, class V>
inline Map<K, V>::Map(std::size_t capacity)
    : buckets_(nullptr), capacity_(capacity > 0 ? capacity : 16), size_(0)
{
    // Allocate bucket array with specified capacity (min 16)
    buckets_ = new Node*[capacity_];

    // Initialize all buckets to nullptr
    for (std::size_t i = 0; i < capacity_; ++i)
    {
        buckets_[i] = nullptr;
    }
}

template <class K, class V>
inline Map<K, V>::~Map()
{
    // Clean up all nodes and deallocate bucket array
    destroyAll();
}

template <class K, class V>
inline void Map<K, V>::destroyAll()
{
    // Early exit if already destroyed
    if (!buckets_) return;

    // Iterate through all buckets
    for (std::size_t i = 0; i < capacity_; ++i)
    {
        // For each bucket, traverse the linked list
        Node* curr = buckets_[i];
        while (curr)
        {
            // Save next pointer before deleting current node
            Node* next = curr->next;

            // Delete the current node
            delete curr;

            // Move to next node in list
            curr = next;
        }
    }

    // After all nodes deleted, deallocate bucket array itself
    delete[] buckets_;
    buckets_ = nullptr;
    size_ = 0;
}

//============================== COPY OPERATIONS ==============================

template <class K, class V>
inline void Map<K, V>::copyFrom(const Map& other)
{
    // Set capacity to match source map
    capacity_ = other.capacity_;
    size_ = 0;  // Start with empty size (will increment in put)

    // Allocate new bucket array
    buckets_ = new Node*[capacity_];

    // Initialize all new buckets to nullptr
    for (std::size_t i = 0; i < capacity_; ++i)
    {
        buckets_[i] = nullptr;
    }

    // Iterate through all buckets in source map
    for (std::size_t i = 0; i < other.capacity_; ++i)
    {
        // Start at head of source bucket's linked list
        Node* curr = other.buckets_[i];

        // Traverse entire linked list for this bucket
        while (curr)
        {
            // Copy each entry using put() (handles insertion + hashing)
            put(curr->key, curr->value);

            // Move to next node
            curr = curr->next;
        }
    }
    // After loop: this map is complete deep copy of other
}

template <class K, class V>
inline Map<K, V>::Map(const Map& other) : buckets_(nullptr), capacity_(0), size_(0)
{
    // Delegate to copyFrom for actual copying
    copyFrom(other);
}

template <class K, class V>
inline Map<K, V>& Map<K, V>::operator=(const Map& other)
{
    // Check for self-assignment (a = a)
    if (this == &other) return *this;

    // Delete old contents
    destroyAll();

    // Copy contents from other
    copyFrom(other);

    // Return reference for chaining (a = b = c)
    return *this;
}

//============================== HASH FUNCTION ==============================

template <class K, class V>
inline std::size_t Map<K, V>::hash(const K& key) const
{
    // Step 1: Create hash functor object specialized for template type K
    // std::hash<K> automatically selects correct hashing algorithm for the type
    // For std::string: uses internal string hashing algorithm
    // For int/double: uses built-in type hashing
    std::hash<K> hasher;

    // Step 2: Call hash function operator on the key
    // hasher(key) computes hash value as large std::size_t integer
    // Different keys generally produce different hash values
    std::size_t hashValue = hasher(key);

    // Step 3: Apply modulo with capacity_ to get valid bucket index
    // Ensures returned index is in range [0, capacity_ - 1]
    // Prevents array out-of-bounds access
    // Distributes keys across all buckets in hash table
    return hashValue % capacity_;
}

//============================== STRING HASH SPECIALIZATION ==============================

// Template specialization for Map with std::string key and int value
// Implements custom hash function for string keys using polynomial rolling hash
template<>
inline std::size_t Map<std::string, int>::hash(const std::string& key) const
{
    // Step 1: Initialize hash accumulator to zero
    // This will store the computed hash value as we process each character
    std::size_t h = 0;

    // Step 2: Loop through each character in the string key
    // Index i goes from 0 to key.size()-1
    for (std::size_t i = 0; i < key.size(); ++i)
    {
        // Step 3: Get current character from string at position i
        // key[i] returns char (ASCII value 0-127 for standard characters)

        // Step 4: Multiply accumulated hash by 31 (prime number for distribution)
        // This shifts previous hash value and makes room for new character
        // Prime multiplier (31) provides good distribution across buckets
        // Prevents collisions by using different weights for each position
        h = h * 31;

        // Step 5: Cast character to std::size_t (converts ASCII value to unsigned int)
        // Ensures we can add character value to hash without type errors
        std::size_t charValue = static_cast<std::size_t>(key[i]);

        // Step 6: Add current character's numeric value to hash
        // Combines all characters into single hash value
        // Different strings will have different hash values with high probability
        h = h + charValue;
    }

    // Step 7: Apply modulo with capacity_ to get valid bucket index
    // Reduces large hash value to valid range [0, capacity_ - 1]
    // Ensures hash maps to actual bucket in hash table array
    // Prevents out-of-bounds array access
    return h % capacity_;
}


//============================== CORE LOOKUP HELPER ==============================

template <class K, class V>
inline typename Map<K, V>::Node* Map<K, V>::findNode(const K& key) const
{
    // Step 1: Compute bucket index for this key
    std::size_t idx = hash(key);

    // Step 2: Get pointer to head of linked list for this bucket
    Node* curr = buckets_[idx];

    // Step 3: Traverse linked list, comparing keys
    while (curr)
    {
        // If key matches, we found it!
        if (curr->key == key)
            return curr;  // Found: return pointer to node

        // Key doesn't match, move to next node
        curr = curr->next;
    }

    // Step 4: End of list reached without finding key
    return nullptr;  // Not found
}

//============================== PUT: INSERT/UPDATE ==============================

template <class K, class V>
inline void Map<K, V>::put(const K& key, const V& value)
{
    // Step 1: Compute bucket index using hash function
    std::size_t idx = hash(key);

    // Step 2: Get pointer to head of linked list for this bucket
    Node* curr = buckets_[idx];

    // Step 3: Search linked list for existing key
    while (curr)
    {
        // If key already exists...
        if (curr->key == key)
        {
            // ...update its value (no duplicate keys)
            curr->value = value;
            return;  // Done!
        }

        // Key not matched, move to next node
        curr = curr->next;
    }

    // Step 4: Key not found in bucket, create new entry
    // Create new node with key and value
    Node* newNode = new Node(key, value);

    // Insert at HEAD of bucket's linked list (O(1) insertion)
    newNode->next = buckets_[idx];  // New node points to old head
    buckets_[idx] = newNode;        // New node becomes new head

    // Increment size counter
    ++size_;

    // Algorithm complete: entry is now in map
}

//============================== CONTAINS: MEMBERSHIP TEST ==============================

template <class K, class V>
inline bool Map<K, V>::contains(const K& key) const
{
    // Use findNode helper: if returns non-null, key exists
    return findNode(key) != nullptr;
}

//============================== GET: RETRIEVE VALUE (MUTABLE) ==============================

template <class K, class V>
inline V& Map<K, V>::get(const K& key)
{
    // Use findNode to search for key
    Node* node = findNode(key);

    // If not found, throw exception (safe lookup)
    if (!node)
    {
        throw std::out_of_range("Map::get - key not found");
    }

    // Return reference to value (can be modified)
    return node->value;
}

//============================== GET: RETRIEVE VALUE (CONST) ==============================

template <class K, class V>
inline const V& Map<K, V>::get(const K& key) const
{
    // Use findNode to search for key
    Node* node = findNode(key);

    // If not found, throw exception
    if (!node)
    {
        throw std::out_of_range("Map::get - key not found");
    }

    // Return const reference to value (cannot be modified)
    return node->value;
}

//============================== REMOVE: DELETE ENTRY ==============================

template <class K, class V>
inline bool Map<K, V>::remove(const K& key)
{
    // Step 1: Compute bucket index
    std::size_t idx = hash(key);

    // Step 2: Get pointer to head of linked list
    Node* curr = buckets_[idx];
    Node* prev = nullptr;  // Track previous node for unlinking

    // Step 3: Traverse linked list looking for key
    while (curr)
    {
        // If key found...
        if (curr->key == key)
        {
            // Step 4a: Unlink node from list
            if (prev)
            {
                // Normal case: unlink from middle/end
                prev->next = curr->next;
            }
            else
            {
                // Special case: unlink from head
                buckets_[idx] = curr->next;
            }

            // Step 4b: Delete the node
            delete curr;

            // Step 4c: Decrement size
            --size_;

            // Step 4d: Report success
            return true;
        }

        // Not found yet, move to next node
        prev = curr;
        curr = curr->next;
    }

    // Step 5: End of list, key not found
    return false;
}

//============================== SIZE: GET COUNT ==============================

template <class K, class V>
inline std::size_t Map<K, V>::size() const noexcept
{
    // Simply return the size counter (O(1))
    return size_;
}

//============================== EMPTY: CHECK IF EMPTY ==============================

template <class K, class V>
inline bool Map<K, V>::empty() const noexcept
{
    // Map is empty if size is 0
    return size_ == 0;
}

//============================== CLEAR: REMOVE ALL ENTRIES ==============================

template <class K, class V>
inline void Map<K, V>::clear()
{
    // Step 1: Delete all nodes in all buckets
    destroyAll();

    // Step 2: Allocate fresh bucket array
    buckets_ = new Node*[capacity_];

    // Step 3: Initialize all buckets to nullptr
    for (std::size_t i = 0; i < capacity_; ++i)
    {
        buckets_[i] = nullptr;
    }

    // After clear(): map is empty, ready for new insertions
}

//============================== OPERATOR[]: ARRAY-LIKE ACCESS ==============================

template <class K, class V>
inline V& Map<K, V>::operator[](const K& key)
{
    // Step 1: Try to find existing node
    Node* node = findNode(key);

    // Step 2: If found, return reference to its value
    if (node)
    {
        return node->value;
    }

    // Step 3: Key not found, insert default value
    // Create default value (calls V's default constructor)
    V defaultValue = V();

    // Insert the default value using put()
    put(key, defaultValue);

    // Step 4: Find and return reference to newly inserted value
    return findNode(key)->value;

    // NOTE: This auto-insert behavior differs from get()!
    //       get() throws exception, operator[] creates entry
}

#endif // MAP_H_INCLUDED
