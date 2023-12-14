#include <array>
#include <cassert>
#include <algorithm>
#include <cstddef>


class TwoThreeTree {

public:
    using size_type = size_t;
    using key_type = uint64_t;

private:
    struct Node {
        std::array<Node*, 4> children;
        std::array<key_type, 3> keys;
        size_type size;

        Node() : children{nullptr}, keys{0}, size(0) {}

        bool leaf() { 
            return children[0] == nullptr; 
        }

        bool insert(key_type key) {
            assert(leaf());
            auto it = std::lower_bound(keys.begin(), keys.begin() + size, key);
            if (it != (keys.begin() + size) && *it == key) {
                return false;
            }
            auto pivot = std::distance(keys.begin(), it);
            for (size_type i = 2; i > pivot; --i) {
                keys[i] = keys[i-1];
            }
            keys[pivot] = key;
            ++size;
            return true;
        }

        bool remove(key_type key) {
            assert(leaf());
            auto it = std::find(keys.begin(), keys.begin() + size, key);
            if (it == keys.begin() + size) {
                return false;
            }
            auto pivot = std::distance(keys.begin(), it);
            for (size_type i = pivot; i < size; ++i) {
                keys[i] = keys[i+1];
            }
            keys[size - 1] = 0;
            --size;
            return true;
        }

        bool contains(key_type key) {
            return std::find(keys.begin(), keys.begin() + size, key) != keys.begin() + size;
        }

        void split() {
            assert(size == 3);

            auto left = new Node();
            left->children[0] = children[0];
            left->children[1] = children[1];
            left->keys[0] = keys[0];
            left->size = 1;

            auto right = new Node();
            right->children[0] = children[2];
            right->children[1] = children[3];
            right->keys[0] = keys[2];
            right->size = 1;

            keys[0] = keys[1];
            keys[1] = 0;
            keys[2] = 0;
            children[0] = left;
            children[1] = right;
            children[2] = nullptr;
            children[3] = nullptr;
            size = 1;
        }

        void merge(Node* child) {
            assert(child->size == 1);
            assert(!child->leaf());

            auto it = std::find(children.begin(), children.begin() + size + 1, child);
            assert(it != children.begin() + size + 1);
            auto pivot = std::distance(children.begin(), it);

            for (size_type i = 2; i > pivot; --i) {
                keys[i] = keys[i-1];
            }

            for (size_type i = 3; i > pivot; --i) {
                children[i] = children[i-1];
            }

            keys[pivot] = child->keys[0];
            children[pivot] = child->children[0];
            children[pivot+1] = child->children[1];
            ++size;
            delete child;         
        }

        void bubble(Node* child) {
            assert(child->size == 0);
            if (size == 1) {
                if (children[0]->size == 1 || children[1]->size == 1) {
                    auto node = new Node();
                    if (children[0]->size == 1) {
                        node->keys[0] = children[0]->keys[0];
                        node->keys[1] = keys[0];
                        node->children[0] = children[0]->children[0];
                        node->children[1] = children[0]->children[1];
                        node->children[2] = children[1]->children[0];
                    } else {
                        node->keys[0] = keys[0];
                        node->keys[1] = children[1]->keys[0];
                        node->children[0] = children[1]->children[0];
                        node->children[1] = children[1]->children[1];
                        node->children[2] = children[0]->children[0];
                    }
                    node->size = 2;
                    
                    delete children[0];
                    delete children[1];

                    size = 0;
                    keys[0] = 0;
                    children[0] = node;
                    children[1] = nullptr;
                } else {
                    // rotate left or right to fill in the hole.
                    auto l = children[0];
                    auto r = children[1];
                    key_type x,y,z;
                    Node* a,*b,*c,*d;
                    if (l->size == 2) {
                        x = l->keys[0];
                        y = l->keys[1];
                        z = keys[0];
                        a = l->children[0];
                        b = l->children[1];
                        c = l->children[2];
                        d = r->children[0];
                    } else {
                        x = keys[0];
                        y = r->keys[0];
                        z = r->keys[1];
                        a = l->children[0];
                        b = r->children[0];
                        c = r->children[1];
                        d = r->children[2];
                    }

                    l->children[0] = a;
                    l->children[1] = b;
                    l->children[2] = nullptr;
                    l->keys[0] = x;
                    l->keys[1] = 0;
                    l->size = 1;

                    r->children[0] = c;
                    r->children[1] = d;
                    r->keys[0] = z;
                    r->size = 1;

                    keys[0] = y;
                    children[0] = l;
                    children[1] = r;
                }
            } else {
                if (children[0]->size == 1 || children[1]->size == 1 || children[2]->size == 1) {
                    Node *a, *b, *c, *d;
                    key_type x, y, z;
                    auto l = children[0];
                    auto m = children[1];
                    auto r = children[2];
                    if (children[0]->size == 0) {
                        a = l->children[0];
                        b = m->children[0];
                        c = m->children[1];
                        d = r;
                        x = keys[0];
                        z = keys[1];
                        y = children[1]->keys[0];
                    } else if (children[1]->size == 0) {
                        a = l->children[0];
                        b = l->children[1];
                        c = m->children[0];
                        d = r;
                        x = l->keys[1];
                        y = keys[0];
                        z = keys[1]
                    } else {
                        a = l;
                        b = m->children[0];
                        c = m->children[1];
                        d = r->children[0];
                        x = keys[0];
                        y = m->keys[0];
                        z = keys[1];

                        children[0] = l;
                        children[1] = r;
                        keys[0] = x;
                        size = 1;

                        r->children[0] = a;
                        r->children[1] = b;
                        r->children[2] = c;
                        r->keys[1] = y;
                        r->keys[2] = z;
                        r->size = 2;

                        return;
                    }

                    children[0] = l;
                    children[1] = r;
                    keys[0] = z;
                    size = 1;

                    l->children[0] = a;
                    l->children[1] = b;
                    l->children[2] = c;
                    l->keys[0] = x;
                    l->keys[1] = y;
                    l->size = 2;

                    delete m;
                } else {
                    Node *a, *b, *c, *d, *e;
                    key_type x, y, w, z;
                    auto l = children[0];
                    auto m = children[1];
                    auto r = children[2];

                    if (l->size == 0) {
                        a = l->children[0];
                        b = m->children[1];
                        c = m->children[2];
                        d = m->children[3];
                        e = r;

                        w = keys[0];
                        z = keys[1];
                        x = m->keys[0];
                        y = m->keys[1];
                    } else if (m->size == 0) {
                        
                    } else {

                        return;
                    }
                }
            }
        }
    };

