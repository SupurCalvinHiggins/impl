#include <algorithm>
#include <iostream>
#include <cassert>

class AVLTree {

public:
    using key_type = uint64_t;
    using size_type = size_t;

private:
    struct Node {
        key_type key;
        Node* left;
        Node* right;
        size_type height;

        Node(key_type key) 
            : key(key), left(nullptr), right(nullptr), height(1) {}
    };

    using node_value = Node;
    using node_ptr = node_value*;

    node_ptr m_root;
    size_type m_size;

    bool contains(node_ptr root, key_type key) const {
        if (root == nullptr) {
            return false;
        }

        if (key == root->key) {
            return true;
        }

        if (key < root->key) {
            return contains(root->left, key);
        } else {
            return contains(root->right, key);
        }
    }

    node_ptr predecessor(node_ptr root, key_type key) const {
        if (root == nullptr) {
            return nullptr;
        }

        // If the key exists and has a left subtree, the predecessor is there.
        if (key == root->key && root->left != nullptr) {
            auto pred = root->left;
            while (pred->right != nullptr) {
                pred = pred->right;
            }
            return pred;
        }

        // Otherwise, the predecessor is the maximum parent less than the key.
        if (key <= root->key) {
            return predecessor(root->left, key);
        }

        auto pred = predecessor(root->right, key);
        return pred == nullptr || root->key > pred->key ? root : pred;
    }

    node_ptr successor(node_ptr root, key_type key) const {
        if (root == nullptr) {
            return nullptr;
        }

        // If the key exists and has a right subtree, the successor is there.
        if (key == root->key && root->right != nullptr) {
            auto succ = root->right;
            while (succ->left != nullptr) {
                succ = succ->left;
            }
            return succ;
        }

        // Otherwise, the successor is the minimum parent greater than the key.
        if (key >= root->key) {
            return successor(root->right, key);
        }

        auto succ = successor(root->left, key);
        return succ == nullptr || root->key < succ->key ? root : succ;
    }

    node_ptr rotate_right(node_ptr root) {
        assert(root != nullptr);
        assert(root->left != nullptr);

        // Destructure.
        auto x = root->left;
        auto y = root;
        auto a = root->left->left;
        auto b = root->left->right;
        auto c = root->right;

        // Rotate.
        y->left = b;
        y->right = c;
        y->height = 1 + std::max(height(b), height(c));
        x->left = a;
        x->right = y;
        x->height = 1 + std::max(height(a), height(y));

        return x;
    }

    node_ptr rotate_left(node_ptr root) {
        assert(root != nullptr);
        assert(root->right != nullptr);

        // Destructure.
        auto x = root->right;
        auto y = root;
        auto a = root->left;
        auto b = root->right->left;
        auto c = root->right->right;

        // Rotate.
        y->left = a;
        y->right = b;
        y->height = 1 + std::max(height(a), height(b));
        x->left = y;
        x->right = c;
        x->height = 1 + std::max(height(y), height(c));

        return x;
    }

    size_type height(const node_ptr root) const {
        return root != nullptr ? root->height : 0;
    }

    node_ptr insert(node_ptr root, key_type key) {
        // If the root is empty, insert the key.
        if (root == nullptr) {
            m_size++;
            return new Node(key);
        }

        // Ignore double insertions.
        if (key == root->key) {
            return root;
        }

        // Insert into the correct subtree.
        if (key < root->key) {
            root->left = insert(root->left, key);
        } else {
            root->right = insert(root->right, key);
        }

        // The root's height might be incorrect now. Fix it.
        root->height = 1 + std::max(height(root->left), height(root->right));

        // If the left subtree is too high, fix it.
        if (height(root->left) > height(root->right) + 1) {
            // If the left right subtree is too high, make the left left subtree high instead.
            if (key > root->left->key) {
                root->left = rotate_left(root->left);
            }
            // Fix the high left left subtree.
            return rotate_right(root);
        }

        // If the right subtree is high, fix it.
        if (height(root->right) > height(root->left) + 1) {
            // If the right left subtree is too high, make the right right subtree high instead.
            if (key < root->right->key) {
                root->right = rotate_right(root->right);
            }
            // Fix the high right right subtree.
            return rotate_left(root);
        }

        return root;
    }
 
    node_ptr remove(node_ptr root, key_type key) {
        if (root == nullptr) {
            return nullptr;
        }

        if (key == root->key) {
            if (root->left == nullptr && root->right == nullptr) {
                m_size--;
                delete root;
                return nullptr;
            }
            else if (root->left == nullptr) {
                m_size--;
                auto right = root->right;
                delete root;
                root = right;
            }
            else if (root->right == nullptr) {
                m_size--;
                auto left = root->left;
                delete root;
                root = left;
            } else {
                auto succ = root->right;
                while (succ->left != nullptr) {
                    succ = succ->left;
                }
                std::swap(succ->key, root->key);
                root->right = remove(root->right, key);
            }            
        }

        if (key < root->key) {
            root->left = remove(root->left, key);
        } else {
            root->right = remove(root->right, key);
        }

        // Left-heavy. Remove in right.
        if (height(root->left) > height(root->right) + 1) {
            if (root->left && height(root->left->right) > height(root->left->left) + 1) {
                root->left = rotate_left(root->left);
            }
            return rotate_right(root);
        }

        // Right-heavy. Remove in left.
        if (height(root->right) > height(root->left) + 1) {
            if (root->right && height(root->right->left) > height(root->right->right) + 1) {
                root->right = rotate_right(root->right);
            }
            return rotate_left(root);
        }

        assert(std::abs((int)height(root->left) - (int)height(root->right)) <= 1);
        return root;
    }

    void print(node_ptr root, size_type depth) {
        if (root == nullptr) {
            for (size_type i = 0; i < depth; ++i) {
                std::cout << "\t";
            }
            std::cout << "NULL" << std::endl;
            return;
        }

        for (size_type i = 0; i < depth; ++i) {
            std::cout << "\t";
        }
        std::cout << root->key << std::endl;
        print(root->left, depth+1);
        print(root->right, depth+1);
    }

public:
    AVLTree() : m_root(nullptr), m_size(0) {}

    bool contains(key_type key) const {
        return contains(m_root, key);
    }

    std::optional<key_type> predecessor(key_type key) {
        auto pred = predecessor(m_root, key);
        if (pred == nullptr) {
            return std::nullopt;
        }
        return std::make_optional(pred->key);
    }

    std::optional<key_type> successor(key_type key) {
        auto succ = successor(m_root, key);
        if (succ == nullptr) {
            return std::nullopt;
        }
        return std::make_optional(succ->key);
    }

    size_type size() const {
        return m_size;
    }

    void insert(key_type key) {
        m_root = insert(m_root, key);
    }

    void remove(key_type key) {
        m_root = remove(m_root, key);
    }

    void print() {
        std::cout << "*** BEGIN TREE ***" << std::endl;
        print(m_root, 0);
        std::cout << "*** END TREE ***" << std::endl;
    }
};