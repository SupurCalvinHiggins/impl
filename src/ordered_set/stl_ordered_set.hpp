#include <set>
#include <optional>
#include <cinttypes>

class StlOrderedSet {

public:
    using key_type = uint64_t;
    using size_type = size_t;

private:
    std::set<key_type> m_set;

public:
    StlOrderedSet() {}

    bool contains(key_type key) const {
        return m_set.find(key) != m_set.end();
    }

    std::optional<key_type> predecessor(key_type key) const {
        auto it = m_set.lower_bound(key);
        if (it == m_set.begin()) {
            return std::nullopt;
        }
        return std::optional(*(--it));
    }

    std::optional<key_type> successor(key_type key) const {
        auto it = m_set.upper_bound(key);
        if (it == m_set.end()) {
            return std::nullopt;
        }
        return std::optional(*it);
    }

    size_type size() const {
        return m_set.size();
    }

    void insert(key_type key) {
        m_set.insert(key);
    }

    void remove(key_type key) {
        auto it = m_set.find(key);
        if (it == m_set.end()) {
            return;
        }
        m_set.erase(it);
    }
};