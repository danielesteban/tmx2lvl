all: libs main

libs:
	cd tmxparser/ && clang++ -c -I tinyxml2 *.cpp base64/*.cpp tinyxml2/*.cpp \
	&& ar rcs libtmxparser.a *.o

main:
	clang++ main.cpp -pedantic -Wall -Wextra -march=native -ggdb3 -O3 -D NDEBUG \
	-I tmxparser -L tmxparser -ltmxparser \
	-o tmx2lvl

clean:
	-rm -f tmx2lvl
	-rm tmxparser/*.o
	-rm tmxparser/*.a