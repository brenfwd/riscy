CXX := clang++
CXXFLAGS := -Wall -Werror -std=c++20 -g3 -O0

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

riscy: buffer.o elf.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

examples:
	riscv64-linux-gnu-gcc examples/quad.c -S -o examples/quad.s -Oz
	riscv64-linux-gnu-gcc examples/quad.s -shared -o examples/quad.so
.PHONY: examples

clean:
	rm examples/*.s examples/*.o
	rm *.o
.PHONY: clean
