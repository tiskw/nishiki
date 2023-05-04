# Makefile

.PHONY: release test check clean cound

PREFIX = ~/.local

gcc: external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 build.py --target nishiki --src ./src --out ./build --link ncursesw --cc gcc

clang: external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 build.py --target nishiki --src ./src --out ./build --link ncursesw --cc clang

external/cxxopts.hpp:
	wget -O ./external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp
	@echo ""

external/toml.hpp:
	wget -O ./external/toml.hpp https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp
	@echo ""

external/glob.hpp:
	wget -O ./external/glob.hpp https://raw.githubusercontent.com/p-ranav/glob/master/single_include/glob/glob.hpp

version.hxx:
	echo '#define VERSION "'`git rev-parse HEAD`'"' > version.hxx

check:
	cppcheck --enable=all -I. --library=posix --suppress=missingIncludeSystem --suppress=useStlAlgorithm src/*.cxx src/*.hxx

clean:
	rm -fr version.hxx ./build ./test/unittest ./test/nishiki-test ./test/*.o ./test/*.gcda ./test/*.gcno ./test/*.gcov

count:
	cloc --by-file src/*

test: external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 build.py --target nishiki-test --src ./src --out ./test --link ncursesw --cc gcc --gcov
	bash test/run_test.bash &
	test/nishiki-test --config test/config.toml
	lcov -c -d test/ -o test/lcov.info
	genhtml test/lcov.info -o test/lcov/

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
