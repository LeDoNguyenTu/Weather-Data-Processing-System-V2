#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "AvlTree.h"
#include "Date.h"

// ======================= TESTING FRAMEWORK =======================

// Global counters for tracking test results
static int g_pass = 0, g_fail = 0;

// ======================= TEST HELPER FUNCTIONS =======================

// Template equality check for any type supporting == operator
template <typename T>
void check_eq(const T& actual, const T& expected, const char* msg)
{
    // Step 1: Compare actual value with expected value
    const bool ok = (actual == expected);

    // Step 2: Print test message with expected, actual, and result
    std::cout << msg << " | expected=" << expected << " actual=" << actual << " -> " << (ok ? 1 : 0) << "\n";

    // Step 3: Update pass/fail counter
    if (ok) ++g_pass;
    else ++g_fail;
}

// Generic boolean condition check
static void check(bool cond, const char* msg)
{
    // Step 1: Print test message with result (1=pass, 0=fail)
    std::cout << msg << " -> " << (cond ? 1 : 0) << "\n";

    // Step 2: Update pass/fail counter
    if (cond) ++g_pass;
    else ++g_fail;
}

// ======================= TRAVERSAL HELPERS =======================

// Capture inorder traversal output to string for comparison
template<typename T>
std::string captureInOrder(const AvlTree<T>& tree)
{
    // Step 1: Create output string stream
    std::ostringstream oss;

    // Step 2: Call tree's inOrder traversal, output to stream
    tree.inOrder(oss);

    // Step 3: Return captured string
    return oss.str();
}

// Capture preorder traversal output to string for comparison
template<typename T>
std::string capturePreOrder(const AvlTree<T>& tree)
{
    // Step 1: Create output string stream
    std::ostringstream oss;

    // Step 2: Call tree's preOrder traversal, output to stream
    tree.preOrder(oss);

    // Step 3: Return captured string
    return oss.str();
}

// Capture postorder traversal output to string for comparison
template<typename T>
std::string capturePostOrder(const AvlTree<T>& tree)
{
    // Step 1: Create output string stream
    std::ostringstream oss;

    // Step 2: Call tree's postOrder traversal, output to stream
    tree.postOrder(oss);

    // Step 3: Return captured string
    return oss.str();
}

// Print tree status information for debugging
template<typename T>
void printAvl(const AvlTree<T>& tree, const char* label)
{
    // Step 1: Print label and tree statistics
    std::cout << label << " size=" << tree.size()

              // Step 2: Print empty status
              << " empty=" << (tree.empty() ? "yes" : "no")

              // Step 3: Print AVL invariant status (height-balanced property)
              << " invariant=" << (tree.checkInvariant() ? "OK" : "BROKEN")

              // Step 4: Print inorder traversal for verification
              << " | inOrder=[ " << captureInOrder(tree) << "]\n";
}

// ======================= FUNCTION POINTER HELPERS =======================

// Pass tree by value (invokes copy constructor)
template<typename T>
AvlTree<T> passByValue(AvlTree<T> tree)
{
    // Step 1: Print notification (copy happened in parameter passing)
    std::cout << "  [Inside passByValue function, tree copied via copy constructor]\n";

    // Step 2: Return copy (invokes move or copy constructor)
    return tree;
}

// Pass tree by const reference (no copy)
template<typename T>
size_t passByConstRef(const AvlTree<T>& tree)
{
    // Step 1: Print notification (no copy made)
    std::cout << "  [Inside passByConstRef function, no copy made]\n";

    // Step 2: Return size without copying tree
    return tree.size();
}

// Pass tree by reference and modify
template<typename T>
void passByRef(AvlTree<T>& tree, const T& value)
{
    // Step 1: Print notification (modifying original)
    std::cout << "  [Inside passByRef function, modifying original tree]\n";

    // Step 2: Insert value into original tree
    tree.insert(value);
}

// ======================= TEST 1: Empty Tree =======================

