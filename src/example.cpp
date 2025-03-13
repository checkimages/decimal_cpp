#include "decimal/decimal.h"

using namespace s21;

int main() try {
  std::cout << "\nКонструкторы\n" << std::endl;

  s21::Decimal d1;
  s21::Decimal d2(d1);
  s21::Decimal d3("79228162514264337593543950335");
  s21::Decimal d4{1, 2, 3, 0};
  s21::Decimal d5{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x801C0000};
  s21::Decimal d6(2147483648);
  s21::Decimal d7(3.1415926535f);
  s21_decimal s21_decimal_var = {{1, 2, 3, 0}};
  s21::Decimal d8(&s21_decimal_var);
  s21::Decimal d9 = "-7.9228162514264337593543950335"_d;
  s21::Decimal d10(d9);

  std::cout << "\nПечать\n" << std::endl;

  d1.printBitsBinary();
  d5.printBitsUint();
  d5.printBitsHex();
  d5.print();
  std::cout << d5 << std::endl;
  "-7.9228162514264337593543950335"_d.printBitsBinary();

  std::cout << "Арифметические операции\n" << std::endl;

  std::cout << d4 + d5 << std::endl;
  std::cout << "-7.9228162514264337593543950335"_d + "-7.9"_d << std::endl;
  std::cout << "-7.9228162514264337593543950335"_d + 2147483648 << std::endl;

  std::cout << "\nОперации сравнения\n" << std::endl;

  std::cout << (d4 >= d5) << std::endl;
  std::cout << ("-7.9228162514264337593543950335"_d == "-7.9"_d) << std::endl;
  std::cout << ("-7.9228162514264337593543950335"_d < 2147483648) << std::endl;

  std::cout << "\nПреобразователи\n" << std::endl;

  auto [ivar, err_code] = "-2147483648"_d.toInt();
  std::cout << "result: " << ivar << ", " << err_code << std::endl;

  auto [fvar, err_code2] = "3.1415926535"_d.toFloat();
  std::cout << std::setprecision(11) << "result: " << fvar << ", " << err_code2
            << std::endl;

  std::cout << "-7.9228162514264337593543950335"_d.toString() << std::endl;

  s21_decimal result;
  "-7.9228162514264337593543950335"_d.to_s21_decimal(&result);

  std::cout << "\nДругие функции\n" << std::endl;

  std::cout << "3.1415926535"_d.round() << std::endl;
  std::cout << "-3.1415926535"_d.floor() << std::endl;
  std::cout << "3.999999999"_d.truncate() << std::endl;
  std::cout << "3.1415926535"_d.negate() << std::endl;

} catch (const std::exception &e) {
  std::cerr << e.what() << std::endl;
}