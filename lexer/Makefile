CXX=clang++
WARNINGS = -Wall -Wextra -pedantic -Wno-unused-parameter
CXXFLAGS = -std=c++11 -D__STRICT_ANSI__ $(WARNINGS)
TARGET = compiler

objs = lexer.o main.o

compiler : $(objs)
	$(CXX) $(objs) -o $(TARGET)

clean :
	rm -f $(TARGET) *.o *~

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

cppcheck :
	cppcheck --enable=all --inconclusive ./ 1> /dev/null

analyze : clean
	scan-build make compiler

tests : compiler
	cd ../tests; ./unitTest.py $(UNITTEST_ARG)
