#pragma once

#include "base_test.h"

namespace s21 {

class OtherTest : public BaseTest {
 public:
  OtherTest(const Decimal& value,
            const std::vector<std::string>& operations = {"floor", "round",
                                                          "truncate", "negate"})
      : BaseTest(operations),
        value_(value),
        s21_value_(convert_to_s21(value)) {}

  OtherTest(const Decimal& value, const char* operation)
      : OtherTest(value, std::vector<std::string>{operation}) {}

 protected:
  const Decimal value_;
  const s21_decimal s21_value_;
  Decimal result_;
  s21_decimal s21_result_;

  void printArgs() override {
    std::cout << "| value              | " << std::setw(31) << std::right
              << value_ << " |      |" << std::endl;
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
    int (*ptr)(s21_decimal, s21_decimal*);

    if (operation_ == "floor") {
      ptr = s21_floor;
    } else if (operation_ == "round") {
      ptr = s21_round;
    } else if (operation_ == "truncate") {
      ptr = s21_truncate;
    } else if (operation_ == "negate") {
      ptr = s21_negate;
    } else {
      return -1;
    }

    return ptr(s21_value_, &s21_result_);
  }

  int perform_cpp() override {
    if (operation_ == "floor") {
      result_ = value_.floor();
    } else if (operation_ == "round") {
      result_ = value_.round();
    } else if (operation_ == "truncate") {
      result_ = value_.truncate();
    } else if (operation_ == "negate") {
      result_ = value_.negate();
    }

    return 0;
  }
};

class OtherTestFloor : public OtherTest {
 public:
  OtherTestFloor(const Decimal& value) : OtherTest(value, {"floor"}) {}
};
class OtherTestRound : public OtherTest {
 public:
  OtherTestRound(const Decimal& value) : OtherTest(value, {"round"}) {}
};
class OtherTestTruncate : public OtherTest {
 public:
  OtherTestTruncate(const Decimal& value) : OtherTest(value, {"truncate"}) {}
};
class OtherTestNegate : public OtherTest {
 public:
  OtherTestNegate(const Decimal& value) : OtherTest(value, {"negate"}) {}
};

}  // namespace s21