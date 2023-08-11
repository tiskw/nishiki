# Makefile

.PHONY: release test check clean cound

PREFIX = ~/.local
CWD = $(shell pwd)

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

test: test/nishiki-test
	make test1
	make test2
	make test3
	make test4
	make test5
	make test6
	lcov -c -d test/ -o test/lcov.info
	genhtml test/lcov.info -o test/lcov/

test1: test/nishiki-test
	bash test/scripts/run_test1.bash &
	test/nishiki-test --config test/config.toml

test2: test/nishiki-test
	bash test/scripts/run_test2.bash &
	test/nishiki-test

test3: test/nishiki-test
	bash test/scripts/run_test3.bash &
	cd ~ && $(CWD)/test/nishiki-test

test4: test/nishiki-test
	bash test/scripts/run_test4.bash &
	test/nishiki-test --config test/config_broken.toml

test5: test/nishiki-test
	bash test/scripts/run_test5.bash &
	test/nishiki-test --plugin filechooser
	bash test/scripts/run_test5.bash &
	test/nishiki-test --plugin histchooser
	bash test/scripts/run_test5.bash &
	test/nishiki-test --plugin procchooser

test6: test/nishiki-test
	test/nishiki-test --hoge fuga || :
	test/nishiki-test --help
	test/nishiki-test --version

test/nishiki-test: external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 build.py --target nishiki-test --src ./src --out ./test --link ncursesw --cc gcc --gcov

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
