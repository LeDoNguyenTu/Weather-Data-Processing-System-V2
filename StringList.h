#ifndef STRINGLIST_H_INCLUDED
#define STRINGLIST_H_INCLUDED

#include <string>
#include <cstddef>

/**
 * @class StringList
 * @brief A dynamic array for storing strings, supporting basic list operations.
 *
 * Used within WeatherProcessor for temporary storage of CSV fields during parsing.
 */
class StringList
{
public:
    /**
     * @brief Default constructor creates an empty list.
     *        Initializes the data pointer and size/capacity to zero.
     */
    StringList();

    /**
     * @brief Destructor releases dynamically allocated array.
     *        Cleans up any strings in the internal array.
     */
    ~StringList();

    /**
     * @brief Copy constructor makes a deep copy of another StringList.
     *        Every string is duplicated; not a shallow pointer copy.
     * @param other Source StringList to copy from.
     */
    StringList(const StringList& other);

    /**
     * @brief Copy assignment operator: replaces this list with contents of another.
     * @param other Source StringList to copy from.
     * @return Reference to this StringList (for chaining).
     */
    StringList& operator=(const StringList& other);

    /**
     * @brief Appends a new string to the end of the list.
     *        Dynamically grows the internal array if needed.
     * @param s String to add.
     *
     * Used every time a new CSV field is parsed.
     */
    void push_back(const std::string& s);

    /**
     * @brief Removes all strings and resets the list to empty.
     *        Does not change capacity -- strings will be overwritten on next usage.
     */
    void clear();

    /**
     * @brief Indexed access to a string with no bounds checking.
     *        Returns string at given index.
     *        Used to access CSV fields after parsing.
     * @param idx List index.
     * @return Copy of string at that index.
     */
    std::string operator[](std::size_t idx) const;

    /**
     * @brief Returns the number of strings currently stored in the list.
     */
    std::size_t size() const noexcept;

    /**
     * @brief Checks whether the list is empty.
     * @return True if size is zero, false otherwise.
     */
    bool empty() const noexcept;

private:
    std::string* data_;      ///< Dynamically-allocated array of strings
    std::size_t size_;       ///< Number of valid strings in the list
    std::size_t capacity_;   ///< Allocated size of array (may be larger than size_)
};

#endif // STRINGLIST_H_INCLUDED
