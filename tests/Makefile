# Makefile

.PHONY: build-nishiki-test test test1 test2 test3 test4 test5 test6 clean

ROOT = ..
CWD  = $(shell pwd)

test:
	make build-nishiki-test
	make test1
	make test2
	make test3
	make test4
	make test5
	make test6
	lcov -c -d ./build -o lcov.info
	genhtml lcov.info -o ./lcov

test1:
	bash ./scripts/run_test1.bash &
	./build/nishiki-test --config ./configs/config.toml

test2:
	bash ./scripts/run_test2.bash &
	./build/nishiki-test

test3:
	bash ./scripts/run_test3.bash &
	cd ~ && $(CWD)/build/nishiki-test

test4:
	bash ./scripts/run_test4.bash &
	./build/nishiki-test --config configs/config_broken.toml

test5:
	bash ./scripts/run_test5.bash &
	./build/nishiki-test --plugin filechooser
	bash ./scripts/run_test5.bash &
	./build/nishiki-test --plugin histchooser
	bash ./scripts/run_test5.bash &
	./build/nishiki-test --plugin procchooser

test6:
	./build/nishiki-test --hoge fuga || :
	./build/nishiki-test --help
	./build/nishiki-test --version

build-nishiki-test: external external/cxxopts.hpp external/glob.hpp external/toml.hpp version.hxx
	python3 ./update_sources.py --src ../src --out ./src
	python3 $(ROOT)/build.py --target nishiki-test --src ./src --out ./build --link ncursesw --cc gcc --gcov

external:
	ln -s $(ROOT)/external

external/cxxopts.hpp:
	wget -q -O ./external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp

external/toml.hpp:
	wget -q -O ./external/toml.hpp https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp

external/glob.hpp:
	wget -q -O ./external/glob.hpp https://raw.githubusercontent.com/p-ranav/glob/master/single_include/glob/glob.hpp

version.hxx:
	echo '#define VERSION "nishiki-test"' > version.hxx

clean:
	rm -fr version.hxx ./build lcov.info ./lcov ./src ./external

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
