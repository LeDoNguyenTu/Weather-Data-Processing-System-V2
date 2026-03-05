#ifndef AVL_TREE_H_INCLUDED
#define AVL_TREE_H_INCLUDED

#include <ostream>

//==================================DECLARATION PART=============================================

/**
 * @file AvlTree.h
 * @class AvlTree
 * @brief Template AVL Tree (self-balancing BST) with recursive operations and function pointer support.
 * @details A complete AVL tree implementation that maintains balance through rotations and
 *          provides O(log n) worst-case performance for insert, search, and traversal operations.
 *          All utilities are implemented manually without STL algorithms.
 *
 * Height Convention:
 * - Empty tree (nullptr) has height -1
 * - Leaf node has height 0
 * - Internal node height = 1 + max(height(left), height(right))
 *
 * Balance Maintenance:
 * AVL trees maintain balance by ensuring the height difference (balance factor) between
 * left and right subtrees of any node is at most 1 (range: -1 to +1).
 *
 * Features:
 * - Duplicate elements are ignored on insertion.
 * - Deep copy support via copy constructor and assignment operator.
 * - Multiple traversal methods: in-order, pre-order, post-order.
 * - Function pointer-based operations for flexible node processing.
 * - Invariant checking to verify AVL tree validity.
 *
 * @tparam T The type of values stored in the tree. T must support comparison operators
 *           less-than (`<`) and equality (`==`).
 */
template <class T>
class AvlTree
{
public:
    /**
     * @typedef NodeOperation
     * @brief Function pointer type for operations performed on node values.
     * @param [in] value The value of the current node being visited.
     */
    typedef void (*NodeOperation)(const T& value);

    /**
     * @typedef Predicate
     * @brief Function pointer type for conditional value testing.
     * @param [in] value The value to evaluate.
     * @return True if the value satisfies the condition, false otherwise.
     */
    typedef bool (*Predicate)(const T& value);

    /**
     * @brief Default constructor initializing an empty AVL tree.
     */
    AvlTree();

    /**
     * @brief Destructor recursively deleting all nodes in the tree.
     */
    ~AvlTree();

    /**
     * @brief Copy constructor creating a deep copy of another AVL tree.
     * @param [in] other The AVL tree to copy.
     */
    AvlTree(const AvlTree& other);

    /**
     * @brief Copy assignment operator replacing tree contents.
     * @param [in] other The AVL tree to assign from.
     * @return A reference to this AVL tree (allows chaining).
     */
    AvlTree& operator=(const AvlTree& other);

    /**
     * @brief Insert a value into the AVL tree.
     * @param [in] value The value to insert.
     * @details If the value already exists, insertion is ignored (no duplicates).
     *          The tree automatically rebalances to maintain AVL properties.
     */
    void insert(const T& value);

    /**
     * @brief Search for a value in the AVL tree.
     * @param [in] value The value to search for.
     * @return True if the value exists in the tree, false otherwise.
     */
    bool search(const T& value) const;

    /**
     * @brief Delete all nodes and reset the tree to empty state.
     */
    void clear();

    /**
     * @brief Perform in-order traversal and output elements.
     * @param [in,out] out The output stream to write to.
     * @details Visits nodes in sorted order (Left-Node-Right).
     */
    void inOrder(std::ostream& out) const;

    /**
     * @brief Perform pre-order traversal and output elements.
     * @param [in,out] out The output stream to write to.
     * @details Visits nodes with parent before children (Node-Left-Right).
     */
    void preOrder(std::ostream& out) const;

    /**
     * @brief Perform post-order traversal and output elements.
     * @param [in,out] out The output stream to write to.
     * @details Visits nodes with children before parent (Left-Right-Node).
     */
    void postOrder(std::ostream& out) const;

    /**
     * @brief Apply a function to each node during in-order traversal.
     * @param [in] func Function pointer to execute for each node's value.
     * @details Nodes are visited in sorted order (Left-Node-Right).
     */
    void inOrderApply(NodeOperation func) const;

    /**
     * @brief Apply a function to each node during pre-order traversal.
     * @param [in] func Function pointer to execute for each node's value.
     * @details Nodes are visited with parent before children (Node-Left-Right).
     */
    void preOrderApply(NodeOperation func) const;

