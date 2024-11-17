CXX := clang++
CXXFLAGS := -Wall -Werror -std=c++20 -g3 -O0 -static

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

riscy: elf.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

examples:
	riscv64-linux-gnu-gcc examples/quad.c -nostdlib -march=rv64g -fPIC -S -o examples/quad.s -Oz
	riscv64-linux-gnu-gcc examples/quad.s -nostdlib -march=rv64g -shared -s -fPIC -o examples/quad.so
# riscv64-linux-gnu-gcc examples/quad.s -c -s -o examples/quad.o
.PHONY: examples

clean:
	rm -fv examples/*.s examples/*.o *.o riscy
.PHONY: clean