    Node* m_root;
    size_type m_size;

    Node* insert(Node* root, key_type key) {
        if (root->leaf()) {
            if (root->insert(key)) {
                ++m_size;
            }
        
            if (root->size <= 2) {
                return nullptr;
            }

            assert(root->size == 3);
            root->split();
            return root;
        }

        assert((root->size == 1) || (root->size == 2));

        Node* child = nullptr;
        if (root->size == 1) {
            if (key == root->keys[0]) return nullptr;
            if (key < root->keys[0]) {
                child = insert(root->children[0], key);
            } else {
                child = insert(root->children[1], key);
            }
        } else {
            if (key == root->keys[0] || key == root->keys[1]) return nullptr;
            if (key < root->keys[0]) {
                child = insert(root->children[0], key);
            } else if (key < root->keys[1]) {
                child = insert(root->children[1], key);
            } else {
                child = insert(root->children[2], key);
            }
        }
        
        if (child == nullptr) return nullptr;
        root->merge(child);
        if (root->size != 3) return nullptr;
        root->split();
        return root;
    }

    bool contains(Node* root, key_type key) {
        if (root->leaf()) {
            return root->contains(key);
        }

        if (root->contains(key)) {
            return true;
        }

        if (root->size == 1) {
            if (key < root->keys[0]) {
                return contains(root->children[0], key);
            } else {
                return contains(root->children[1], key);
            }
        } else {
            if (key < root->keys[0]) {
                return contains(root->children[0], key);
            } else if (key < root->keys[1]) {
                return contains(root->children[1], key);
            } else {
                return contains(root->children[2], key);
            }
        }
    }

    Node* remove(Node* root, key_type key) {
        if (!root->contains(key)) {
            goto remove;
        }

        if (!root->leaf()) {
            // swap with succ
            auto it = std::find(root->keys.begin(), root->keys.begin() + root->size + 1, key);
            auto pivot = std::distance(root->keys.begin(), it);
            auto node = root->children[pivot+1];
            while (!node->leaf()) {
                node = node->children[0];
            }
            std::swap(root->keys[pivot], node->keys[0]);
            goto remove;
        }

        assert(root->leaf());
        assert(root->size == 1 || root->size == 2);

        root->remove(key);
        if (root->size == 1) {
            return nullptr;
        }

        assert(root->size == 0);
        return root;

    remove:
        Node* child = nullptr;
        if (root->size == 1) {
            if (key < root->keys[0]) {
                child = remove(root->children[0], key);
            } else {
                child = remove(root->children[1], key);
            }
        } else {
            if (key < root->keys[0]) {
                child = remove(root->children[0], key);
            } else if (key < root->keys[1]) {
                child = remove(root->children[1], key);
            } else {
                child = remove(root->children[2], key);
            }
        }
        if (child == nullptr) return nullptr;
        root->bubble(child);
        return root->size == 0 ? root : nullptr;
    }

public:
    TwoThreeTree() : m_root(new Node()) {}

    void insert(key_type key) {
        insert(m_root, key);
    }

    void remove(key_type key) {
        remove(m_root, key);
    }

    bool contains(key_type key) {
        return contains(m_root, key);
    }
    
    size_type size() {
        return m_size;
    }
};