#include <ostream>
#include <gtest/gtest.h>
#include "List.hpp"

using namespace org_pqrs_Karabiner;

int allocatecount = 0;

class TestItem final : public List::Item {
public:
  TestItem(int v) : value_(v) { ++allocatecount; }
  virtual ~TestItem(void) {
    --allocatecount;
  };

  int v(void) { return value_; }

private:
  int value_;
};

TEST(List, push_back) {
  List list;
  TestItem* p = nullptr;
  const int MAXITEM = 10;

  // ------------------------------------------------------------
  list.push_back(nullptr);
  EXPECT_EQ(static_cast<size_t>(0), list.size());

  // ------------------------------------------------------------
  EXPECT_EQ(static_cast<size_t>(0), list.size());
  for (int i = 0; i < MAXITEM; ++i) {
    list.push_back(new TestItem(i));
    EXPECT_EQ(static_cast<size_t>(i + 1), list.size());
  }

  for (int i = 0; i < MAXITEM; ++i) {
    p = dynamic_cast<TestItem*>(list.safe_front());
    EXPECT_TRUE(p != nullptr);
    EXPECT_EQ(i, p->v());

    list.pop_front();
    EXPECT_EQ(static_cast<size_t>(MAXITEM - (i + 1)), list.size());
  }
  EXPECT_EQ(static_cast<size_t>(0), list.size());
  EXPECT_EQ(0, allocatecount);

  // ------------------------------------------------------------
  // check prev/next value.
  TestItem* item1 = new TestItem(1);
  TestItem* item2 = new TestItem(2);
  EXPECT_TRUE(item1->getprev() == nullptr);
  EXPECT_TRUE(item1->getnext() == nullptr);
  EXPECT_TRUE(item2->getprev() == nullptr);
  EXPECT_TRUE(item2->getnext() == nullptr);

  list.push_back(item1);
  EXPECT_TRUE(item1->getprev() == nullptr);
  EXPECT_TRUE(item1->getnext() == nullptr);
  list.push_back(item2);
  EXPECT_TRUE(item1->getprev() == nullptr);
  EXPECT_TRUE(item1->getnext() == item2);
  EXPECT_TRUE(item2->getprev() == item1);
  EXPECT_TRUE(item2->getnext() == nullptr);

  list.pop_front();
  EXPECT_TRUE(item2->getprev() == nullptr);
  EXPECT_TRUE(item2->getnext() == nullptr);

  list.pop_front();
  EXPECT_EQ(0, allocatecount);
}

TEST(List, push_front) {
  List list;
  TestItem* p = nullptr;
  const int MAXITEM = 10;

  // ------------------------------------------------------------
  list.push_front(nullptr);
  EXPECT_EQ(static_cast<size_t>(0), list.size());

  // ------------------------------------------------------------
  EXPECT_EQ(static_cast<size_t>(0), list.size());
  for (int i = 0; i < MAXITEM; ++i) {
    list.push_front(new TestItem(i));
    EXPECT_EQ(static_cast<size_t>(i + 1), list.size());
  }

  for (int i = 0; i < MAXITEM; ++i) {
    p = dynamic_cast<TestItem*>(list.safe_front());
    EXPECT_TRUE(p != nullptr);
    EXPECT_EQ(MAXITEM - (i + 1), p->v());

    list.pop_front();
    EXPECT_EQ(static_cast<size_t>(MAXITEM - (i + 1)), list.size());
  }
  EXPECT_EQ(static_cast<size_t>(0), list.size());
  EXPECT_EQ(0, allocatecount);

  // ------------------------------------------------------------
  // check prev/next value.
  TestItem* item1 = new TestItem(1);
  TestItem* item2 = new TestItem(2);
  EXPECT_TRUE(item1->getprev() == nullptr);
  EXPECT_TRUE(item1->getnext() == nullptr);
  EXPECT_TRUE(item2->getprev() == nullptr);
  EXPECT_TRUE(item2->getnext() == nullptr);

  list.push_front(item1);
  EXPECT_TRUE(item1->getprev() == nullptr);
  EXPECT_TRUE(item1->getnext() == nullptr);
  list.push_front(item2);
  EXPECT_TRUE(item1->getprev() == item2);
  EXPECT_TRUE(item1->getnext() == nullptr);
  EXPECT_TRUE(item2->getprev() == nullptr);
  EXPECT_TRUE(item2->getnext() == item1);

  list.pop_front();
  EXPECT_TRUE(item1->getprev() == nullptr);
  EXPECT_TRUE(item1->getnext() == nullptr);

  list.pop_front();
  EXPECT_EQ(0, allocatecount);
}