    /**
     * @brief Apply a function to each node during post-order traversal.
     * @param [in] func Function pointer to execute for each node's value.
     * @details Nodes are visited with children before parent (Left-Right-Node).
     */
    void postOrderApply(NodeOperation func) const;

    /**
     * @brief Apply a function to nodes satisfying a predicate.
     * @param [in] predicate Function that returns true for nodes to process.
     * @param [in] action    Function to apply to matching nodes.
     * @details Traversal is performed in in-order (sorted order).
     */
    void inOrderIf(Predicate predicate, NodeOperation action) const;

    /**
     * @brief Get the number of elements in the AVL tree.
     * @return The total count of nodes in the tree.
     */
    std::size_t size() const noexcept;

    /**
     * @brief Check if the tree contains no elements.
     * @return True if the tree is empty, false otherwise.
     */
    bool empty() const noexcept;

    /**
     * @brief Verify that the tree satisfies all AVL invariants.
     * @return True if the tree is a valid AVL tree, false otherwise.
     * @details Checks BST property, AVL balance property, and height consistency.
     */
    bool checkInvariant() const;

private:
    struct Node
    {
        T key;          ///< Value stored in this node
        Node* left;     ///< Pointer to left subtree (values less than key)
        Node* right;    ///< Pointer to right subtree (values greater than key)
        int height;     ///< Height of this node (leaf = 0, empty = -1)

        /**
         * @brief Constructor initializing a new node with a value.
         * @param [in] value The value to store in this node.
         */
        explicit Node(const T& value)
            : key(value), left(nullptr), right(nullptr), height(0) {}
    };

    Node* root_;         ///< Pointer to root node of the tree
    std::size_t size_;   ///< Total count of nodes in the tree

    /**
     * @brief Create a deep copy of a subtree.
     * @param [in] node The root node of the subtree to clone.
     * @return Pointer to the newly cloned subtree.
     */
    static Node* clone(Node* node);

    /**
     * @brief Recursively delete all nodes in a subtree.
     * @param [in] node The root node of the subtree to destroy.
     */
    static void destroy(Node* node);

    /**
     * @brief Get the height of a node.
     * @param [in] node The node to query (nullptr returns -1).
     * @return The height of the node (-1 for empty, 0 for leaf).
     */
    static int getHeight(Node* node);

    /**
     * @brief Calculate the balance factor of a node.
     * @param [in] node The node to check.
     * @return Balance factor (left height - right height).
     */
    static int getBalance(Node* node);

    /**
     * @brief Return the maximum of two integers.
     * @param [in] a First integer.
     * @param [in] b Second integer.
     * @return The larger value.
     */
    static int max(int a, int b);

    /**
     * @brief Recalculate and update a node's height.
     * @param [in,out] node The node whose height to update.
     */
    static void updateHeight(Node* node);

    /**
     * @brief Perform a right rotation to fix left-heavy imbalance.
     * @param [in] y The node to rotate.
     * @return The new subtree root after rotation.
     */
    static Node* rotateRight(Node* y);

    /**
     * @brief Perform a left rotation to fix right-heavy imbalance.
     * @param [in] x The node to rotate.
     * @return The new subtree root after rotation.
     */
    static Node* rotateLeft(Node* x);

    /**
     * @brief Rebalance a node and its subtree using rotations if needed.
     * @param [in,out] node The node to rebalance.
     * @return The balanced subtree root.
     */
    static Node* rebalance(Node* node);

    /**
     * @brief Recursively insert a value into the tree and rebalance.
     * @param [in,out] node The root of the subtree to insert into.
     * @param [in] value The value to insert.
     * @param [out] inserted Flag set to true if insertion occurred.
     * @return The root of the rebalanced subtree.
     */
    static Node* insertRec(Node* node, const T& value, bool& inserted);

    /**
     * @brief Recursively search for a value in a subtree.
     * @param [in] node The root of the subtree to search.
     * @param [in] value The value to find.
     * @return True if value exists in subtree, false otherwise.
     */
    static bool containsRec(const Node* node, const T& value);

    /**
     * @brief Recursively perform in-order traversal (Left-Node-Right).
     * @param [in] node The root of the subtree to traverse.
     * @param [in,out] out Output stream to write to.
     */
    static void inOrderRec(const Node* node, std::ostream& out);