static void test_empty_tree()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 1: Empty tree operations ------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Print creation message
    std::cout << "Creating empty AvlTree<int>...\n";

    // Step 3: Create empty tree
    AvlTree<int> tree;

    // Step 4: Print tree status
    printAvl(tree, "tree:");

    // Step 5: Check size is 0
    check_eq(tree.size(), (std::size_t)0, "empty tree size is 0");

    // Step 6: Check empty() returns true
    check(tree.empty(), "empty tree: empty() returns true");

    // Step 7: Check invariant holds on empty tree
    check(tree.checkInvariant(), "empty tree maintains invariant");

    // Step 8: Check search returns false for any value
    check(!tree.search(5), "empty tree does not contain 5");
    check(!tree.search(0), "empty tree does not contain 0");

    // Step 9: Test empty traversals
    std::cout << "Checking empty traversals...\n";
    std::string inOrder = captureInOrder(tree);
    check(inOrder.empty() || inOrder == " ", "inOrder of empty tree is empty");

    // Step 10: Test clear on empty tree
    std::cout << "Calling clear() on empty tree...\n";
    tree.clear();
    printAvl(tree, "tree after clear:");
    check_eq(tree.size(), (std::size_t)0, "clear on empty: size still 0");
    check(tree.checkInvariant(), "clear on empty: invariant OK");
}

// ======================= TEST 2: Single Element =======================

static void test_single_element()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 2: Single element operations --------------";
    std::cout << "\n====================================================\n";

    // Step 2: Print insertion message
    std::cout << "Creating AVL tree and inserting 42...\n";

    // Step 3: Create tree and insert single element
    AvlTree<int> tree;
    tree.insert(42);

    // Step 4: Print tree status
    printAvl(tree, "tree:");

    // Step 5: Check size is 1
    check_eq(tree.size(), (std::size_t)1, "size after one insert");

    // Step 6: Check not empty
    check(!tree.empty(), "tree is not empty");

    // Step 7: Check search finds 42
    check(tree.search(42), "tree contains 42");

    // Step 8: Check search doesn't find nearby values
    check(!tree.search(41), "tree does not contain 41");
    check(!tree.search(43), "tree does not contain 43");

    // Step 9: Check invariant (single node, height 0)
    check(tree.checkInvariant(), "single element: invariant OK (height=0)");

    // Step 10: Test traversals of single-node tree
    std::cout << "Checking traversals of single-node tree...\n";
    check_eq(captureInOrder(tree), std::string("42 "), "inOrder: 42");
    check_eq(capturePreOrder(tree), std::string("42 "), "preOrder: 42");
    check_eq(capturePostOrder(tree), std::string("42 "), "postOrder: 42");
}

// ======================= TEST 3: Multiple Elements =======================

static void test_multiple_elements()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 3: Multiple element insertion & ordering --";
    std::cout << "\n====================================================\n";

    // Step 2: Print insertion sequence
    std::cout << "Creating AVL tree and inserting: 50, 30, 70, 20, 40, 60, 80...\n";

    // Step 3: Create tree and insert 7 elements
    AvlTree<int> tree;
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    tree.insert(60);
    tree.insert(80);

    // Step 4: Print tree status
    printAvl(tree, "tree:");

    // Step 5: Check size is 7
    check_eq(tree.size(), (std::size_t)7, "size after 7 inserts");

    // Step 6: Check all elements present
    std::cout << "Checking all elements are present...\n";
    check(tree.search(50), "contains 50");
    check(tree.search(30), "contains 30");
    check(tree.search(70), "contains 70");
    check(tree.search(20), "contains 20");
    check(tree.search(40), "contains 40");
    check(tree.search(60), "contains 60");
    check(tree.search(80), "contains 80");

    // Step 7: Check non-existent elements
    std::cout << "Checking non-existent elements...\n";
    check(!tree.search(10), "does not contain 10");
    check(!tree.search(35), "does not contain 35");
    check(!tree.search(90), "does not contain 90");

    // Step 8: Check AVL invariant
    check(tree.checkInvariant(), "multi-element: AVL invariant OK");

    // Step 9: Verify inorder traversal is sorted
    std::cout << "Verifying traversal orders...\n";
    check_eq(captureInOrder(tree), std::string("20 30 40 50 60 70 80 "), "inOrder sorted");
}

