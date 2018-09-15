#include <gtest/gtest.h>
#include <history.hxx>

namespace replxx {

  class history_test : public ::testing::Test {

  public:
    history_test() = default;

    ~history_test() override = default;

  protected:
    void TearDown() override {
    }

    void SetUp() override {
    }
  };

  TEST(history_test, add_remove_history) {

    History p_hist;
    std::string test_str = "this is the first add";
    p_hist.add(test_str);
    EXPECT_EQ(p_hist[0], test_str);

  }

}