    /**
     * @brief Recursively perform pre-order traversal (Node-Left-Right).
     * @param [in] node The root of the subtree to traverse.
     * @param [in,out] out Output stream to write to.
     */
    static void preOrderRec(const Node* node, std::ostream& out);

    /**
     * @brief Recursively perform post-order traversal (Left-Right-Node).
     * @param [in] node The root of the subtree to traverse.
     * @param [in,out] out Output stream to write to.
     */
    static void postOrderRec(const Node* node, std::ostream& out);

    /**
     * @brief Apply function to each node in in-order traversal.
     * @param [in] node The root of the subtree to traverse.
     * @param [in] func Function to apply to each node's value.
     */
    static void inOrderApplyRec(const Node* node, NodeOperation func);

    /**
     * @brief Apply function to each node in pre-order traversal.
     * @param [in] node The root of the subtree to traverse.
     * @param [in] func Function to apply to each node's value.
     */
    static void preOrderApplyRec(const Node* node, NodeOperation func);

    /**
     * @brief Apply function to each node in post-order traversal.
     * @param [in] node The root of the subtree to traverse.
     * @param [in] func Function to apply to each node's value.
     */
    static void postOrderApplyRec(const Node* node, NodeOperation func);

    /**
     * @brief Apply function to nodes satisfying a predicate (in-order).
     * @param [in] node The root of the subtree to traverse.
     * @param [in] pred Predicate function to test values.
     * @param [in] action Function to apply to matching nodes.
     */
    static void inOrderIfRec(const Node* node, Predicate pred, NodeOperation action);

    /**
     * @brief Verify AVL tree invariants in a subtree.
     * @param [in] node The root of the subtree to check.
     * @param [in] lo Lower bound pointer (nullptr for no bound).
     * @param [in] hi Upper bound pointer (nullptr for no bound).
     * @param [out] height The computed height of this subtree.
     * @return True if subtree is a valid AVL tree, false otherwise.
     */
    static bool checkRec(const Node* node, const T* lo, const T* hi, int& height);
};

//==================================IMPLEMENTATION PART=============================================

template <class T>
inline AvlTree<T>::AvlTree() : root_(nullptr), size_(0)
{
    // Initialize empty tree with no root node and zero elements
}

template <class T>
inline AvlTree<T>::~AvlTree()
{
    // Step 1: Recursively destroy all nodes starting from root
    destroy(root_);
    // Step 2: Clear root pointer to nullptr
    root_ = nullptr;
    // Step 3: Reset size to zero
    size_ = 0;
}

template <class T>
inline typename AvlTree<T>::Node* AvlTree<T>::clone(Node* node)
{
    // Step 1: Base case - if node is null, return nullptr (empty subtree)
    if (!node) return nullptr;

    // Step 2: Create new node with same key value
    Node* newNode = new Node(node->key);

    // Step 3: Copy height from original node
    newNode->height = node->height;

    // Step 4: Recursively clone left subtree
    newNode->left = clone(node->left);

    // Step 5: Recursively clone right subtree
    newNode->right = clone(node->right);

    // Step 6: Return the newly cloned node
    return newNode;
}

template <class T>
inline AvlTree<T>::AvlTree(const AvlTree& other)
    : root_(clone(other.root_)), size_(other.size_)
{
    // Initialize this tree as a deep copy of other tree
    // Uses clone to recursively copy all nodes and size
}

template <class T>
inline AvlTree<T>& AvlTree<T>::operator=(const AvlTree& other)
{
    // Step 1: Check for self-assignment to avoid unnecessary work
    if (this == &other) return *this;

    // Step 2: Create temporary copy of other tree using copy constructor
    AvlTree tmp(other);

    // Step 3: Swap root pointers (this->root_ gets tmp's copy)
    Node* tempRoot = root_;
    root_ = tmp.root_;
    tmp.root_ = tempRoot;

    // Step 4: Swap size values
    std::size_t tempSize = size_;
    size_ = tmp.size_;
    tmp.size_ = tempSize;

    // Step 5: Return reference to this tree (tmp destructor cleans old content)
    return *this;
}