// ======================= TEST 4: Duplicates =======================

static void test_duplicates()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 4: Duplicate handling ---------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create tree with initial elements
    std::cout << "Creating AVL tree with 10, 5, 15...\n";
    AvlTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);

    // Step 3: Print tree status
    printAvl(tree, "tree:");
    check_eq(tree.size(), (std::size_t)3, "initial size is 3");

    // Step 4: Try inserting duplicates
    std::cout << "Inserting duplicates: 10, 5, 15...\n";
    tree.insert(10);  // duplicate root
    tree.insert(5);   // duplicate left
    tree.insert(15);  // duplicate right

    // Step 5: Print tree after duplicate attempts
    printAvl(tree, "tree after duplicates:");

    // Step 6: Check size unchanged (duplicates rejected)
    check_eq(tree.size(), (std::size_t)3, "size unchanged after duplicates");

    // Step 7: Verify all original elements still present
    check(tree.search(10), "still contains 10");
    check(tree.search(5), "still contains 5");
    check(tree.search(15), "still contains 15");

    // Step 8: Check invariant maintained
    check(tree.checkInvariant(), "duplicates: invariant OK");

    // Step 9: Verify traversal unchanged
    check_eq(captureInOrder(tree), std::string("5 10 15 "), "duplicates: inOrder unchanged");
}

// ======================= TEST 5: LL Rotation =======================

static void test_right_rotation_LL()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 5: AVL Right Rotation (LL case) -----------";
    std::cout << "\n====================================================\n";

    // Step 2: Print insertion sequence that triggers LL case
    std::cout << "Inserting 30, 20, 10 to trigger LL case (right rotation)...\n";

    // Step 3: Create tree and insert first element
    AvlTree<int> tree;
    tree.insert(30);
    printAvl(tree, "after insert 30:");

    // Step 4: Insert second element
    tree.insert(20);
    printAvl(tree, "after insert 20:");

    // Step 5: Insert third element (triggers right rotation)
    tree.insert(10);
    printAvl(tree, "after insert 10 (rotation triggered):");

    // Step 6: Check size is 3
    check_eq(tree.size(), (std::size_t)3, "size is 3");

    // Step 7: Check invariant after rotation
    check(tree.checkInvariant(), "LL rotation: AVL invariant OK");

    // Step 8: Verify tree structure after rotation
    // After LL rotation: root should be 20, left=10, right=30
    std::string pre = capturePreOrder(tree);
    check_eq(pre, std::string("20 10 30 "), "LL rotation: root is now 20");

    // Step 9: Verify sorted order maintained
    check_eq(captureInOrder(tree), std::string("10 20 30 "), "LL rotation: inOrder sorted");
}

// ======================= TEST 6: RR Rotation =======================

static void test_left_rotation_RR()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 6: AVL Left Rotation (RR case) ------------";
    std::cout << "\n====================================================\n";

    // Step 2: Print insertion sequence that triggers RR case
    std::cout << "Inserting 10, 20, 30 to trigger RR case (left rotation)...\n";

    // Step 3: Create tree and insert first element
    AvlTree<int> tree;
    tree.insert(10);
    printAvl(tree, "after insert 10:");

    // Step 4: Insert second element
    tree.insert(20);
    printAvl(tree, "after insert 20:");

    // Step 5: Insert third element (triggers left rotation)
    tree.insert(30);
    printAvl(tree, "after insert 30 (rotation triggered):");

    // Step 6: Check size is 3
    check_eq(tree.size(), (std::size_t)3, "size is 3");

    // Step 7: Check invariant after rotation
    check(tree.checkInvariant(), "RR rotation: AVL invariant OK");

    // Step 8: Verify tree structure after rotation
    // After RR rotation: root should be 20, left=10, right=30
    std::string pre = capturePreOrder(tree);
    check_eq(pre, std::string("20 10 30 "), "RR rotation: root is now 20");

    // Step 9: Verify sorted order maintained
    check_eq(captureInOrder(tree), std::string("10 20 30 "), "RR rotation: inOrder sorted");
}

