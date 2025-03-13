CXX						= g++
CXXFLAGS				= -g -Wall -Werror -Wextra -std=c++17 -Wfatal-errors
CLANG_OPT 				= -iname '*.c' -o -iname '*.h' -o -iname '*.cc' -o -iname '*.cpp'
S21_LIB					= ~/project/C/C5_s21_decimal-1/src/s21_decimal.a

example:
	@$(CXX) $(CXXFLAGS) example.cpp decimal/decimal.cpp $(S21_LIB)
	@./a.out

test:
	@$(CXX) $(CXXFLAGS) tests/test.cpp decimal/decimal.cpp $(S21_LIB)
	@./a.out
#	@valgrind ./a.out 2>valgrind.log

test_loop:
	@$(CXX) $(CXXFLAGS) tests/test_loop.cpp decimal/decimal.cpp $(S21_LIB)
	@./a.out
#	@valgrind ./a.out 2>valgrind.log

clean:
	@rm -rf *.out
	@rm -rf *.log
	@echo "cleaning completed"

check_clang:
	find . \( $(CLANG_OPT) \) -exec clang-format -n --style=Google --Werror {} \;

format_clang:
	find . \( $(CLANG_OPT) \) -exec clang-format -i --style=Google {} \;