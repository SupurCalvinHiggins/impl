#pragma once

#include <cstdint>
#include <array>
#include <cassert>
#include <cstddef>

#define B 4

// invariants: besides root, the numbers of keys in node
// is between B/2 - 1 and B-1

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
            ++m_size;

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

    bool remove(Node* root, key_type key) {
        // Otherwise, find the child to recurse on.
        size_type i = 0;
        while (i < root->size && root->keys.at(i) < key) ++i;
        
        // If the key already exists.
        if (i < root->size && root->keys.at(i) == key) {
            // Remove it if it's in a leaf.
            if (root->children[0] == nullptr) {
                for (size_type j = i; j < root->size; ++j) {
                    root->keys[j] = root->keys[j+1];
                }
                root->keys[root->size-1] = 0;
                --(root->size);
                --m_size;
                return root->size < (B/2 - 1);
            }

            // Otherwise, swap with its succ.
            auto node = root->children[i+1];
            while (node->children[0] != nullptr) {
                node = node->children[0];
            }
            std::swap(node->keys[0], key);
        } else {
            // The key does not exist.
            if (root->children[0] == nullptr) {
                return false;
            }
            
        }
        // search and recurse
        bool empty = remove(root->children[i], key);
        if (!empty) return false;

        // if can take from sibling nodes to fill, do it.
        if (i > 0 && root->children[i-1]->size > (B/2 - 1)) {
            std::cout << "left" << std::endl;
            // Shift over.
            root->children[i]->children[root->children[i]->size+1] = root->children[i]->children[root->children[i]->size];
            for (size_type j = root->children[i]->size; j > 0; --j) {
                root->children[i]->keys[j] = root->children[i]->keys[j-1];
                root->children[i]->children[j] = root->children[i]->children[j-1];
            }

            // Move down.
            root->children[i]->keys[0] = root->keys[i];
            ++(root->children[i]->size);

            // Move up.
            root->keys[i] = root->children[i-1]->keys[root->children[i-1]->size-1];
            root->children[i-1]->keys[root->children[i-1]->size-1] = 0;
            --(root->children[i-1]->size);

            return false;

        } else if (i < root->size && root->children[i+1]->size > (B/2 - 1)) {
            std::cout << "right " << root->children[i+1]->size << std::endl;
            // Move down.
            root->children[i]->keys[root->children[i]->size] = root->keys[i];
            ++(root->children[i]->size);

            // Move up.
            root->keys[i] = root->children[i+1]->keys[0];

            // Shift over.
            for (size_type j = 1; j < root->children[i+1]->size; ++j) {
                root->children[i+1]->keys[j-1] = root->children[i+1]->keys[j];
                root->children[i+1]->children[j-1] = root->children[i+1]->children[j];
            }
            root->children[i+1]->children[root->children[i+1]->size-1] = root->children[i+1]->children[root->children[i+1]->size];
            root->children[i+1]->keys[root->children[i+1]->size-1] = 0;
            root->children[i+1]->children[root->children[i+1]->size] = nullptr;
            --(root->children[i+1]->size);

            return false;
        }

        std::cout << "merge" << std::endl;
        // if cant take from siblings to fill, merge them (i) and (i+1) unless i is max in which case its i-1
        size_type l, r;
        if (i == root->size) {
            l = i-1;
            r = i;
        } else {
            l = i;
            r = i+1;
        }

        // build merged node
        auto merged = new Node();
        for (size_type j = 0; j < root->children[l]->size; ++j) {
            merged->keys[j] = root->children[l]->keys[j];
            merged->children[j] = root->children[l]->children[j];
        }
        merged->keys[root->children[l]->size] = root->keys[l];
        merged->children[root->children[l]->size] = root->children[l]->children[root->children[l]->size];
        merged->size = root->children[l]->size+1;

        for(size_type j = 0; j < root->children[r]->size; ++j) {
            merged->keys[j+merged->size] = root->children[r]->keys[j];
            merged->children[j+merged->size] = root->children[r]->children[j];
        }
        merged->children[root->children[r]->size + merged->size] = root->children[r]->children[root->children[r]->size];
        merged->size += root->children[r]->size;
        
        // free memory
        delete root->children[i];
        delete root->children[i+1];

        // insert merged node into parent
        for (size_type j = i; j < root->size; ++j) {
            root->keys[j] = root->keys[j+1];
            root->children[j] = root->children[j+1];
        }
        root->children[root->size-1] = root->children[root->size];
        root->keys[root->size-1] = 0;
        root->children[root->size] = nullptr;
        --(root->size);

        // link in the merged node
        // TODO: instead of this, copy merged into the current root here.
        if (root->size == 0 && root == m_root) {
            m_root = merged;
            return false;
        }
        // if (root->size == 0) {
        //     root->size = merged->size;
        //     root->children = merged->children;
        //     root->keys = merged->keys;
        //     delete merged;
        //     return false;
        // }

        root->children[i] = merged;
        print(m_root, 0);
        std::cout << "end merge" << std::endl;
        return root->size < (B/2 - 1);
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
        remove(m_root, key);
        // cases:
        // if not leaf -> put in leaf by switching with successor
        // remove from leaf -> merge with sibling if too small

        // to fixup try to pull keys from siblings to maintain >= b/2-1
        // if not possible, merge the nodes together (this impacts parent)
    }

    bool contains(key_type key) {
        return contains(m_root, key);
    }

    size_type size() {
        return m_size;
    }

    void print() { 
        std::cout << "*** TREE ***" << std::endl;
        print(m_root, 0);
        std::cout << "*** END TREE ***" << std::endl;
        std::cout << std::endl;
    }
private:
    void print(Node* root, int depth) {
        if (root == nullptr) return;
        for (int j = 0; j < root->size; ++j) {
            print(root->children[j], depth+1);
            for (int i = 0; i < depth; ++i) {
                std::cout << "\t";
            }
            std::cout << root->keys[j] << std::endl;

            for (int i = root->size; i < B; ++i) {
                assert(root->keys[i] == 0);
            }

            for (int i = root->size+1; i < B+1; ++i) {
                assert(root->children[B] == nullptr);
            }
        }
        print(root->children[root->size], depth+1);
    }
};