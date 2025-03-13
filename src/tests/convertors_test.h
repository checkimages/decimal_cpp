#pragma once

#include "base_test.h"

namespace s21 {

class FromDecimalTest : public BaseTest {
 public:
  FromDecimalTest(const Decimal& value,
                  const std::vector<std::string>& operations =
                      {"from_decimal_to_int", "from_decimal_to_float"})
      : BaseTest(operations),
        value_(value),
        s21_value_(convert_to_s21(value)) {}

  FromDecimalTest(const Decimal& value, const char* operation)
      : FromDecimalTest(value, std::vector<std::string>{operation}) {}

 protected:
  const Decimal value_;
  const s21_decimal s21_value_;
  union {
    int int_result_;
    float float_result_;
  };
  union {
    int s21_int_result_;
    float s21_float_result_;
  };

  void printArgs() override {
    std::cout << "| value              | " << std::setw(31) << std::right
              << value_ << " |      |" << std::endl;
  }

  bool checking() override {
    if (!checkingCode()) {
      return false;
    }

    if (!code_) {
      if (operation_ == "from_decimal_to_int") {
        if (int_result_ != s21_int_result_) {
          printResults<int>(int_result_, s21_int_result_, false);
          return false;
        }
        if (PrintAllResults) {
          printResults<int>(int_result_, s21_int_result_, true);
        }
      } else {
        if (float_result_ != s21_float_result_) {
          printResults<float>(float_result_, s21_float_result_, false);
          return false;
        }
        if (PrintAllResults) {
          printResults<float>(float_result_, s21_float_result_, true);
        }
      }
    } else {
      if (PrintAllResults) {
        printResults<int>(true);
      }
    }

    return true;
  }

  int perform_c() override {
    if (operation_ == "from_decimal_to_int") {
      return s21_from_decimal_to_int(s21_value_, &s21_int_result_);
    } else if (operation_ == "from_decimal_to_float") {
      return s21_from_decimal_to_float(s21_value_, &s21_float_result_);
    }

    return -1;
  }

  int perform_cpp() override {
    if (operation_ == "from_decimal_to_int") {
      auto [result, err_code] = value_.toInt();
      int_result_ = result;
      return err_code ? 1 : 0;
    } else if (operation_ == "from_decimal_to_float") {
      auto [result, err_code] = value_.toFloat();
      float_result_ = result;
      return err_code ? 1 : 0;
    }

    return 1;
  }
};

class FromDecimalTestToInt : public FromDecimalTest {
 public:
  FromDecimalTestToInt(const Decimal& value)
      : FromDecimalTest(value, {"from_decimal_to_int"}) {}
};
class FromDecimalTestToFloat : public FromDecimalTest {
 public:
  FromDecimalTestToFloat(const Decimal& value)
      : FromDecimalTest(value, {"from_decimal_to_float"}) {}
};

class FromFloatTest : public BaseTest {
 public:
  FromFloatTest(const float& value)
      : BaseTest({"from_float_to_decimal"}), value_(value) {}

 protected:
  const float value_;
  Decimal result_;
  s21_decimal s21_result_;

  void printArgs() override {
    std::cout << "| value              | " << std::setw(31) << std::right;
    std::cout << std::scientific << std::setprecision(6) << value_
              << " |      |" << std::endl;
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
    return s21_from_float_to_decimal(value_, &s21_result_);
  }

  int perform_cpp() override {
    result_ = Decimal(value_);
    if (result_.getErrorCode()) {
      return 1;
    }

    return 0;
  }
};

class FromIntTest : public BaseTest {
 public:
  FromIntTest(const int& value)
      : BaseTest({"from_int_to_decimal"}), value_(value) {}

 protected:
  const int value_;
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
    return s21_from_int_to_decimal(value_, &s21_result_);
  }

  int perform_cpp() override {
    result_ = Decimal(static_cast<long>(value_));

    return 0;
  }
};

}  // namespace s21