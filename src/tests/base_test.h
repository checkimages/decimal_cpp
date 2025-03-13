#pragma once

#include <iomanip>
#include <optional>
#include <vector>

#include "../decimal/decimal.h"

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"

// using namespace s21;

namespace s21 {

class BaseTest {
 public:
  static bool PrintAllResults;

  BaseTest(const std::vector<std::string>& operations)
      : operations_(operations) {}
  virtual ~BaseTest() = default;

  bool run() {
    bool test_result = true;
    for (const auto& operation : operations_) {
      operation_ = operation;
      test_result &= run_operation();
    }
    return test_result;
  }

  static void printTotal() {
    bool hasErrors = (count_ != success_count_);
    const char* color = hasErrors ? RED : GREEN;

    std::cout << color;
    std::cout
        << "==============================================================="
        << std::endl;
    std::cout << "| Total              | " << std::setw(38) << count_ << " |"
              << std::endl;
    std::cout
        << "---------------------------------------------------------------"
        << std::endl;
    std::cout << "| Success            | " << std::setw(38) << success_count_
              << " |" << std::endl;
    std::cout << "| Fail               | " << std::setw(38)
              << (count_ - success_count_) << " |" << std::endl;
    std::cout << std::endl;
    std::cout << RESET;
  }

 protected:
  const std::vector<std::string> operations_;
  std::string operation_;
  int code_, s21_code_;

  inline static int count_ = 0;
  inline static int success_count_ = 0;

  virtual void printArgs() = 0;
  virtual int perform_c() = 0;
  virtual int perform_cpp() = 0;
  virtual bool checking() = 0;

  static s21_decimal convert_to_s21(const Decimal& value) {
    s21_decimal s;
    value.to_s21_decimal(&s);
    return s;
  }

  bool run_operation() {
    s21_code_ = perform_c();
    code_ = perform_cpp();

    bool test_result = checking();
    success_count_ += test_result;
    ++count_;

    return test_result;
  }

  bool checkingCode() {
    if (code_ != s21_code_) {
      printResults<int>(false);
      return false;
    }

    return true;
  }

  template <typename T>
  void printResults(bool test_result,
                    const std::optional<T>& result = std::nullopt,
                    const std::optional<T>& cmp_result = std::nullopt) {
    std::cout
        << "==============================================================="
        << std::endl;
    std::cout << "| ";
    const char* color = test_result ? GREEN : RED;
    std::cout << color;
    std::cout << "test # " << std::setw(5) << std::left << (count_ + 1);
    std::cout << std::boolalpha << std::setw(7) << std::left;
    printTestResult(test_result);
    std::cout << RESET;
    std::cout << "| operation: " << std::setw(21) << std::left << operation_
              << "| code |" << std::endl;
    std::cout
        << "---------------------------------------------------------------"
        << std::endl;
    printArgs();
    std::cout
        << "---------------------------------------------------------------"
        << std::endl;

    std::cout << "| Decimal     result | " << std::setw(31) << std::right;
    printValue(result);
    std::cout << " | " << std::setw(4) << code_ << " |" << std::endl;

    std::cout << "| s21_decimal result | " << std::setw(31) << std::right;
    printValue(cmp_result);
    std::cout << " | " << std::setw(4) << s21_code_ << " |" << std::endl;

    std::cout << std::endl;
  }

  template <typename T>
  void printValue(const std::optional<T>& value) {
    if (value) {
      if (typeid(T) == typeid(float)) {
        std::cout << std::scientific << std::setprecision(6) << *value;
      } else {
        std::cout << *value;
      }
    } else {
      std::cout << " ";
    }
  }

  void printTestResult(bool test_result) {
    if (test_result) {
      std::cout << "[ok]";
    } else {
      std::cout << "[fail]";
    }
  }
};

template <typename TestType, typename... Args>
void run_test(Args&&... args) {
  TestType test(std::forward<Args>(args)...);
  test.run();
}

void general_results() { BaseTest::printTotal(); }

bool BaseTest::PrintAllResults = false;

}  // namespace s21