// ======================= TEST 7: LR Rotation =======================

static void test_left_right_rotation_LR()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 7: AVL Left-Right Rotation (LR case) ------";
    std::cout << "\n====================================================\n";

    // Step 2: Print insertion sequence that triggers LR case
    std::cout << "Inserting 30, 10, 20 to trigger LR case (left-right rotation)...\n";

    // Step 3: Create tree and insert first element
    AvlTree<int> tree;
    tree.insert(30);
    printAvl(tree, "after insert 30:");

    // Step 4: Insert second element
    tree.insert(10);
    printAvl(tree, "after insert 10:");

    // Step 5: Insert third element (triggers LR rotation)
    tree.insert(20);
    printAvl(tree, "after insert 20 (rotation triggered):");

    // Step 6: Check size is 3
    check_eq(tree.size(), (std::size_t)3, "size is 3");

    // Step 7: Check invariant after rotation
    check(tree.checkInvariant(), "LR rotation: AVL invariant OK");

    // Step 8: Verify tree structure after rotation
    // After LR rotation: root should be 20, left=10, right=30
    std::string pre = capturePreOrder(tree);
    check_eq(pre, std::string("20 10 30 "), "LR rotation: root is now 20");

    // Step 9: Verify sorted order maintained
    check_eq(captureInOrder(tree), std::string("10 20 30 "), "LR rotation: inOrder sorted");
}

// ======================= TEST 8: RL Rotation =======================

static void test_right_left_rotation_RL()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 8: AVL Right-Left Rotation (RL case) ------";
    std::cout << "\n====================================================\n";

    // Step 2: Print insertion sequence that triggers RL case
    std::cout << "Inserting 10, 30, 20 to trigger RL case (right-left rotation)...\n";

    // Step 3: Create tree and insert first element
    AvlTree<int> tree;
    tree.insert(10);
    printAvl(tree, "after insert 10:");

    // Step 4: Insert second element
    tree.insert(30);
    printAvl(tree, "after insert 30:");

    // Step 5: Insert third element (triggers RL rotation)
    tree.insert(20);
    printAvl(tree, "after insert 20 (rotation triggered):");

    // Step 6: Check size is 3
    check_eq(tree.size(), (std::size_t)3, "size is 3");

    // Step 7: Check invariant after rotation
    check(tree.checkInvariant(), "RL rotation: AVL invariant OK");

    // Step 8: Verify tree structure after rotation
    // After RL rotation: root should be 20, left=10, right=30
    std::string pre = capturePreOrder(tree);
    check_eq(pre, std::string("20 10 30 "), "RL rotation: root is now 20");

    // Step 9: Verify sorted order maintained
    check_eq(captureInOrder(tree), std::string("10 20 30 "), "RL rotation: inOrder sorted");
}

// ======================= TEST 9: Sequential Ascending =======================

static void test_sequential_ascending()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 9: Sequential ascending (stress test) -----";
    std::cout << "\n====================================================\n";

    // Step 2: Print note about non-AVL behavior
    std::cout << "Inserting 1-20 in ascending order (would create right skew in regular BST)...\n";

    // Step 3: Create tree and insert sequential values
    AvlTree<int> tree;
    for (int i = 1; i <= 20; ++i)
    {
        tree.insert(i);
    }

    // Step 4: Print tree status
    printAvl(tree, "tree after 20 sequential inserts:");

    // Step 5: Check size is 20
    check_eq(tree.size(), (std::size_t)20, "size is 20");

    // Step 6: Check invariant (AVL prevents skew)
    check(tree.checkInvariant(), "sequential ascending: AVL invariant OK");

    // Step 7: Verify all elements present
    std::cout << "Verifying all elements present...\n";
    bool all_present = true;
    for (int i = 1; i <= 20; ++i)
    {
        if (!tree.search(i))
        {
            all_present = false;
            break;
        }
    }
    check(all_present, "all 20 elements present");

    // Step 8: Note about AVL benefit
    std::cout << "Note: AVL tree prevents right-skew degeneration!\n";
}

