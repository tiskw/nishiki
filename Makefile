# Makefile

.PHONY: gcc clang test check count clean

CWD = $(shell pwd)

gcc: external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 build.py --target nishiki --src ./src --out ./build --link ncursesw --cc gcc

clang: external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 build.py --target nishiki --src ./src --out ./build --link ncursesw --cc clang

external/cxxopts.hpp:
	wget -q -O ./external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp

external/toml.hpp:
	wget -q -O ./external/toml.hpp https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp

external/glob.hpp:
	wget -q -O ./external/glob.hpp https://raw.githubusercontent.com/p-ranav/glob/master/single_include/glob/glob.hpp

version.hxx:
	echo '#define VERSION "'`git rev-parse HEAD`'"' > version.hxx

test:
	cd tests; make test; cd ..

check: version.hxx
	cppcheck --enable=all -I. -I./src --library=posix --suppress=missingIncludeSystem src/*.cxx

count:
	cloc --by-file src/*.cxx src/*.hxx

clean:
	rm -fr version.hxx ./build
	cd tests; make clean; cd ..

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
