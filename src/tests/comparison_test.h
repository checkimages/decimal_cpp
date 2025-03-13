#pragma once

#include "base_test.h"

namespace s21 {

class ComparisonTest : public BaseTest {
 public:
  ComparisonTest(const Decimal& lhs, const Decimal& rhs,
                 const std::vector<std::string>& operations = {"<", "<=", ">",
                                                               ">=", "==",
                                                               "!="})
      : BaseTest(operations),
        lhs_(lhs),
        rhs_(rhs),
        s21_lhs_(convert_to_s21(lhs)),
        s21_rhs_(convert_to_s21(rhs)) {}

  ComparisonTest(const Decimal& lhs, const Decimal& rhs, const char* operation)
      : ComparisonTest(lhs, rhs, std::vector<std::string>{operation}) {}

 protected:
  const Decimal lhs_, rhs_;
  const s21_decimal s21_lhs_, s21_rhs_;

  void printArgs() override {
    std::cout << "| value_1            | " << std::setw(31) << std::right
              << lhs_ << " |      |" << std::endl;
    std::cout << "| value_2            | " << std::setw(31) << std::right
              << rhs_ << " |      |" << std::endl;
  }

  bool checking() override {
    bool checking_code_result = checkingCode();

    if (checking_code_result && PrintAllResults) {
      printResults<int>(true);
    }

    return checking_code_result;
  }

  int perform_c() override {
    int (*ptr)(s21_decimal, s21_decimal);

    if (operation_ == "<") {
      ptr = s21_is_less;
    } else if (operation_ == "<=") {
      ptr = s21_is_less_or_equal;
    } else if (operation_ == ">") {
      ptr = s21_is_greater;
    } else if (operation_ == ">=") {
      ptr = s21_is_greater_or_equal;
    } else if (operation_ == "==") {
      ptr = s21_is_equal;
    } else if (operation_ == "!=") {
      ptr = s21_is_not_equal;
    } else {
      return -1;
    }

    return ptr(s21_lhs_, s21_rhs_);
  }

  int perform_cpp() override {
    if (operation_ == "<") {
      return lhs_ < rhs_;
    } else if (operation_ == "<=") {
      return lhs_ <= rhs_;
    } else if (operation_ == ">") {
      return lhs_ > rhs_;
    } else if (operation_ == ">=") {
      return lhs_ >= rhs_;
    } else if (operation_ == "==") {
      return lhs_ == rhs_;
    } else if (operation_ == "!=") {
      return lhs_ != rhs_;
    }

    return -1;
  }
};

class ComparisonTestLess : public ComparisonTest {
 public:
  ComparisonTestLess(const Decimal& lhs, const Decimal& rhs)
      : ComparisonTest(lhs, rhs, {"<"}) {}
};
class ComparisonTestLessOrEqual : public ComparisonTest {
 public:
  ComparisonTestLessOrEqual(const Decimal& lhs, const Decimal& rhs)
      : ComparisonTest(lhs, rhs, {"<="}) {}
};
class ComparisonTestGreater : public ComparisonTest {
 public:
  ComparisonTestGreater(const Decimal& lhs, const Decimal& rhs)
      : ComparisonTest(lhs, rhs, {">"}) {}
};
class ComparisonTestGreaterOrEqual : public ComparisonTest {
 public:
  ComparisonTestGreaterOrEqual(const Decimal& lhs, const Decimal& rhs)
      : ComparisonTest(lhs, rhs, {">="}) {}
};
class ComparisonTestEqual : public ComparisonTest {
 public:
  ComparisonTestEqual(const Decimal& lhs, const Decimal& rhs)
      : ComparisonTest(lhs, rhs, {"=="}) {}
};
class ComparisonTestNotEqual : public ComparisonTest {
 public:
  ComparisonTestNotEqual(const Decimal& lhs, const Decimal& rhs)
      : ComparisonTest(lhs, rhs, {"!="}) {}
};
}  // namespace s21