// ======================= TEST 10: Sequential Descending =======================

static void test_sequential_descending()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 10: Sequential descending (stress test) ---";
    std::cout << "\n====================================================\n";

    // Step 2: Print note about non-AVL behavior
    std::cout << "Inserting 20-1 in descending order (would create left skew in regular BST)...\n";

    // Step 3: Create tree and insert sequential values in reverse
    AvlTree<int> tree;
    for (int i = 20; i >= 1; --i)
    {
        tree.insert(i);
    }

    // Step 4: Print tree status
    printAvl(tree, "tree after 20 sequential descending inserts:");

    // Step 5: Check size is 20
    check_eq(tree.size(), (std::size_t)20, "size is 20");

    // Step 6: Check invariant (AVL prevents skew)
    check(tree.checkInvariant(), "sequential descending: AVL invariant OK");

    // Step 7: Verify all elements present
    std::cout << "Verifying all elements present...\n";
    bool all_present = true;
    for (int i = 1; i <= 20; ++i)
    {
        if (!tree.search(i))
        {
            all_present = false;
            break;
        }
    }
    check(all_present, "all 20 elements present");

    // Step 8: Note about AVL benefit
    std::cout << "Note: AVL tree prevents left-skew degeneration!\n";
}

// ======================= TEST 11: Function Pointer Apply =======================

static void test_function_pointer_apply()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 11: Function pointer traversals -----------";
    std::cout << "\n====================================================\n";

    // Step 2: Create tree with multiple elements
    std::cout << "Creating tree with 50, 30, 70, 20, 40, 60, 80...\n";
    AvlTree<int> tree;
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    tree.insert(60);
    tree.insert(80);

    // Step 3: Print tree status
    printAvl(tree, "tree:");

    // Step 4: Test inOrderApply with accumulation
    std::cout << "Testing inOrderApply with sum accumulator...\n";

    // Step 5: Initialize sum to 0
    static int sum = 0;
    sum = 0;

    // Step 6: Define helper struct with static method
    struct Helper
    {
        // Step 6a: Static callback function to accumulate values
        static void addToSum(const int& val)
        {
            sum += val;
        }
    };

    // Step 7: Apply function to all elements in order
    tree.inOrderApply(Helper::addToSum);

    // Step 8: Print result
    std::cout << "\nSum of all elements via inOrderApply: " << sum << "\n";

    // Step 9: Verify sum (20+30+40+50+60+70+80=350)
    check_eq(sum, 350, "inOrderApply: sum is correct (20+30+40+50+60+70+80=350)");
}

// ======================= TEST 12: Function Pointer If =======================

static void test_function_pointer_if()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 12: Function pointer conditional apply ----";
    std::cout << "\n====================================================\n";

    // Step 2: Create tree with even numbers
    std::cout << "Creating tree with 10, 20, 30, 40, 50, 60, 70, 80, 90, 100...\n";
    AvlTree<int> tree;
    for (int i = 10; i <= 100; i += 10)
    {
        tree.insert(i);
    }

    // Step 3: Print tree status
    printAvl(tree, "tree:");

    // Step 4: Initialize accumulator for even sum
    static int evenSum = 0;
    evenSum = 0;

    // Step 5: Define helper with predicate and action functions
    struct Helper
    {
        // Step 5a: Predicate function - check if value is even
        static bool isEven(const int& val)
        {
            return (val % 2 == 0);
        }

        // Step 5b: Action function - add to sum if even
        static void addEven(const int& val)
        {
            evenSum += val;
        }
    };

    // Step 6: Apply action only to values matching predicate
    std::cout << "Using inOrderIf to sum only even numbers...\n";
    tree.inOrderIf(Helper::isEven, Helper::addEven);

    // Step 7: Print result
    std::cout << "Sum of even elements: " << evenSum << "\n";

    // Step 8: Verify sum (all 10 elements are even, so sum = 550)
    check_eq(evenSum, 550, "inOrderIf: sum of evens is 550 (all are even)");
}

