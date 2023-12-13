#include <random>
#include <limits>

#include <gtest/gtest.h>

template <class T>
class OrderedSetTest : public testing::Test {
protected:
    OrderedSetTest() : m_set(new T()) {}
    ~OrderedSetTest() override { delete m_set; }

    T* const m_set;
};

TYPED_TEST_SUITE_P(OrderedSetTest);

TYPED_TEST_P(OrderedSetTest, InsertInc) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    constexpr size_type size = 256;

    for (key_type i = 0; i < size; ++i) {
        ASSERT_FALSE(this->m_set->contains(i));
    }
    ASSERT_EQ(this->m_set->size(), 0);

    for (key_type i = 0; i < size; ++i) {
        this->m_set->insert(i);

        for (key_type j = 0; j <= i; ++j) {
            ASSERT_TRUE(this->m_set->contains(j));
        }
        
        for (key_type j = i+1; j < size; ++j) {
            ASSERT_FALSE(this->m_set->contains(j)); 
        }

        ASSERT_EQ(this->m_set->size(), i + 1);
    }
}

TYPED_TEST_P(OrderedSetTest, InsertDec) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    constexpr size_type size = 256;

    for (key_type i = 0; i < size; ++i) {
        ASSERT_FALSE(this->m_set->contains(i));
    }
    ASSERT_EQ(this->m_set->size(), 0);

    for (key_type i = 0; i < size; ++i) {
        this->m_set->insert(size - i);

        for (key_type j = 0; j <= i; ++j) {
            ASSERT_TRUE(this->m_set->contains(size - j));
        }
        
        for (key_type j = i+1; j < size; ++j) {
            ASSERT_FALSE(this->m_set->contains(size - j));
        }

        ASSERT_EQ(this->m_set->size(), i + 1);
    }
}

TYPED_TEST_P(OrderedSetTest, InsertRng) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    constexpr size_type size = 256;
    
    std::mt19937 rng;
    std::uniform_int_distribution<key_type> dist(
        std::numeric_limits<key_type>::min(),
        std::numeric_limits<key_type>::max()
    );

    std::vector<key_type> data;
    for (key_type i = 0; i < size; ++i) {
        data.push_back(dist(rng));
    }

    for (key_type i = 0; i < size; ++i) {
        ASSERT_FALSE(this->m_set->contains(data[i]));
    }
    ASSERT_EQ(this->m_set->size(), 0);

    for (key_type i = 0; i < size; ++i) {
        this->m_set->insert(data[i]);

        for (key_type j = 0; j <= i; ++j) {
            ASSERT_TRUE(this->m_set->contains(data[j]));
        }
        
        for (key_type j = i+1; j < size; ++j) {
            ASSERT_FALSE(this->m_set->contains(data[j]));
        }

        ASSERT_EQ(this->m_set->size(), i + 1);
    }
}

TYPED_TEST_P(OrderedSetTest, InsertDbl) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    ASSERT_FALSE(this->m_set->contains(0));
    ASSERT_EQ(this->m_set->size(), 0);
    this->m_set->insert(0);
    ASSERT_TRUE(this->m_set->contains(0));
    ASSERT_EQ(this->m_set->size(), 1);
    this->m_set->insert(0);
    ASSERT_TRUE(this->m_set->contains(0));
    ASSERT_EQ(this->m_set->size(), 1);
}

TYPED_TEST_P(OrderedSetTest, RemoveInc) {
    using key_type = typename TypeParam::key_type;
    using size_type = typename TypeParam::size_type;

    constexpr size_type size = 16;

    for (key_type i = 0; i < size; ++i) {
        this->m_set->insert(i);
    }

    for (key_type i = 0; i < size; ++i) {
        ASSERT_TRUE(this->m_set->contains(i));
    }
    ASSERT_EQ(this->m_set->size(), size);

    this->m_set->print();

    for (key_type i = 0; i < size; ++i) {
        this->m_set->remove(i);
        this->m_set->print();

        for (key_type j = 0; j <= i; ++j) {
            ASSERT_FALSE(this->m_set->contains(j));
        }
        
        for (key_type j = i+1; j < size; ++j) {
            ASSERT_TRUE(this->m_set->contains(j));
        }

        ASSERT_EQ(this->m_set->size(), size - (i + 1));
    }
}

// TYPED_TEST_P(OrderedSetTest, InsertDec) {
//     using key_type = typename TypeParam::key_type;
//     using size_type = typename TypeParam::size_type;

//     constexpr size_type size = 256;

//     for (key_type i = 0; i < size; ++i) {
//         ASSERT_FALSE(this->m_set->contains(i));
//     }
//     ASSERT_EQ(this->m_set->size(), 0);

//     for (key_type i = 0; i < size; ++i) {
//         this->m_set->insert(size - i);

//         for (key_type j = 0; j <= i; ++j) {
//             ASSERT_TRUE(this->m_set->contains(size - j));
//         }
        
//         for (key_type j = i+1; j < size; ++j) {
//             ASSERT_FALSE(this->m_set->contains(size - j));
//         }

//         ASSERT_EQ(this->m_set->size(), i + 1);
//     }
// }

// TYPED_TEST_P(OrderedSetTest, InsertRng) {
//     using key_type = typename TypeParam::key_type;
//     using size_type = typename TypeParam::size_type;

//     constexpr size_type size = 256;
    
//     std::mt19937 rng;
//     std::uniform_int_distribution<key_type> dist(
//         std::numeric_limits<key_type>::min(),
//         std::numeric_limits<key_type>::max()
//     );

//     std::vector<key_type> data;
//     for (key_type i = 0; i < size; ++i) {
//         data.push_back(dist(rng));
//     }

//     for (key_type i = 0; i < size; ++i) {
//         ASSERT_FALSE(this->m_set->contains(data[i]));
//     }
//     ASSERT_EQ(this->m_set->size(), 0);

//     for (key_type i = 0; i < size; ++i) {
//         this->m_set->insert(data[i]);

//         for (key_type j = 0; j <= i; ++j) {
//             ASSERT_TRUE(this->m_set->contains(data[j]));
//         }
        
//         for (key_type j = i+1; j < size; ++j) {
//             ASSERT_FALSE(this->m_set->contains(data[j]));
//         }

//         ASSERT_EQ(this->m_set->size(), i + 1);
//     }
// }

// TYPED_TEST_P(OrderedSetTest, InsertDbl) {
//     using key_type = typename TypeParam::key_type;
//     using size_type = typename TypeParam::size_type;

//     ASSERT_FALSE(this->m_set->contains(0));
//     ASSERT_EQ(this->m_set->size(), 0);
//     this->m_set->insert(0);
//     ASSERT_TRUE(this->m_set->contains(0));
//     ASSERT_EQ(this->m_set->size(), 1);
//     this->m_set->insert(0);
//     ASSERT_TRUE(this->m_set->contains(0));
//     ASSERT_EQ(this->m_set->size(), 1);
// }

REGISTER_TYPED_TEST_SUITE_P(OrderedSetTest,
    InsertInc, 
    InsertDec,
    InsertRng, 
    InsertDbl,
    RemoveInc
);

// #include "b-tree.hpp"
// #include "2-3-tree.hpp"
#include "avl-tree.hpp"

typedef testing::Types<AVLTree> OrderedSetImplementations;
INSTANTIATE_TYPED_TEST_SUITE_P(OrderedSetTestSuite, OrderedSetTest, OrderedSetImplementations);
