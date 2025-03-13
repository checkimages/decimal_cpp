#include "arithmetic_test.h"
#include "comparison_test.h"
#include "convertors_test.h"
#include "other_test.h"

using namespace s21;

int main() try {
  BaseTest::PrintAllResults = false;

  run_test<OtherTest>("-79228162514264337593543.950335"_d);

  run_test<OtherTestNegate>("792281625142643.37593543950335"_d);

  run_test<ArithmeticTest>("79228162514264337593543950335"_d, "-0.5"_d, "+");

  run_test<OtherTest>("792281625142643.37593543950335"_d, "floor");

  run_test<FromDecimalTest>("7922816531.37593543950335"_d);

  run_test<FromFloatTest>(79228172514264337593543950335.0f);

  run_test<FromIntTest>(1520);

  run_test<ArithmeticTest>("792281625142643.37593543950335"_d,
                           "453435434.434343"_d, "*");

  run_test<ArithmeticTestMul>("792281625142643.37593543950335"_d,
                              "453435434.434343"_d);

  run_test<ComparisonTest>("792281625142643.37593543950335"_d,
                           "453435434.434343"_d, "!=");

  general_results();

} catch (const std::exception& e) {
  std::cout << e.what() << std::endl;
}