// ======================= TEST 13: Copy Constructor =======================

static void test_copy_constructor()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 13: Copy constructor ----------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create original tree with elements
    std::cout << "Creating original AVL tree with 50, 30, 70, 20, 40...\n";
    AvlTree<int> original;
    original.insert(50);
    original.insert(30);
    original.insert(70);
    original.insert(20);
    original.insert(40);

    // Step 3: Print original tree
    printAvl(original, "original:");

    // Step 4: Create copy via copy constructor
    std::cout << "Creating copy via copy constructor: AvlTree<int> copy(original)...\n";
    AvlTree<int> copy(original);

    // Step 5: Print copy
    printAvl(copy, "copy:");

    // Step 6: Verify original size
    check_eq(original.size(), (std::size_t)5, "original size is 5");

    // Step 7: Verify copy has same size
    check_eq(copy.size(), (std::size_t)5, "copy size is 5");

    // Step 8: Verify copy contains all original elements
    std::cout << "Verifying copy has same elements...\n";
    check(copy.search(50), "copy contains 50");
    check(copy.search(30), "copy contains 30");
    check(copy.search(70), "copy contains 70");
    check(copy.search(20), "copy contains 20");
    check(copy.search(40), "copy contains 40");

    // Step 9: Verify traversal order is same
    check_eq(captureInOrder(original), captureInOrder(copy), "same inOrder traversal");

    // Step 10: Modify copy by inserting new element
    std::cout << "Modifying copy by inserting 60...\n";
    copy.insert(60);

    // Step 11: Print both trees after modification
    printAvl(original, "original (should be unchanged):");
    printAvl(copy, "copy (modified):");

    // Step 12: Verify copy has new element
    check_eq(copy.size(), (std::size_t)6, "copy size is 6 after insert");

    // Step 13: Verify original unchanged
    check_eq(original.size(), (std::size_t)5, "original size still 5");
    check(copy.search(60), "copy contains 60");
    check(!original.search(60), "original does not contain 60");

    // Step 14: Check both maintain invariants
    check(original.checkInvariant(), "original: invariant OK");
    check(copy.checkInvariant(), "copy: invariant OK");
}

// ======================= TEST 14: Assignment Operator =======================

