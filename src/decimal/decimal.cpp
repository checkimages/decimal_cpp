#include "decimal.h"

namespace s21 {

Decimal::Decimal(const std::string& input) : Decimal() {
  fromString(input);
  checkValue();
  normalize();
}

Decimal::Decimal(const long& value) noexcept : Decimal() {
  SignedMantissaType mantissa = static_cast<SignedMantissaType>(value);
  if (mantissa < 0) {
    mantissa *= -1;
    sign_ = true;
  }
  mantissa_ = mantissa;
}

Decimal::Decimal(std::initializer_list<uint32_t> values) : error_(0) {
  std::copy(values.begin(),
            values.begin() + std::min(values.size(), static_cast<size_t>(4)),
            bits_);
  sign_ = (bits_[3] >> 31) & 1;
  scale_ = (bits_[3] >> 16) & 0x1F;
  bits_[3] &= ~(0x1F << 16);
  bits_[3] &= ~(1U << 31);

  checkValue();
  normalize();
}

Decimal::Decimal(float value) : Decimal() {
  std::ostringstream oss;
  oss << std::scientific << std::setprecision(6) << value;
  std::string str = oss.str();

  int e_pos = str.find('e');
  std::string mantissa_str = str.substr(0, e_pos);
  std::string exponent = str.substr(e_pos + 1);

  fromString(mantissa_str);
  scale_ = 6 - std::stoi(exponent);

  if (scale_ < 0) {
    do {
      mantissa_ *= 10;
      ++scale_;
    } while (!overflow(mantissa_) && scale_ < 0);
  }

  bool non_zero_value = mantissa_ > 0;
  compression();
  normalize();
  checkValue();

  if (mantissa_ == 0 && non_zero_value) {
    error_ = UnderflowError;
  }
}

Decimal::Decimal(const s21_decimal* s21_d) noexcept : Decimal() {
  const uint32_t* array = reinterpret_cast<const uint32_t*>(s21_d);

  for (int i = 0; i < 3; ++i) {
    bits_[i] = array[i];
  }

  sign_ = (array[3] >> 31) & 1;
  scale_ = (array[3] >> 16) & 0x1F;

  checkValue();
  normalize();
}

Decimal::Decimal(const Decimal& d) noexcept
    : mantissa_(d.mantissa_),
      scale_(d.scale_),
      sign_(d.sign_),
      error_(d.error_) {
  normalize();
}

Decimal& Decimal::operator=(const Decimal& d) noexcept {
  if (this == &d) return *this;

  mantissa_ = d.mantissa_;
  scale_ = d.scale_;
  sign_ = d.sign_;
  error_ = d.error_;
  normalize();

  return *this;
}

Decimal& Decimal::operator=(const std::string& input) {
  sign_ = false;
  scale_ = 0;
  mantissa_ = 0;
  fromString(input);
  checkValue();
  normalize();
  return *this;
}

int Decimal::compare(const Decimal& other) const noexcept {
  if (this == &other) return 0;
  if (mantissa_ == 0 && other.mantissa_ == 0) return 0;

  if (sign_ && !other.sign_) {
    return -1;
  } else if (!sign_ && other.sign_) {
    return 1;
  }

  Decimal lhs = *this;
  Decimal rhs = other;
  lhs.alignScales(rhs);

  if (lhs.mantissa_ == rhs.mantissa_) {
    return 0;
  }

  if (lhs.mantissa_ > rhs.mantissa_) {
    return sign_ ? -1 : 1;
  } else {
    return sign_ ? 1 : -1;
  }
}

Decimal& Decimal::operator+=(const Decimal& other) {
  calculate(other, 1);
  checkValue();
  return *this;
}

Decimal& Decimal::operator-=(const Decimal& other) {
  calculate(other, -1);
  checkValue();
  return *this;
}

Decimal& Decimal::operator*=(const Decimal& other) {
  if (error_) return *this;
  if (other.error_) {
    error_ = other.error_;
    return *this;
  }
  sign_ = sign_ ^ other.sign_;
  scale_ += other.scale_;

  if (mantissa_ > 1 || other.mantissa_ > 1) {
    multiplyWithClamping(other);
  } else {
    mantissa_ *= other.mantissa_;
    compression(true);
    normalize();
    checkValue();
  }

  return *this;
}

Decimal& Decimal::operator/=(const Decimal& other) {
  if (error_) return *this;
  if (other.error_) {
    error_ = other.error_;
    return *this;
  }
  if (other.mantissa_ == 0) {
    error_ = DivisionByZeroError;
    return *this;
  }

  if (mantissa_ == 0) {
    normalize();
    return *this;
  }

  sign_ = sign_ ^ other.sign_;
  scale_ -= other.scale_;
  auto result = mantissa_ / other.mantissa_;

  mantissa_ %= other.mantissa_;

  while (mantissa_ && !overflow(result) && scale_ < 29) {
    mantissa_ *= 10;
    result = result * 10 + mantissa_ / other.mantissa_;
    mantissa_ %= other.mantissa_;
    ++scale_;
  }

  if (overflow(result) || scale_ == 29) {
    int remainder = result % 10;
    result /= 10;
    round_tail(result, remainder, !mantissa_);
    --scale_;
  }

  if (scale_ < 0) {
    do {
      result *= 10;
      ++scale_;
    } while (!overflow(result) && scale_ < 0);
  }

  mantissa_ = result;
  normalize();
  checkValue();

  return *this;
}

Decimal& Decimal::operator%=(const Decimal& other) {
  if (error_) return *this;
  if (other.error_) {
    error_ = other.error_;
    return *this;
  }
  if (other.mantissa_ == 0) {
    error_ = DivisionByZeroError;
    return *this;
  }

  if (scale_ || other.scale_) {
    error_ = ScaleError;
    return *this;
  }

  if (mantissa_ == 0) {
    normalize();
    return *this;
  }

  sign_ = sign_ ^ other.sign_;
  mantissa_ %= other.mantissa_;

  return *this;
}

void Decimal::printBitsUint() const noexcept {
  if (error_) {
    std::cout << "error " << error_ << std::endl;
  } else {
    std::cout << "{ ";
    for (int i = 0; i < 3; ++i) {
      std::cout << bits_[i] << ", ";
    }
    std::cout << getSignAndScale();
    std::cout << " }" << std::endl;
  }
}

void Decimal::printBitsHex() const noexcept {
  if (error_) {
    std::cout << "error " << error_ << std::endl;
  } else {
    std::cout << std::hex << std::uppercase;
    std::cout << "{ ";
    for (int i = 0; i < 3; ++i) {
      std::cout << "0x" << bits_[i] << ", ";
    }
    std::cout << "0x" << getSignAndScale();
    std::cout << " }" << std::endl;
    std::cout << std::dec;
  }
}

void Decimal::printBitsBinary() const noexcept {
  std::cout << "| i |               binary                |" << std::endl;
  std::cout << "-------------------------------------------" << std::endl;
  std::cout << "| " << 3 << " | " << formatBinary(getSignAndScale()) << " |"
            << std::endl;
  for (int i = 2; i >= 0; --i) {
    std::cout << "| " << i << " | " << formatBinary(bits_[i]) << " |"
              << std::endl;
  }
  std::cout << std::endl;
}

std::string Decimal::toString() const noexcept {
  if (error_) return "error " + std::to_string(error_);
  std::string result;
  MantissaType value = mantissa_;
  if (value == 0) {
    result.push_back('0');
  }
  while (value > 0) {
    result.insert(result.begin(), '0' + (value % 10));
    value /= 10;
  }
  if (scale_) {
    size_t decimal_pos = result.size() - scale_;
    if (scale_ >= static_cast<int>(result.size())) {
      result = "0." + std::string(scale_ - result.size(), '0') + result;
    } else {
      result.insert(decimal_pos, ".");
    }
  }
  if (sign_) {
    result.insert(result.begin(), '-');
  }
  return result;
}

std::pair<int, int> Decimal::toInt() const {
  Decimal trunc = truncate();
  SignedMantissaType mantissa = trunc.mantissa_;
  if (sign_) {
    mantissa *= -1;
  }
  int result = static_cast<int>(mantissa);
  mantissa -= static_cast<MantissaType>(result);

  int err_code = 0;

  if (mantissa) {
    err_code = ConvertationError;
  }

  return {result, err_code};
}

std::pair<float, int> Decimal::toFloat() const {
  std::string str = toString();
  float result;
  int err_code = 0;

  try {
    result = std::stof(str);
  } catch (...) {
    err_code = ConvertationError;
    ;
  }

  return {result, err_code};
}

void Decimal::to_s21_decimal(s21_decimal* s21_d) const noexcept {
  uint32_t* array = reinterpret_cast<uint32_t*>(s21_d);

  for (int i = 0; i < 3; ++i) {
    array[i] = bits_[i];
  }

  array[3] = (scale_ & 0xFF) << 16;
  if (sign_) {
    array[3] |= (1 << 31);
  }
}

Decimal Decimal::round() const noexcept {
  if (!scale_) {
    return *this;
  }

  Decimal result = *this;
  int remainder;
  int count = 0;

  while (result.scale_ > 0) {
    remainder = result.mantissa_ % 10;
    result.mantissa_ /= 10;
    ++count;
    --result.scale_;
  }

  if (count) {
    result.round_tail(result.mantissa_, remainder, false);
  }

  return result;
}

Decimal Decimal::floor() const noexcept {
  if (!scale_) {
    return *this;
  }

  Decimal result = truncate();

  if (result.sign_) {
    ++result.mantissa_;
  }

  return result;
}

Decimal Decimal::truncate() const noexcept {
  if (!scale_) {
    return *this;
  }

  Decimal result = *this;
  while (result.scale_ > 0) {
    result.mantissa_ /= 10;
    --result.scale_;
  }

  return result;
}

Decimal Decimal::negate() const noexcept {
  Decimal result = *this;
  result.sign_ = !sign_;

  return result;
}

void Decimal::calculate(Decimal other, int8_t factor) {
  if (error_) return;
  if (other.error_) {
    error_ = other.error_;
    return;
  }

  alignScales(other);
  SignedMantissaType mantissa = static_cast<SignedMantissaType>(mantissa_);
  if (sign_) {
    mantissa *= -1;
  }
  SignedMantissaType other_mantissa =
      static_cast<SignedMantissaType>(other.mantissa_);
  if (other.sign_) {
    other_mantissa *= -1;
  }
  mantissa += other_mantissa * factor;

  if (mantissa < 0) {
    sign_ = true;
    mantissa *= -1;
  } else {
    sign_ = false;
  }
  mantissa_ = mantissa;
  compression(true);
  normalize();
}

void Decimal::multiplyWithClamping(const Decimal& other) {
  uint64_t high = mantissa_ >> 64;
  uint64_t low = static_cast<uint64_t>(mantissa_);
  uint64_t other_high = other.mantissa_ >> 64;
  uint64_t other_low = static_cast<uint64_t>(other.mantissa_);

  auto low_low = static_cast<MantissaType>(low) * other_low;
  auto low_high = static_cast<MantissaType>(low) * other_high;
  auto high_low = static_cast<MantissaType>(high) * other_low;
  auto high_high = static_cast<MantissaType>(high) * other_high;

  Term t({{low_low, 0}, {low_high, 1}, {high_low, 1}, {high_high, 2}});

  bool success = t.data_->size() == 1;
  while (scale_ && !success) {
    success = t.attempt();
    --scale_;
  }

  if (!success) {
    if (sign_) {
      error_ = NegativeOverflowError;
      return;
    } else {
      error_ = PositiveOverflowError;
      return;
    }
  }

  mantissa_ = t.data_->front().first;
  bool remainder_discarded = t.carry_->size() == 0;
  compression(remainder_discarded);
  normalize();
  checkValue();
}

void Decimal::normalize() noexcept {
  if (error_) return;

  if (mantissa_ == 0) {
    // sign_ = false;
    scale_ = 0;
  }
  while (mantissa_ % 10 == 0 && mantissa_ > 0 && scale_ > 0) {
    mantissa_ /= 10;
    scale_--;
  }
}

void Decimal::round_tail(MantissaType& value, int remainder,
                         bool bank_round) noexcept {
  if (remainder == 5 && bank_round) {
    if (value % 2 != 0) value++;
  } else if (remainder > 4) {
    value++;
  }
}

void Decimal::compression(bool bank_round) noexcept {
  int remainder;
  int count = 0;

  while (mantissa_ > 0 && scale_ > 28) {
    remainder = mantissa_ % 10;
    mantissa_ /= 10;
    ++count;
    --scale_;
  }

  while (overflow(mantissa_) && scale_ > 0) {
    remainder = mantissa_ % 10;
    mantissa_ /= 10;
    ++count;
    --scale_;
  }

  if (count) {
    round_tail(mantissa_, remainder, count == 1 && bank_round);
  }
}

uint32_t Decimal::getSignAndScale() const noexcept {
  uint32_t result = (scale_ & 0xFF) << 16;
  if (sign_) {
    result |= (1U << 31);
  }

  result |= error_;

  return result;
}

void Decimal::alignScales(Decimal& other) {
  while (scale_ > other.scale_ && !overflow(other.mantissa_)) {
    other.mantissa_ *= 10;
    ++other.scale_;
  }
  while (scale_ < other.scale_ && !overflow(mantissa_)) {
    mantissa_ *= 10;
    ++scale_;
  }

  int remainder, count = 0;
  if (scale_ > other.scale_) {
    while (scale_ > other.scale_) {
      remainder = mantissa_ % 10;
      mantissa_ /= 10;
      --scale_;
      ++count;
    }
    round_tail(mantissa_, remainder, count == 1);
  } else if (scale_ < other.scale_) {
    while (scale_ < other.scale_) {
      remainder = other.mantissa_ % 10;
      other.mantissa_ /= 10;
      --other.scale_;
      ++count;
    }
    round_tail(mantissa_, remainder, count == 1);
  }
}

bool Decimal::overflow(MantissaType value) const noexcept {
  return (value >> 96) != 0;
}

void Decimal::checkValue() {
  if (overflow(mantissa_)) {
    if (sign_) {
      error_ = NegativeOverflowError;
      return;
    } else {
      error_ = PositiveOverflowError;
      return;
    }
  }
  if (scale_ < 0 || scale_ > 28) {
    error_ = ScaleError;
  }
}

void Decimal::fromString(const std::string& input) {
  bool decimal_separator_found = false;
  for (char ch : input) {
    if (ch == '-') {
      sign_ = true;
    } else if (ch == '+') {
      sign_ = false;
    } else if (ch == '.') {
      decimal_separator_found = true;
    } else if (std::isdigit(ch)) {
      mantissa_ = mantissa_ * 10 + (ch - '0');
      if (decimal_separator_found) {
        ++scale_;
      }
      if (overflow(mantissa_)) {
        break;
      }
    } else {
      throw std::invalid_argument("Invalid character in input string");
    }
  }
}

std::string Decimal::formatBinary(uint32_t value) const noexcept {
  std::bitset<32> binary(value);
  std::string binaryStr = binary.to_string();
  std::string formatted;

  for (size_t j = 0; j < binaryStr.size(); ++j) {
    formatted += binaryStr[j];
    if ((j + 1) % 8 == 0 && j != binaryStr.size() - 1) {
      formatted += " ";
    }
  }
  return formatted;
}

Decimal::Term::Term(std::initializer_list<PairType> const& items)
    : data_(std::make_unique<std::list<PairType>>(items)),
      carry_(std::make_unique<std::deque<int>>()) {
  enlargement();
}

bool Decimal::Term::attempt() {
  division_by_ten();
  enlargement();

  return data_->size() == 1;
}

void Decimal::Term::enlargement() {
  auto it = data_->begin();
  auto& aggregator = *it;
  ++it;
  while (it != data_->end()) {
    if ((*it).second > 0 && (*it).first >> kShift == 0) {
      (*it).first = ((*it).first << kShift);
      (*it).second -= 1;
    }
    if ((*it).first == 0) {
      it = data_->erase(it);
    } else if ((*it).second == 0 && (kMax - aggregator.first > (*it).first)) {
      aggregator.first += (*it).first;
      it = data_->erase(it);
    } else {
      ++it;
    }
  }
}

void Decimal::Term::division_by_ten() {
  MantissaType carry = 0;
  for (auto it = data_->begin(); it != data_->end(); ++it) {
    if ((*it).second == 0) {
      carry += (*it).first % 10;
      (*it).first /= 10;
    } else {
      data_->push_back({((*it).first % 10) << kShift, (*it).second - 1});
      (*it).first /= 10;
    }
  }
  if (carry >= 10) {
    data_->push_back({carry / 10, 0});
    carry %= 10;
  }

  carry_->push_front(static_cast<int>(carry));
}

std::ostream& operator<<(std::ostream& os, const Decimal& decimal) {
  os << decimal.toString();
  return os;
}

Decimal operator"" _d(const char* str, size_t) {
  return Decimal(std::string(str));
}

Decimal operator+(const Decimal& lhs, const Decimal& rhs) {
  Decimal result(lhs);
  result += rhs;
  return result;
}

Decimal operator-(const Decimal& lhs, const Decimal& rhs) {
  Decimal result(lhs);
  result -= rhs;
  return result;
}

Decimal operator*(const Decimal& lhs, const Decimal& rhs) {
  Decimal result(lhs);
  result *= rhs;
  return result;
}

Decimal operator/(const Decimal& lhs, const Decimal& rhs) {
  Decimal result(lhs);
  result /= rhs;
  return result;
}

Decimal operator%(const Decimal& lhs, const Decimal& rhs) {
  Decimal result(lhs);
  result %= rhs;
  return result;
}

}  // namespace s21