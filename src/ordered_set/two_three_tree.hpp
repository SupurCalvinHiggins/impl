#include <array>
#include <optional>
#include <cinttypes>
#include <cassert>

class TwoThreeTree {
public:
    using size_type = size_t;
    using key_type = uint64_t;

private:
    struct Node {
        std::array<key_type, 2> keys;
        std::array<Node*, 3> children;
        size_type size;
    };

    using node_value = Node;
    using node_ptr = Node*;

    node_ptr m_root;
    size_type m_size;

    node_ptr rotate_left(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        assert(pivot < root->size);
        
        // Destructure root.
        auto l = root->children[pivot];
        auto r = root->children[pivot + 1];
        auto x = root->keys[pivot];

        assert(l != nullptr);
        assert(r != nullptr);
        assert(l->size == 0);
        assert(r->size == 2);

        // Destructure left and right.
        auto y = r->keys[0];
        auto z = r->keys[1];
        auto a = l->children[0];
        auto b = r->children[0];
        auto c = r->children[1];
        auto d = r->children[2];

        // Rotate.
        l->keys[0] = x;
        l->keys[1] = 0;
        l->children[0] = a;
        l->children[1] = b;
        l->children[2] = 0;
        l->size = 1;

        r->keys[0] = z;
        r->keys[1] = 0;
        r->children[0] = c;
        r->children[1] = d;
        r->children[2] = nullptr;
        r->size = 1;

        root->keys[pivot] = y;
        return root;
    }

    node_ptr rotate_right(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        assert(pivot < root->size);

        // Destructure root.
        auto l = root->children[pivot];
        auto r = root->children[pivot+1];
        auto z = root->keys[pivot];

        assert(l != nullptr);
        assert(r != nullptr);
        assert(l->size == 2);
        assert(r->size == 0);

        // Destructure left and right.
        auto x = l->keys[0];
        auto y = l->keys[1];
        auto a = l->children[0];
        auto b = l->children[1];
        auto c = l->children[2];
        auto d = r->children[0];

        // Rotate.
        l->keys[0] = x;
        l->keys[1] = 0;
        l->children[0] = a;
        l->children[1] = b;
        l->children[2] = nullptr;
        l->size = 1;

        r->keys[0] = z;
        r->keys[1] = 0;
        r->children[0] = c;
        r->children[1] = d;
        r->children[2] = nullptr;
        r->size = 1;

        root->keys[pivot] = y;
        return root;
    }

    node_ptr merge(node_ptr root, size_type pivot) const {
        
    }

public:
    TwoThreeTree() {}

    bool contains(key_type key) const {
        return false;
    }

    std::optional<key_type> predecessor(key_type key) const {
        return std::nullopt;
    }

    std::optional<key_type> successor(key_type key) const {
        return std::nullopt;
    }

    size_type size() const {
        return m_size;
    }

    void insert(key_type key) {
        
    }

    void remove(key_type key) {

    }
};