static void test_assignment_operator()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 14: Assignment operator -------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create first tree with 3 elements
    std::cout << "Creating tree1 with 10, 5, 15...\n";
    AvlTree<int> tree1;
    tree1.insert(10);
    tree1.insert(5);
    tree1.insert(15);

    // Step 3: Print tree1
    printAvl(tree1, "tree1:");

    // Step 4: Create second tree with different elements
    std::cout << "Creating tree2 with 100, 200...\n";
    AvlTree<int> tree2;
    tree2.insert(100);
    tree2.insert(200);

    // Step 5: Print tree2 before assignment
    printAvl(tree2, "tree2 before assignment:");

    // Step 6: Assign tree1 to tree2
    std::cout << "Assigning: tree2 = tree1...\n";
    tree2 = tree1;

    // Step 7: Print both trees after assignment
    printAvl(tree1, "tree1:");
    printAvl(tree2, "tree2 after assignment:");

    // Step 8: Verify tree2 has tree1's elements
    check_eq(tree2.size(), (std::size_t)3, "tree2 size is 3");
    check(tree2.search(10), "tree2 contains 10");
    check(tree2.search(5), "tree2 contains 5");
    check(tree2.search(15), "tree2 contains 15");

    // Step 9: Verify tree2 lost old elements
    check(!tree2.search(100), "tree2 no longer contains 100");
    check(!tree2.search(200), "tree2 no longer contains 200");

    // Step 10: Verify tree1 unchanged
    check_eq(tree1.size(), (std::size_t)3, "tree1 size unchanged");

    // Step 11: Modify tree2 by inserting new element
    std::cout << "Modifying tree2 by inserting 20...\n";
    tree2.insert(20);

    // Step 12: Print both trees
    printAvl(tree1, "tree1 (should be unchanged):");
    printAvl(tree2, "tree2 (modified):");

    // Step 13: Verify tree2 modified but tree1 unchanged
    check_eq(tree2.size(), (std::size_t)4, "tree2 size is 4");
    check_eq(tree1.size(), (std::size_t)3, "tree1 size still 3");
    check(!tree1.search(20), "tree1 does not contain 20");
    check(tree2.search(20), "tree2 contains 20");

    // Step 14: Check invariants
    check(tree1.checkInvariant(), "tree1: invariant OK");
    check(tree2.checkInvariant(), "tree2: invariant OK");
}

// ======================= TEST 15: Clear =======================

static void test_clear()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 15: Clear operation -----------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create tree with multiple elements
    std::cout << "Creating AVL tree with 50, 25, 75, 10, 30, 60, 90...\n";
    AvlTree<int> tree;
    tree.insert(50);
    tree.insert(25);
    tree.insert(75);
    tree.insert(10);
    tree.insert(30);
    tree.insert(60);
    tree.insert(90);

    // Step 3: Print tree before clear
    printAvl(tree, "tree:");
    check_eq(tree.size(), (std::size_t)7, "initial size is 7");

    // Step 4: Call clear
    std::cout << "Calling clear()...\n";
    tree.clear();

    // Step 5: Print tree after clear
    printAvl(tree, "tree after clear:");

    // Step 6: Verify tree is empty
    check_eq(tree.size(), (std::size_t)0, "size is 0 after clear");
    check(tree.empty(), "tree is empty after clear");

    // Step 7: Verify elements are gone
    check(!tree.search(50), "does not contain 50");
    check(!tree.search(25), "does not contain 25");

    // Step 8: Check invariant
    check(tree.checkInvariant(), "invariant OK after clear");

    // Step 9: Verify tree can be reused
    std::cout << "Inserting 100 into cleared tree...\n";
    tree.insert(100);
    printAvl(tree, "tree after insert 100:");
    check_eq(tree.size(), (std::size_t)1, "size is 1 after insert");
    check(tree.search(100), "contains 100");
}

// ======================= TEST 16: Large Tree =======================

static void test_large_tree()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 16: Large AVL tree (100 elements) ---------";
    std::cout << "\n====================================================\n";

    // Step 2: Create tree and insert 100 sequential elements
    std::cout << "Inserting 100 elements (1-100)...\n";
    AvlTree<int> tree;
    const int N = 100;
    for (int i = 1; i <= N; ++i)
    {
        tree.insert(i);
    }

    // Step 3: Print tree status
    std::cout << "tree: size=" << tree.size()
              << " invariant=" << (tree.checkInvariant() ? "OK" : "BROKEN") << "\n";

    // Step 4: Check size
    check_eq(tree.size(), (std::size_t)N, "size is 100");

    // Step 5: Verify all elements present
    std::cout << "Checking all elements present...\n";
    bool all_present = true;
    for (int i = 1; i <= N; ++i)
    {
        if (!tree.search(i))
        {
            all_present = false;
            break;
        }
    }
    check(all_present, "all 100 elements present");

    // Step 6: Check non-existent elements
    std::cout << "Checking non-existent elements...\n";
    check(!tree.search(0), "does not contain 0");
    check(!tree.search(101), "does not contain 101");
    check(!tree.search(500), "does not contain 500");

    // Step 7: Check invariant (AVL maintains balance)
    check(tree.checkInvariant(), "large AVL tree: invariant OK");

    // Step 8: Note about AVL benefit
    std::cout << "Note: AVL tree keeps 100 sequential inserts balanced!\n";
}

