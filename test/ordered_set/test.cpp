#include <random>
#include <limits>

#include <gtest/gtest.h>

#include "../../src/ordered_set/avl_tree.hpp"
#include "../../src/ordered_set/two_three_tree.hpp"
#include "../../src/ordered_set/stl_ordered_set.hpp"

enum Op {
    Insert,
    Remove
};

template <class OrderedSet>
class OrderedSetTest : public testing::Test {
protected:
    using key_type = typename OrderedSet::key_type;
    using size_type = typename OrderedSet::size_type;

    static constexpr size_type size = 1024;

protected:
    OrderedSetTest() { }
    ~OrderedSetTest() override { }

    void check(const std::vector<Op>& ops, const std::vector<key_type> keys) {
        assert(ops.size() == keys.size());

        OrderedSet set;
        StlOrderedSet stl_set;
        
        for (size_t i = 0; ; ++i) {
            ASSERT_EQ(stl_set.size(), set.size());
            for (const auto key : keys) {
                ASSERT_EQ(stl_set.contains(key), set.contains(key));
                // ASSERT_EQ(stl_set.predecessor(key), set.predecessor(key));
                // ASSERT_EQ(stl_set.successor(key), set.successor(key));
            }

            if (i >= ops.size()) {
                break;
            }

            auto op = ops[i];
            auto key = keys[i];

            switch (op) {
                case Op::Insert:
                    stl_set.insert(key);
                    set.insert(key);
                    break;
                case Op::Remove:
                    stl_set.remove(key);
                    set.remove(key);
                    break;
            }
        }
    }
};

TYPED_TEST_SUITE_P(OrderedSetTest);

TYPED_TEST_P(OrderedSetTest, InsertInc) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(i);
        ops.push_back(Op::Insert);
    }

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, InsertDec) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(this->size - i);
        ops.push_back(Op::Insert);
    }

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, InsertRng) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::mt19937 rng(0);
    std::uniform_int_distribution<key_type> dist(
        std::numeric_limits<key_type>::min(),
        std::numeric_limits<key_type>::max()
    );

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(dist(rng));
        ops.push_back(Op::Insert);
    }

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, InsertDbl) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::vector<key_type> keys{0, 0};
    std::vector<Op> ops{Op::Insert, Op::Insert};

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, RemoveInc) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(i);
        ops.push_back(Op::Insert);
    }
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(keys[i]);
        ops.push_back(Op::Remove);
    }

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, RemoveDec) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(this->size - i);
        ops.push_back(Op::Insert);
    }
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(keys[i]);
        ops.push_back(Op::Remove);
    }

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, RemoveRng) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::mt19937 rng;
    std::uniform_int_distribution<key_type> dist(
        std::numeric_limits<key_type>::min(),
        std::numeric_limits<key_type>::max()
    );

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(dist(rng));
        ops.push_back(Op::Insert);
    }
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(keys[i]);
        ops.push_back(Op::Remove);
    }

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, RemoveDbl) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::vector<key_type> keys{0, 0, 0};
    std::vector<Op> ops{Op::Insert, Op::Remove, Op::Remove};

    this->check(ops, keys);
}

TYPED_TEST_P(OrderedSetTest, InsertRemoveRng) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    std::mt19937 rng;
    std::uniform_int_distribution<key_type> dist(
        0,
        static_cast<size_type>(std::sqrt(this->size))
    );

    std::vector<key_type> keys;
    std::vector<Op> ops;
    for (key_type i = 0; i < this->size; ++i) {
        keys.push_back(dist(rng));
        ops.push_back(Op::Insert);
        keys.push_back(dist(rng));
        ops.push_back(Op::Remove);
    }

    this->check(ops, keys);
}

REGISTER_TYPED_TEST_SUITE_P(OrderedSetTest,
    InsertInc, InsertDec, InsertRng, InsertDbl,
    RemoveInc, RemoveDec, RemoveRng, RemoveDbl,
    InsertRemoveRng
);

typedef testing::Types<TwoThreeTree, AVLTree> OrderedSetImplementations;
INSTANTIATE_TYPED_TEST_SUITE_P(OrderedSetTestSuite, OrderedSetTest, OrderedSetImplementations);