template <class T>
inline void AvlTree<T>::destroy(Node* node)
{
    // Step 1: Base case - if node is null, nothing to destroy
    if (!node) return;

    // Step 2: Recursively destroy left subtree
    destroy(node->left);

    // Step 3: Recursively destroy right subtree
    destroy(node->right);

    // Step 4: Delete current node
    delete node;
}

template <class T>
inline int AvlTree<T>::max(int a, int b)
{
    // Step 1: Return the larger of the two integers
    return (a > b) ? a : b;
}

template <class T>
inline int AvlTree<T>::getHeight(Node* node)
{
    // Step 1: If node is null, return -1 (empty tree height convention)
    return node ? node->height : -1;
}

template <class T>
inline int AvlTree<T>::getBalance(Node* node)
{
    // Step 1: If node is null, balance is 0
    // Step 2: Otherwise, calculate balance factor as left height minus right height
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

template <class T>
inline void AvlTree<T>::updateHeight(Node* node)
{
    // Step 1: If node is valid, recalculate its height
    if (node)
    {
        // Step 2: Height = 1 + maximum of left and right child heights
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    }
}

template <class T>
inline typename AvlTree<T>::Node* AvlTree<T>::rotateRight(Node* y)
{
    // Right rotation handles left-heavy imbalance
    // Before: y is parent, x is left child

    // Step 1: Save left child (x) which will become new parent
    Node* x = y->left;

    // Step 2: Save x's right child (T2) which will become y's new left child
    Node* T2 = x->right;

    // Step 3: Perform rotation - x becomes new parent of y
    x->right = y;

    // Step 4: T2 becomes y's new left child
    y->left = T2;

    // Step 5: Update y's height first (it's now a child)
    updateHeight(y);

    // Step 6: Update x's height second (it's now the parent)
    updateHeight(x);

    // Step 7: Return new subtree root (x)
    return x;
}

template <class T>
inline typename AvlTree<T>::Node* AvlTree<T>::rotateLeft(Node* x)
{
    // Left rotation handles right-heavy imbalance
    // Before: x is parent, y is right child

    // Step 1: Save right child (y) which will become new parent
    Node* y = x->right;

    // Step 2: Save y's left child (T2) which will become x's new right child
    Node* T2 = y->left;

    // Step 3: Perform rotation - y becomes new parent of x
    y->left = x;

    // Step 4: T2 becomes x's new right child
    x->right = T2;

    // Step 5: Update x's height first (it's now a child)
    updateHeight(x);

    // Step 6: Update y's height second (it's now the parent)
    updateHeight(y);

    // Step 7: Return new subtree root (y)
    return y;
}

template <class T>
inline typename AvlTree<T>::Node* AvlTree<T>::rebalance(Node* node)
{
    // Rebalance a node by checking balance factor and applying rotations if needed

    // Step 1: If node is null, nothing to rebalance
    if (!node) return node;

    // Step 2: Recalculate this node's height based on children
    updateHeight(node);

    // Step 3: Calculate balance factor (left height - right height)
    int balance = getBalance(node);

    // Step 4: Check left-heavy case (balance > 1)
    if (balance > 1)
    {
        // Step 5a: If left child is right-heavy, perform Left-Right case
        if (getBalance(node->left) < 0)
        {
            node->left = rotateLeft(node->left);
        }
        // Step 5b: Perform right rotation to fix Left-Left case (or converted Left-Right)
        return rotateRight(node);
    }

    // Step 6: Check right-heavy case (balance < -1)
    if (balance < -1)
    {
        // Step 7a: If right child is left-heavy, perform Right-Left case
        if (getBalance(node->right) > 0)
        {
            node->right = rotateRight(node->right);
        }
        // Step 7b: Perform left rotation to fix Right-Right case (or converted Right-Left)
        return rotateLeft(node);
    }

    // Step 8: Node is balanced, return unchanged
    return node;
}

template <class T>
inline typename AvlTree<T>::Node* AvlTree<T>::insertRec(Node* parent, const T& value, bool& inserted)
{
    // Recursively insert value into AVL tree and rebalance as needed

    // Step 1: Base case - if parent is null, create new leaf node
    if (!parent)
    {
        inserted = true;  // Mark that insertion occurred
        return new Node(value);
    }

    // Step 2: Check for duplicate - if value equals parent's key, skip insertion
    if (value == parent->key)
    {
        inserted = false;  // Duplicate found, no insertion
        return parent;
    }

    // Step 3: Determine which subtree to insert into
    if (value < parent->key)
    {
        // Step 4a: Value is less than parent, insert into left subtree
        parent->left = insertRec(parent->left, value, inserted);

        // Step 4b: If insertion succeeded, rebalance this subtree
        if (inserted)
        {
            parent = rebalance(parent);
        }
    }
    else
    {
        // Step 4c: Value is greater than parent, insert into right subtree
        parent->right = insertRec(parent->right, value, inserted);

        // Step 4d: If insertion succeeded, rebalance this subtree
        if (inserted)
        {
            parent = rebalance(parent);
        }
    }

    // Step 5: Return the (possibly rebalanced) subtree root
    return parent;
}

template <class T>
inline void AvlTree<T>::insert(const T& value)
{
    // Public interface for inserting a value into the AVL tree

    // Step 1: Initialize flag to track if insertion occurred
    bool inserted = false;

    // Step 2: Call recursive insert starting from root
    root_ = insertRec(root_, value, inserted);

    // Step 3: If insertion was successful, increment size counter
    if (inserted) ++size_;
}

template <class T>
inline bool AvlTree<T>::containsRec(const Node* node, const T& value)
{
    // Recursively search for value in subtree rooted at node

    // Step 1: Base case - if node is null, value not found
    if (!node) return false;

    // Step 2: Check if current node contains the value
    if (value == node->key) return true;

    // Step 3: If value is less than current node, search left subtree
    if (value < node->key) return containsRec(node->left, value);

    // Step 4: If value is greater than current node, search right subtree
    return containsRec(node->right, value);
}

template <class T>
inline bool AvlTree<T>::search(const T& value) const
{
    // Public interface for searching for a value in the tree
    return containsRec(root_, value);
}

template <class T>
inline void AvlTree<T>::clear()
{
    // Step 1: Recursively destroy all nodes starting from root
    destroy(root_);

    // Step 2: Clear root pointer
    root_ = nullptr;

    // Step 3: Reset size to zero
    size_ = 0;
}

template <class T>
inline void AvlTree<T>::inOrderRec(const Node* node, std::ostream& out)
{
    // In-order traversal: Left - Node - Right (produces sorted output)

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Process left subtree
    inOrderRec(node->left, out);

    // Step 3: Visit current node (output its value)
    out << node->key << ' ';

    // Step 4: Process right subtree
    inOrderRec(node->right, out);
}

template <class T>
inline void AvlTree<T>::preOrderRec(const Node* node, std::ostream& out)
{
    // Pre-order traversal: Node - Left - Right (parent before children)

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Visit current node first (output its value)
    out << node->key << ' ';

    // Step 3: Process left subtree
    preOrderRec(node->left, out);

    // Step 4: Process right subtree
    preOrderRec(node->right, out);
}

template <class T>
inline void AvlTree<T>::postOrderRec(const Node* node, std::ostream& out)
{
    // Post-order traversal: Left - Right - Node (children before parent)

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Process left subtree
    postOrderRec(node->left, out);

    // Step 3: Process right subtree
    postOrderRec(node->right, out);

    // Step 4: Visit current node (output its value)
    out << node->key << ' ';
}

template <class T>
inline void AvlTree<T>::inOrder(std::ostream& out) const
{
    // Public interface for in-order traversal
    inOrderRec(root_, out);
}

template <class T>
inline void AvlTree<T>::preOrder(std::ostream& out) const
{
    // Public interface for pre-order traversal
    preOrderRec(root_, out);
}

template <class T>
inline void AvlTree<T>::postOrder(std::ostream& out) const
{
    // Public interface for post-order traversal
    postOrderRec(root_, out);
}

template <class T>
inline void AvlTree<T>::inOrderApplyRec(const Node* node, NodeOperation func)
{
    // In-order traversal applying function to each node: Left - Node - Right

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Process left subtree
    inOrderApplyRec(node->left, func);

    // Step 3: Apply function to current node's value
    func(node->key);

    // Step 4: Process right subtree
    inOrderApplyRec(node->right, func);
}

template <class T>
inline void AvlTree<T>::preOrderApplyRec(const Node* node, NodeOperation func)
{
    // Pre-order traversal applying function to each node: Node - Left - Right

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Apply function to current node first
    func(node->key);

    // Step 3: Process left subtree
    preOrderApplyRec(node->left, func);

    // Step 4: Process right subtree
    preOrderApplyRec(node->right, func);
}

template <class T>
inline void AvlTree<T>::postOrderApplyRec(const Node* node, NodeOperation func)
{
    // Post-order traversal applying function to each node: Left - Right - Node

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Process left subtree
    postOrderApplyRec(node->left, func);

    // Step 3: Process right subtree
    postOrderApplyRec(node->right, func);

    // Step 4: Apply function to current node
    func(node->key);
}

template <class T>
inline void AvlTree<T>::inOrderApply(NodeOperation func) const
{
    // Public interface for in-order traversal with function application
    inOrderApplyRec(root_, func);
}

template <class T>
inline void AvlTree<T>::preOrderApply(NodeOperation func) const
{
    // Public interface for pre-order traversal with function application
    preOrderApplyRec(root_, func);
}

template <class T>
inline void AvlTree<T>::postOrderApply(NodeOperation func) const
{
    // Public interface for post-order traversal with function application
    postOrderApplyRec(root_, func);
}

template <class T>
inline void AvlTree<T>::inOrderIfRec(const Node* node, Predicate pred, NodeOperation action)
{
    // In-order traversal applying action only to nodes satisfying predicate

    // Step 1: Base case - if node is null, return
    if (!node) return;

    // Step 2: Process left subtree
    inOrderIfRec(node->left, pred, action);

    // Step 3: Test if current node's value satisfies predicate
    if (pred(node->key))
    {
        // Step 4a: If predicate is true, apply action to this node's value
        action(node->key);
    }

    // Step 4b: Process right subtree
    inOrderIfRec(node->right, pred, action);
}

template <class T>
inline void AvlTree<T>::inOrderIf(Predicate predicate, NodeOperation action) const
{
    // Public interface for conditional in-order traversal
    inOrderIfRec(root_, predicate, action);
}

template <class T>
inline std::size_t AvlTree<T>::size() const noexcept
{
    // Return the cached size counter (O(1) operation)
    return size_;
}

template <class T>
inline bool AvlTree<T>::empty() const noexcept
{
    // Return true if size is zero (tree is empty)
    return size_ == 0;
}

template <class T>
inline bool AvlTree<T>::checkRec(const Node* node, const T* lo, const T* hi, int& height)
{
    // Recursively verify AVL tree invariants:
    // 1. BST property (values in valid range)
    // 2. AVL balance property (balance factor in [-1, 1])
    // 3. Height consistency (height = 1 + max(left, right) height)

    // Step 1: Base case - if node is null, it's valid with height -1
    if (!node)
    {
        height = -1;  // Empty tree has height -1 by convention
        return true;
    }

    // Step 2: Verify BST lower bound - if lo is set, node's key must be > lo
    if (lo && !(*lo < node->key)) return false;

    // Step 3: Verify BST upper bound - if hi is set, node's key must be < hi
    if (hi && !(node->key < *hi)) return false;

    // Step 4: Recursively check left subtree with updated upper bound
    int leftHeight = -1, rightHeight = -1;
    if (!checkRec(node->left, lo, &node->key, leftHeight)) return false;

    // Step 5: Recursively check right subtree with updated lower bound
    if (!checkRec(node->right, &node->key, hi, rightHeight)) return false;

    // Step 6: Check AVL balance property - balance factor must be in [-1, 1]
    int balance = leftHeight - rightHeight;
    if (balance < -1 || balance > 1) return false;

    // Step 7: Verify height is correctly computed
    int computedHeight = 1 + max(leftHeight, rightHeight);
    if (node->height != computedHeight) return false;

    // Step 8: Set output parameter to current node's height
    height = computedHeight;

    // Step 9: All checks passed, return true
    return true;
}

template <class T>
inline bool AvlTree<T>::checkInvariant() const
{
    // Public interface to check if tree satisfies all AVL invariants

    // Step 1: Initialize height variable
    int height = -1;

    // Step 2: Call recursive check starting from root with no bounds
    return checkRec(root_, nullptr, nullptr, height);
}

#endif // AVL_TREE_H_INCLUDED