TEST(List, safe_front) {
  List list;
  list.safe_front();
  list.safe_front();
  list.safe_front();
}
TEST(List, safe_back) {
  List list;
  list.safe_back();
  list.safe_back();
  list.safe_back();
}
TEST(List, pop_front) {
  List list;
  list.pop_front();
  list.pop_front();
  list.pop_front();
}
TEST(List, pop_back) {
  List list;
  list.pop_back();
  list.pop_back();
  list.pop_back();

  list.push_back(new TestItem(1));
  list.push_back(new TestItem(2));
  list.pop_back();
  EXPECT_EQ(static_cast<size_t>(1), list.size());

  TestItem* p = static_cast<TestItem*>(list.safe_back());
  EXPECT_EQ(static_cast<size_t>(1), p->v());
}

TEST(List, insert) {
  List list;
  TestItem* p = nullptr;

  list.insert(nullptr, nullptr);

  // push_front if first argument == nullptr.
  p = static_cast<TestItem*>(list.insert(nullptr, new TestItem(1))); // [1]
  p = static_cast<TestItem*>(list.insert(nullptr, new TestItem(2))); // [2,1]

  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_front())->v());
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_front()->getnext())->v());
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_back())->v());
  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_back()->getprev())->v());

  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();

  p = static_cast<TestItem*>(list.insert(nullptr, new TestItem(1))); // [1]
  p = static_cast<TestItem*>(list.insert(p, new TestItem(2)));       // [2,1]

  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_front())->v());
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_front()->getnext())->v());
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_back())->v());
  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_back()->getprev())->v());

  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();

  p = static_cast<TestItem*>(list.insert(nullptr, new TestItem(1)));          // [1]
  p = static_cast<TestItem*>(list.insert(p, new TestItem(2)));                // [2,1]
  p = static_cast<TestItem*>(list.insert(list.safe_back(), new TestItem(3))); // [2,3,1]

  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_front())->v());
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_back())->v());

  EXPECT_EQ(2, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();
  EXPECT_EQ(3, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();
  EXPECT_EQ(1, static_cast<TestItem*>(list.safe_front())->v());
  list.pop_front();
}

TEST(List, erase_and_delete) {
  const int MAXITEM = 10;

  // ------------------------------------------------------------
  for (int erase_index = 0; erase_index < MAXITEM; ++erase_index) {
    List list;

    EXPECT_EQ(static_cast<size_t>(0), list.size());
    for (int i = 0; i < MAXITEM; ++i) {
      list.push_back(new TestItem(i));
      EXPECT_EQ(static_cast<size_t>(i + 1), list.size());
    }

    TestItem* p = static_cast<TestItem*>(list.safe_front());
    for (int i = 0; i < erase_index; ++i) {
      p = static_cast<TestItem*>(p->getnext());
    }
    TestItem* next = static_cast<TestItem*>(p->getnext());
    EXPECT_EQ(next, list.erase_and_delete(p));

    // check
    p = static_cast<TestItem*>(list.safe_front());
    for (int i = 0; i < MAXITEM; ++i) {
      if (i == erase_index) continue;
      EXPECT_EQ(i, p->v());
      p = static_cast<TestItem*>(p->getnext());
    }
    EXPECT_EQ(static_cast<size_t>(MAXITEM - 1), list.size());
  }
}

TEST(List, clear) {
  List list;
  const int MAXITEM = 10;

  // ------------------------------------------------------------
  EXPECT_EQ(static_cast<size_t>(0), list.size());
  for (int i = 0; i < MAXITEM; ++i) {
    list.push_back(new TestItem(i));
    EXPECT_EQ(static_cast<size_t>(i + 1), list.size());
  }
  list.clear();
  EXPECT_EQ(static_cast<size_t>(0), list.size());
  EXPECT_EQ(0, allocatecount);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
