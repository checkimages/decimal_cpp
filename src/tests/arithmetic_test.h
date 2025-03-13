#pragma once

#include "base_test.h"

namespace s21 {

class ArithmeticTest : public BaseTest {
 public:
  ArithmeticTest(const Decimal& lhs, const Decimal& rhs,
                 const std::vector<std::string>& operations = {"+", "-", "*",
                                                               "/"})
      : BaseTest(operations),
        lhs_(lhs),
        rhs_(rhs),
        s21_lhs_(convert_to_s21(lhs)),
        s21_rhs_(convert_to_s21(rhs)) {}

  ArithmeticTest(const Decimal& lhs, const Decimal& rhs, const char* operation)
      : ArithmeticTest(lhs, rhs, std::vector<std::string>{operation}) {}

 protected:
  const Decimal lhs_, rhs_;
  const s21_decimal s21_lhs_, s21_rhs_;
  Decimal result_;
  s21_decimal s21_result_;

  void printArgs() override {
    std::cout << "| value_1            | " << std::setw(31) << std::right
              << lhs_ << " |      |" << std::endl;
    std::cout << "| value_2            | " << std::setw(31) << std::right
              << rhs_ << " |      |" << std::endl;
  }

  bool checking() override {
    if (!checkingCode()) {
      return false;
    }

    if (!code_) {
      Decimal cmp_result(&s21_result_);
      if (result_ != cmp_result) {
        printResults<Decimal>(false, result_, cmp_result);
        return false;
      }
      if (PrintAllResults) {
        printResults<Decimal>(true, result_, cmp_result);
      }
    } else {
      if (PrintAllResults) {
        printResults<int>(true);
      }
    }

    return true;
  }

  int perform_c() override {
    int (*ptr)(s21_decimal, s21_decimal, s21_decimal*);

    if (operation_ == "+") {
      ptr = s21_add;
    } else if (operation_ == "-") {
      ptr = s21_sub;
    } else if (operation_ == "*") {
      ptr = s21_mul;
    } else if (operation_ == "/") {
      ptr = s21_div;
    } else {
      return -1;
    }

    return ptr(s21_lhs_, s21_rhs_, &s21_result_);
  }

  int perform_cpp() override {
    if (operation_ == "+") {
      result_ = lhs_ + rhs_;
    } else if (operation_ == "-") {
      result_ = lhs_ - rhs_;
    } else if (operation_ == "*") {
      result_ = lhs_ * rhs_;
    } else if (operation_ == "/") {
      result_ = lhs_ / rhs_;
    } else {
      return -1;
    }

    return result_.getErrorCode();
  }
};

class ArithmeticTestAdd : public ArithmeticTest {
 public:
  ArithmeticTestAdd(const Decimal& lhs, const Decimal& rhs)
      : ArithmeticTest(lhs, rhs, {"+"}) {}
};
class ArithmeticTestSub : public ArithmeticTest {
 public:
  ArithmeticTestSub(const Decimal& lhs, const Decimal& rhs)
      : ArithmeticTest(lhs, rhs, {"-"}) {}
};
class ArithmeticTestMul : public ArithmeticTest {
 public:
  ArithmeticTestMul(const Decimal& lhs, const Decimal& rhs)
      : ArithmeticTest(lhs, rhs, {"*"}) {}
};
class ArithmeticTestDiv : public ArithmeticTest {
 public:
  ArithmeticTestDiv(const Decimal& lhs, const Decimal& rhs)
      : ArithmeticTest(lhs, rhs, {"/"}) {}
};

}  // namespace s21