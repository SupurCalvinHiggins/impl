#pragma once

#include <cstdint>
#include <array>
#include <cassert>
#include <cstddef>

#define B 4

class BTree {

public:
    using size_type = size_t;
    using key_type = uint64_t;

private:

    struct Node {
        std::array<Node*, B+1> children;
        std::array<key_type, B> keys;
        size_type size;

        Node() {
            for (size_type i = 0; i < B; ++i) {
                keys.at(i) = 0;
            }

            for (size_type i = 0; i < B + 1; ++i) {
                children.at(i) = nullptr;
            }

            size = 0;
        }
    };

    Node* m_root;
    size_type m_size;

private:

    bool split(Node* root, size_type i) {
        auto child = root->children.at(i);
        assert(child->size == B);

        auto m = B / 2;
        auto median = child->keys.at(m);

        // Create the left node.
        auto left = new Node();
        for (size_type j = 0; j < m; ++j) {
            left->keys.at(j) = child->keys.at(j);
            left->children.at(j) = child->children.at(j);
        }
        left->children.at(m) = child->children.at(m);
        left->size = m;

        // Create the right node.
        auto right = new Node();
        for (size_type j = m+1; j < B; ++j) {
            right->keys.at(j-(m+1)) = child->keys.at(j);
            right->children.at(j-(m+1)) = child->children.at(j);
        }
        right->children.at(B-(m+1)) = child->children.at(B);
        right->size = m-1;

        // Create a gap for the median
        root->children.at(root->size+1) = root->children.at(root->size);
        for (int j = root->size-1; j >= (int)i; --j) {
            root->keys.at(j+1) = root->keys.at(j);
            root->children.at(j+1) = root->children.at(j);
        }

        // And insert the median.
        root->keys.at(i) = median;
        root->children.at(i) = left;
        root->children.at(i+1) = right;
        ++(root->size);
        return root->size == B;
    }

    bool insert(Node* root, key_type key) {
        // If the node is a leaf, insert the key in sorted order.
        if (root->children.at(0) == nullptr) {
            // Find where the key will be inserted.
            size_type i = 0;
            while (i < root->size && root->keys.at(i) < key) ++i;

            // If the key already exists, do not insert it.
            if (i < root->size && root->keys.at(i) == key) {
                return false;
            }

            // Otherwise, create a gap for the new key.
            for (int j = root->size - 1; j >= (int)i; --j) {
                root->keys.at(j+1) = root->keys.at(j);
            }

            // And insert the new key.
            root->keys.at(i) = key;
            ++(root->size);

            // Return if the node is full.
            return root->size == B;
        }

        // Otherwise, find the child to recurse on.
        size_type i = 0;
        while (i < root->size && root->keys.at(i) < key) ++i;
        
        // If the key already exists, stop inserting.
        if (i < root->size && root->keys.at(i) == key) {
            return false;
        }

        // Otherwise, insert the key and split if required.
        bool full = insert(root->children.at(i), key);
        if (!full) return false;
        return split(root, i);
    }

    bool contains(Node* root, key_type key) {
        if (root == nullptr) {
            return false;
        }

        size_type i = 0;
        while (i < root->size && root->keys.at(i) < key) ++i;
        if (i < root->size && root->keys.at(i) == key) {
            return true;
        }

        return contains(root->children.at(i), key);
    }

public:
    BTree() : m_root(new Node()), m_size(0) {}

    void insert(key_type key) {
        bool full = insert(m_root, key);
        if (!full) return;
        auto new_root = new Node();
        new_root->children.at(0) = m_root;
        split(new_root, 0);
        m_root = new_root;
    }

    void remove(key_type key) {

    }

    bool contains(key_type key) {
        return contains(m_root, key);
    }

    size_type size() {
        return m_size;
    }

    bool empty() {
        return size() == 0;
    }
};