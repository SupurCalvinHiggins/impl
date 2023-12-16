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
        
        bool ok() const {
            if (size == 0) {
                return (
                    (keys[0] == 0)
                    && (keys[1] == 0)
                    // && (children[0] != nullptr)
                    && (children[1] == nullptr)
                    && (children[2] == nullptr)
                );
            } else if (size == 1) {
                return (
                    (keys[1] == 0)
                    && (children[0] != nullptr)
                    && (children[1] != nullptr)
                    && (children[2] == nullptr)
                ) || (children[0] == nullptr && children[1] == nullptr && children[2] == nullptr);
            } else if (size == 2) {
                return (
                    (children[0] != nullptr)
                    && (children[1] != nullptr)
                    && (children[2] != nullptr)
                ) || (children[0] == nullptr && children[1] == nullptr && children[2] == nullptr);
            } else if (size == 3) {
                return (
                    (keys[1] == 0)
                    && (children[0] != nullptr)
                    && (children[1] != nullptr)
                    && (children[2] == nullptr)
                );
            }
            return false;
        }
    };

    using node_value = Node;
    using node_ptr = Node*;

    node_ptr m_root;
    size_type m_size;

    static constexpr size_type HOLE = 0;
    static constexpr size_type KICK = 3;

    size_type find_pivot(const node_ptr root, const key_type key) const {
        assert(root != nullptr);
        auto it = std::lower_bound(
            root->keys.begin(), root->keys.begin() + root->size, key);
        return std::distance(root->keys.begin(), it);
    }

    bool contains(const node_ptr root, const key_type key) const {
        if (root == nullptr) {
            return false;
        }

        const auto pivot = find_pivot(root, key);
        if ((pivot < root->size) && (key == root->keys[pivot])) {
            return true;
        }

        return contains(root->children[pivot], key);
    }

    node_ptr rotate(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        assert(pivot <= root->size);

        node_ptr l, r, a, b, c, d;
        key_type x, y, z;

        auto is_left = (
            (pivot != root->size)
            && (root->children[pivot+1]->size == 2) 
        );

        // Destructure for left rotation.
        if (is_left) {

            // Destructure root.
            x = root->keys[pivot];
            l = root->children[pivot];
            r = root->children[pivot+1];

            assert(l != nullptr);
            assert(r != nullptr);
            assert(l->size == HOLE);
            assert(r->size == 2);

            // Destructure left and right.
            y = r->keys[0];
            z = r->keys[1];
            a = l->children[0];
            b = r->children[0];
            c = r->children[1];
            d = r->children[2];

            // Rotate root.
            root->keys[pivot] = y;
        } 

        // Destructure for right rotation.
        else {

            // Destructure root.
            z = root->keys[pivot-1];
            l = root->children[pivot-1];
            r = root->children[pivot];

            assert(l != nullptr);
            assert(r != nullptr);
            assert(l->size == 2);
            assert(r->size == HOLE);

            // Destructure left and right.
            x = l->keys[0];
            y = l->keys[1];
            a = l->children[0];
            b = l->children[1];
            c = l->children[2];
            d = r->children[0];

            // Rotate root.
            root->keys[pivot-1] = y;
        }

        // Rotate left subtree.
        l->keys[0] = x;
        l->keys[1] = 0;
        l->children[0] = a;
        l->children[1] = b;
        l->children[2] = nullptr;
        l->size = 1;
        assert(l->ok());

        // Rotate right subtree.
        r->keys[0] = z;
        r->keys[1] = 0;
        r->children[0] = c;
        r->children[1] = d;
        r->children[2] = nullptr;
        r->size = 1;
        assert(r->ok());

        return root;
    }

    node_ptr merge(node_ptr root, size_type pivot) const {
        assert(root != nullptr);
        assert(pivot <= root->size);

        node_ptr l, r, a, b, c;
        key_type x, y;

        auto is_left = (
            (pivot != root->size)
            && (root->children[pivot+1]->size == 1) 
        );

        // Destructure for left merge.
        if (is_left) {

            // Destructure root.
            x = root->keys[pivot];
            l = root->children[pivot];
            r = root->children[pivot+1];

            assert(l != nullptr);
            assert(r != nullptr);
            assert(l->size == HOLE);
            assert(r->size == 1);

            // Destructure left and right.
            y = r->keys[0];
            a = l->children[0];
            b = r->children[0];
            c = r->children[1];

            // Merge root.
            root->children[pivot] = l;
            root->children[pivot+1] = nullptr;
            root->keys[pivot] = 0;
            if (root->size == 2 && pivot == 0) {
                std::swap(root->children[pivot+1], root->children[pivot+2]);
                std::swap(root->keys[pivot], root->keys[pivot+1]);
            }
            root->size--;
            assert(root->ok());  
        } 

        // Destructure for right merge.
        else {

            // Destructure root.
            y = root->keys[pivot-1];
            l = root->children[pivot-1];
            r = root->children[pivot];

            assert(l != nullptr);
            assert(r != nullptr);
            assert(l->size == 1);
            assert(r->size == HOLE);

            // Destructure left and right.
            x = l->keys[0];
            a = l->children[0];
            b = l->children[1];
            c = r->children[0];

            // Merge root.
            root->children[pivot-1] = l;
            root->children[pivot] = nullptr;
            root->keys[pivot-1] = 0;
            if (root->size == 2 && pivot-1 == 0) {
                std::swap(root->children[pivot], root->children[pivot+1]);
                std::swap(root->keys[pivot-1], root->keys[pivot]);
            }
            root->size--;
            assert(root->ok());           
        }

        // Merge left subtree.
        l->keys[0] = x;
        l->keys[1] = y;
        l->children[0] = a;
        l->children[1] = b;
        l->children[2] = c;
        l->size = 2;
        assert(l->ok());

        // Merge right subtree.
        delete r;

        return root;
    }

    node_ptr split(node_ptr root, size_type pivot) {
        assert(root->size == 2);

        // Destructure root.
        auto kick = root->children[pivot];
        assert(kick->size == KICK);

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
        assert(kick->ok());

        auto node = new node_value({z, 0}, {c, d, nullptr}, 1);
        assert(node->ok());

        root->keys[0] = y;
        root->keys[1] = 0;
        root->children[0] = kick;
        root->children[1] = node;
        root->children[2] = nullptr;
        // Root is kicked.
        root->size = KICK;
        assert(root->ok());
        return root;
    }

    node_ptr insert(node_ptr root, key_type key) {
        if (root == nullptr) {
            ++m_size;
            return new node_value({key, 0}, {nullptr, nullptr, nullptr}, 1);
        }

        auto pivot = find_pivot(root, key);
        if ((pivot < root->size) && (key == root->keys[pivot])) {
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
                assert(root->ok());
                return root;
            }

            auto node = new node_value({key, 0}, {nullptr, nullptr, nullptr}, KICK);
            root->children[pivot] = node;
            return split(root, pivot);
        }

        root->children[pivot] = insert(root->children[pivot], key);
        if (root->children[pivot]->size != KICK) {
            assert(root->ok());
            return root;
        }

        if (root->size == 1) {
            root->children[pivot]->size = 1;
            auto node = new node_value({0, 0}, {root->children[1-pivot], nullptr, nullptr}, 0);
            root->children[1-pivot] = node;
            assert(root->ok());
            root = merge(root, 1-pivot)->children[0];
            assert(root->size == 2);
            return root;
        }

        return split(root, pivot);
    }

    node_ptr remove(node_ptr root, key_type key) {
        if (root == nullptr) {
            return nullptr;
        }

        auto pivot = find_pivot(root, key);

        if ((pivot < root->size) && (key == root->keys[pivot])) {
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

            auto succ_idx = root->keys[0] == key ? 1 : 2;
            auto succ = root->children[succ_idx];
            while (succ->children[0] != nullptr) {
                succ = succ->children[0];
            }
            if (root->keys[0] == key) {
                std::swap(succ->keys[0], root->keys[0]);
            } else {
                std::swap(succ->keys[0], root->keys[1]);
            }

            root->children[succ_idx] = remove(root->children[succ_idx], key);
            pivot = succ_idx;
            goto balance;
        }

        root->children[pivot] = remove(root->children[pivot], key);
        balance:
        if (root->children[pivot] == nullptr || root->children[pivot]->size != 0) {
            return root;
        }
        // assert(root->children[pivot]->ok());
        // assert(root->ok());
        
        // TODO: balance.
        auto is_rotate = (
            (pivot == 0 && root->children[pivot+1]->size == 2)
            || (pivot == root->size && root->children[pivot-1]->size == 2)
            || (pivot != 0 && pivot != root->size && (root->children[pivot+1]->size == 2 || root->children[pivot-1]->size == 2))
        );
        if (is_rotate) {
            return rotate(root, pivot);
        }
        return merge(root, pivot);
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
        if (m_root->size == KICK) {
            m_root->size = 1;
        }
        assert(contains(key));
    }

    void remove(key_type key) {
        m_root = remove(m_root, key);
        if (m_root != nullptr && m_root->size == HOLE) {
            auto root = m_root->children[0];
            delete m_root;
            m_root = root;
        }
        assert(!contains(key));
    }
};