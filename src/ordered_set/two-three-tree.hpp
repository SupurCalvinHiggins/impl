#include <array>
#include <cstddef>
#include <cstdint>

class TwoThreeTree {
    using size_type = size_t;
    using key_type = uint64_t;

    enum InsertionType {
        N2,
        N3P2,
        N3P3
    };

    struct Node {
        std::array<Node*, 4> children;
        std::array<key_type, 3> keys;
        size_type size;
    };

    void insert_two_node(Node* root, size_type child, key_type ) {

    }

    void insert_n3p2_fixup(Node* root, size_type child) {
        
    }

    void three_node_three_parent_insert(Node* root, size_type child) {

    }
};