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

        Node(std::array<key_type, 2> keys, std::array<Node*, 3> children, size_type size)
            : keys(keys), children(children), size(size) { }
    };

    using node_value = Node;
    using node_ptr = Node*;

    node_ptr m_root;
    size_type m_size;

    bool contains(node_ptr root, key_type key) const {
        if (root == nullptr) {
            return false;
        }

        if (root->size == 1) {
            if (key == root->keys[0]) {
                return true;
            }
            if (key < root->keys[0]) {
                return contains(root->children[0], key);
            }
            return contains(root->children[1], key);
        }

        else {
            if (key == root->keys[0] || key == root->keys[1]) {
                return true;
            }
            if (key < root->keys[0]) {
                return contains(root->children[0], key);
            }
            if (key < root->keys[1]) {
                return contains(root->children[1], key);
            }
            return contains(root->children[2], key);
        }
    }

    node_ptr rotate_left(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        assert(pivot < root->size);
        
        // Destructure root.
        auto x = root->keys[pivot];
        auto l = root->children[pivot];
        auto r = root->children[pivot+1];

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
        auto z = root->keys[pivot];
        auto l = root->children[pivot];
        // TODO: either for left or right this should be -1
        auto r = root->children[pivot+1];

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

    node_ptr merge_left(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        assert(pivot < root->size);

        // Destructure root.
        auto x = root->keys[pivot];
        auto l = root->children[pivot];
        auto r = root->children[pivot+1];

        assert(l != nullptr);
        assert(r != nullptr);
        assert(l->size == 0);
        assert(r->size == 1);

        // Destructure left and right.
        auto y = r->keys[0];
        auto a = l->children[0];
        auto b = r->children[0];
        auto c = r->children[1];

        // Merge.
        l->keys[0] = x;
        l->keys[1] = y;
        l->children[0] = a;
        l->children[1] = b;
        l->children[2] = c;
        l->size = 2;

        delete r;

        root->children[pivot] = l;
        root->children[pivot+1] = nullptr;
        root->keys[pivot] = 0;

        // Compaction.
        if (root->size == 2 && pivot == 0) {
            std::swap(root->children[pivot+1], root->children[pivot+2]);
            std::swap(root->keys[pivot], root->keys[pivot+1]);
        }
        root->size--;
        return root;
    }

    node_ptr merge_right(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        // TODO: called with pivot as children idx but this assumes pivot is keys idx.
        assert(pivot < root->size);

        // Destructure root.
        auto y = root->keys[pivot];
        auto l = root->children[pivot];
        auto r = root->children[pivot+1];

        assert(l != nullptr);
        assert(r != nullptr);
        assert(l->size == 1);
        assert(r->size == 0);

        // Destructure left and right.
        auto x = l->keys[0];
        auto a = l->children[0];
        auto b = l->children[1];
        auto c = r->children[2];

        // Merge.
        l->keys[0] = x;
        l->keys[1] = y;
        l->children[0] = a;
        l->children[1] = b;
        l->children[2] = c;
        l->size = 2;

        delete r;

        root->children[pivot] = l;
        root->children[pivot+1] = nullptr;
        root->keys[pivot] = 0;

        // Compaction.
        if (root->size == 2 && pivot == 0) {
            std::swap(root->children[pivot+1], root->children[pivot+2]);
            std::swap(root->keys[pivot], root->keys[pivot+1]);
        }
        root->size--;
        return root;
    }

    node_ptr split(node_ptr root, size_type pivot) {
        assert(root->size == 2);

        // Destructure root.
        auto kick = root->children[pivot];
        assert(kick->size == 0);

        // Destructure kick.
        node_ptr a, b, c, d;
        key_type x, y, z;

        if (pivot == 0) {
            x = kick->keys[0];
            y = root->keys[0];
            z = root->keys[1];
            a = kick->children[0];
            b = kick->children[1];
            c = root->children[1];
            d = root->children[2];
        } else if (pivot == 1) {
            x = root->keys[0];
            y = kick->keys[0];
            z = root->keys[1];
            a = root->children[0];
            b = kick->children[0];
            c = kick->children[1];
            d = root->children[2];
        } else {
            x = root->keys[0];
            y = root->keys[1];
            z = kick->keys[0];
            a = root->children[0];
            b = root->children[1];
            c = kick->children[0];
            d = kick->children[1];
        }

        // Split.
        kick->keys[0] = x;
        kick->children[0] = a;
        kick->children[1] = b;
        kick->children[2] = nullptr;
        kick->size = 1;

        auto node = new node_value({z, 0}, {c, d, nullptr}, 1);

        root->keys[0] = y;
        root->children[0] = kick;
        root->children[1] = node;
        root->children[2] = nullptr;
        // Root is kicked.
        root->size = 0;
        return root;
    }

    node_ptr insert(node_ptr root, key_type key) {
        if (root == nullptr) {
            ++m_size;
            return new node_value({key, 0}, {nullptr, nullptr, nullptr}, 1);
        }
        
        if (key == root->keys[0] || (root->size == 2 && key == root->keys[1])) {
            return root;
        }

        if (root->children[0] == nullptr) {
            ++m_size;
            if (root->size == 1) {
                root->keys[1] = key;
                root->children[2] = nullptr;
                root->size = 2;
                if (root->keys[0] > root->keys[1]) {
                    std::swap(root->keys[0], root->keys[1]);
                }
                return root;
            } else {
                auto node = new node_value({key, 0}, {nullptr, nullptr, nullptr}, 0);
                size_type pivot = -1;
                if (key < root->keys[0]) {
                    pivot = 0;
                } else if ((root->size == 1) || (key < root->keys[1])) {
                    pivot = 1;
                } else {
                    pivot = 2;
                }
                root->children[pivot] = node;
                return split(root, pivot);
            }
        }

        size_type pivot = -1;
        if (key < root->keys[0]) {
            pivot = 0;
        } else if ((root->size == 1) || (key < root->keys[1])) {
            pivot = 1;
        } else {
            pivot = 2;
        }

        root->children[pivot] = insert(root->children[pivot], key);
        if (root->children[pivot]->size != 0) {
            return root;
        }

        if (root->size == 1) {
            auto node = new node_value({0, 0}, {root->children[pivot], nullptr, nullptr}, 0);
            root->children[pivot] = node;
            if (pivot == 0) {
                return merge_left(root, pivot);
            }
            return merge_right(root, pivot);
        }

        return split(root, pivot);
    }

    node_ptr remove(node_ptr root, key_type key) {
        if (root == nullptr) {
            return nullptr;
        }

        if (key == root->keys[0] || (root->size == 2 && key == root->keys[1])) {
            if (root->children[0] == nullptr) {
                --m_size;
                if (root->keys[0] == key) {
                    root->keys[0] = 0;
                    if (root->size == 2) {
                        std::swap(root->keys[0], root->keys[1]);
                    }
                } else {
                    root->keys[1] = 0;
                }
                root->size--;
                return root;
            } 
            
            auto succ = root->children[root->size];
            while (succ->children[0] != nullptr) {
                succ = succ->children[0];
            }
            if (root->keys[0] == key) {
                std::swap(succ->keys[0], root->keys[0]);
            } else {
                std::swap(succ->keys[0], root->keys[1]);
            }
        }

        size_type pivot = -1;
        if (key < root->keys[0]) {
            pivot = 0;
        } else if ((root->size == 1) || (key < root->keys[1])) {
            pivot = 1;
        } else {
            pivot = 2;
        }

        root->children[pivot] = remove(root->children[pivot], key);
        if (root->children[pivot]->size != 0) {
            return root;
        }

        // TODO: balance.
        if (root->size == 2) {
            if (pivot == 0) {
                if (root->children[1]->size == 1) {
                    root = merge_left(root, pivot);
                } else {
                    root = rotate_left(root, pivot);
                }
            } else if (pivot == 1) {
                if (root->children[0]->size == 2) {
                    root = rotate_right(root, pivot);
                } else if (root->children[2]->size == 2) {
                    root = rotate_left(root, pivot);
                } else {
                    root = merge_left(root, pivot);
                }
            } else {
                if (root->children[1]->size == 1) {
                    root = merge_right(root, pivot);
                } else {
                    root = rotate_right(root, pivot);
                }
            }
        } else {
            if (pivot == 0) {
                if (root->children[1]->size == 1) {
                    root = merge_left(root, pivot);
                } else {
                    root = rotate_left(root, pivot);
                }
            } else {
                if (root->children[0]->size == 1) {
                    root = merge_right(root, pivot);
                } else {
                    root = rotate_right(root, pivot);
                }
            }
        }

        return root;
    }

public:
    TwoThreeTree() : m_root(nullptr), m_size(0) {}

    bool contains(key_type key) const {
        return contains(m_root, key);
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
        m_root = insert(m_root, key);
        if (m_root->size == 0) m_root->size = 1;
    }

    void remove(key_type key) {
        m_root = remove(m_root, key);
    }
};