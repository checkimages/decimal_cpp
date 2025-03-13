#include "arithmetic_test.h"
#include "comparison_test.h"
#include "convertors_test.h"
#include "other_test.h"

using namespace s21;

void loop_test_decimal(const Decimal arr[], size_t size) {
  for (size_t i = 0; i < size; ++i) {
    run_test<OtherTest>(arr[i]);
    run_test<FromDecimalTest>(arr[i]);
    for (size_t j = 0; j < size; ++j) {
      run_test<ArithmeticTest>(arr[i], arr[j]);
      run_test<ComparisonTest>(arr[i], arr[j]);
    }
  }
}

void loop_test_float(const float arr[], size_t size) {
  for (size_t i = 0; i < size; ++i) {
    run_test<FromFloatTest>(arr[i]);
  }
}

void loop_test_int(const int arr[], size_t size) {
  for (size_t i = 0; i < size; ++i) {
    run_test<FromIntTest>(arr[i]);
  }
}

int main() try {
  const Decimal decimal_arr[] = {"-7.9228162514264337593543950335"_d,
                                 "79228162514264337593543950335"_d,
                                 "-79228162514264337593543950335"_d,
                                 "-0.4"_d,
                                 "-0.5"_d,
                                 "-0.6"_d,
                                 "-0"_d,
                                 "0"_d,
                                 "1"_d,
                                 "2"_d,
                                 "-1"_d,
                                 "6"_d,
                                 "8"_d,
                                 "3"_d,
                                 "-0.0000000000000000000000000001"_d,
                                 "453435434.434343"_d,
                                 "792281625142643.37593543950335"_d};

  const float float_arr[] = {-7.9228162514264337593543950335f,
                             79228162514264337593543950335.0f,
                             -79228162514264337593543950335.0f,
                             -0.5f,
                             453435434.434343f,
                             -0.0000000000000000000000000001f,
                             -0.00000000000000000000000000001f,
                             79228172514264337593543950335.0f,
                             1.175494e-38,
                             3.402823e+38,
                             3541183.9035615,
                             -0.0f,
                             0.0f};

  const int int_arr[] = {0,  1,       2147483647, -2147483648,
                         -1, -215456, 1651689541};

  // BaseTest::PrintAllResults = true;
  loop_test_decimal(decimal_arr, sizeof(decimal_arr) / sizeof(decimal_arr[0]));
  loop_test_float(float_arr, sizeof(float_arr) / sizeof(float_arr[0]));
  loop_test_int(int_arr, sizeof(int_arr) / sizeof(int_arr[0]));

  general_results();

} catch (const std::exception& e) {
  std::cout << e.what() << std::endl;
}