// ======================= TEST 17: Date Objects =======================

static void test_with_date_objects()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 17: Date objects (actual use case) --------";
    std::cout << "\n====================================================\n";

    // Step 2: Create Date objects
    std::cout << "Creating Date objects and inserting into AVL tree...\n";
    AvlTree<Date> tree;
    Date d1(1, 1, 2024);
    Date d2(15, 6, 2023);
    Date d3(25, 12, 2024);
    Date d4(10, 3, 2024);

    // Step 3: Insert dates into tree
    std::cout << "Inserting dates: 1/1/2024, 15/6/2023, 25/12/2024, 10/3/2024...\n";
    tree.insert(d1);
    tree.insert(d2);
    tree.insert(d3);
    tree.insert(d4);

    // Step 4: Print tree status
    std::cout << "tree: size=" << tree.size()
              << " invariant=" << (tree.checkInvariant() ? "OK" : "BROKEN") << "\n";

    // Step 5: Verify size
    check_eq(tree.size(), (std::size_t)4, "Date tree size is 4");

    // Step 6: Verify all dates present
    check(tree.search(d1), "contains d1");
    check(tree.search(d2), "contains d2");
    check(tree.search(d3), "contains d3");
    check(tree.search(d4), "contains d4");

    // Step 7: Check invariant
    check(tree.checkInvariant(), "Date AVL tree: invariant OK");

    // Step 8: Test copy with Date objects
    std::cout << "Testing copy with Date objects...\n";
    AvlTree<Date> copy = tree;

    // Step 9: Print copy status
    std::cout << "copy: size=" << copy.size()
              << " invariant=" << (copy.checkInvariant() ? "OK" : "BROKEN") << "\n";

    // Step 10: Verify copy
    check_eq(copy.size(), (std::size_t)4, "Date copy size is 4");
    check(copy.search(d1), "copy contains d1");
    check(copy.checkInvariant(), "Date copy: invariant OK");
}

// ======================= MAIN TEST DRIVER =========================

// Main entry point - runs all AVL tree tests
int main()
{
    // Step 1: Print test suite header
    std::cout << "AVL TREE Unit Tests (1=PASS, 0=FAIL)\n";
    std::cout << "Testing self-balancing binary search tree implementation\n";

    // Step 2: Run basic functionality tests
    test_empty_tree();
    test_single_element();
    test_multiple_elements();
    test_duplicates();

    // Step 3: Run AVL-specific rotation tests
    test_right_rotation_LL();
    test_left_rotation_RR();
    test_left_right_rotation_LR();
    test_right_left_rotation_RL();

    // Step 4: Run AVL stress tests
    test_sequential_ascending();
    test_sequential_descending();

    // Step 5: Run function pointer tests
    test_function_pointer_apply();
    test_function_pointer_if();

    // Step 6: Run standard functionality tests
    test_copy_constructor();
    test_assignment_operator();
    test_clear();
    test_large_tree();
    test_with_date_objects();

    // Step 7: Print test summary
    std::cout << "\n====================================================\n";
    std::cout << "TOTAL PASSED: " << g_pass << "\n";
    std::cout << "TOTAL FAILED: " << g_fail << "\n";
    std::cout << "====================================================\n";

    // Step 8: Print completion message based on results
    if (g_fail == 0)
    {
        std::cout << "\n ALL TESTS PASSED! AVL tree is working correctly.\n";
    }
    else
    {
        std::cout << "\n SOME TESTS FAILED! Check implementation.\n";
    }

    // Step 9: Return exit code
    return (g_fail == 0) ? 0 : 1;
}
