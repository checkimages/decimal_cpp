#pragma once

#include <bitset>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>

#define S21_PATH "../s21_decimal.h"

#if __has_include(S21_PATH)
extern "C" {
#include S21_PATH
}
#else
typedef struct {
  uint32_t bits[4];
} s21_decimal;
#endif

namespace s21 {

class Decimal {
 public:
  using MantissaType = __uint128_t;
  using SignedMantissaType = __int128;

  enum Errors {
    NoErrors,
    PositiveOverflowError,
    NegativeOverflowError,
    DivisionByZeroError,
    UnderflowError,
    ConvertationError,
    ScaleError
  };

  Decimal() noexcept : mantissa_(0), scale_(0), sign_(false), error_(0) {}
  explicit Decimal(const std::string& input);
  Decimal(const long& value) noexcept;
  Decimal(std::initializer_list<uint32_t> values);
  explicit Decimal(float value);
  explicit Decimal(const s21_decimal* s21_d) noexcept;
  Decimal(const Decimal& d) noexcept;

  Decimal& operator=(const Decimal& d) noexcept;
  Decimal& operator=(const std::string& input);

  int getErrorCode() const noexcept { return error_; }
  int compare(const Decimal& other) const noexcept;
  bool operator<(const Decimal& other) const noexcept {
    return compare(other) == -1;
  }
  bool operator<=(const Decimal& other) const noexcept {
    return compare(other) < 1;
  }
  bool operator>(const Decimal& other) const noexcept {
    return compare(other) == 1;
  }
  bool operator>=(const Decimal& other) const noexcept {
    return compare(other) > -1;
  }
  bool operator==(const Decimal& other) const noexcept {
    return compare(other) == 0;
  }
  bool operator!=(const Decimal& other) const noexcept {
    return compare(other) != 0;
  }

  Decimal& operator+=(const Decimal& other);
  Decimal& operator-=(const Decimal& other);
  Decimal& operator*=(const Decimal& other);
  Decimal& operator/=(const Decimal& other);
  Decimal& operator%=(const Decimal& other);

  void print() const { std::cout << toString() << std::endl; }

  void printBitsUint() const noexcept;
  void printBitsHex() const noexcept;
  void printBitsBinary() const noexcept;

  std::string toString() const noexcept;
  std::pair<int, int> toInt() const;
  std::pair<float, int> toFloat() const;
  void to_s21_decimal(s21_decimal* s21_d) const noexcept;

  Decimal round() const noexcept;
  Decimal floor() const noexcept;
  Decimal truncate() const noexcept;
  Decimal negate() const noexcept;

 private:
  union {
    MantissaType mantissa_;
    struct {
      uint32_t bits_[4];
    };
  };

  int scale_;
  bool sign_;
  int error_;

  friend std::ostream& operator<<(std::ostream& os, const Decimal& decimal);

  void calculate(Decimal other, int8_t factor);
  void multiplyWithClamping(const Decimal& other);
  void normalize() noexcept;
  void round_tail(MantissaType& value, int remainder,
                  bool bank_round = false) noexcept;
  void compression(bool bank_round = false) noexcept;
  uint32_t getSignAndScale() const noexcept;
  void alignScales(Decimal& other);
  bool overflow(MantissaType value) const noexcept;
  void checkValue();
  void fromString(const std::string& input);
  std::string formatBinary(uint32_t value) const noexcept;

  // helper struct for multiplication with clamping
  struct Term {
    using PairType = std::pair<MantissaType, uint8_t>;

    constexpr static int kShift = sizeof(MantissaType) * 8 / 2;
    constexpr static MantissaType kMax = ~static_cast<MantissaType>(0);

    std::unique_ptr<std::list<PairType>> data_;
    std::unique_ptr<std::deque<int>> carry_;

    Term(std::initializer_list<PairType> const& items);

    bool attempt();
    void enlargement();
    void division_by_ten();
  };
};

Decimal operator"" _d(const char* str, size_t);
Decimal operator+(const Decimal& lhs, const Decimal& rhs);
Decimal operator-(const Decimal& lhs, const Decimal& rhs);
Decimal operator*(const Decimal& lhs, const Decimal& rhs);
Decimal operator/(const Decimal& lhs, const Decimal& rhs);
Decimal operator%(const Decimal& lhs, const Decimal& rhs);

}  // namespace s21
