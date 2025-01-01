################################################################################
# Makefile
#
# [Available commands]
#   - make clean       Clean up build cache files.
#   - make uninstall   Uninstall NiShiKi (both binary and config files).
################################################################################

.PHONY: build-nishiki-test test test1 test2 test3 test4 test5 test6 clean

#-------------------------------------------------------------------------------
# Compile settings
#-------------------------------------------------------------------------------

SRCDIR = sources
OBJDIR = objects
LIBS = ncursesw

#-------------------------------------------------------------------------------
# Entrypoint
#-------------------------------------------------------------------------------

all:
	make copy
	make build
	make test --ignore-errors

#-------------------------------------------------------------------------------
# Build commands
#-------------------------------------------------------------------------------

copy:
	rm -rf sources
	cp -ar ../sources .
	cp unittest/unittest.* sources/
	cp -af unittest/main_with_unittest.cxx sources/main.cxx
	cp -af ../build/compile.bash .

build: external external/cxxopts.hpp external/glob.hpp external/toml.hpp
	bash compile.bash --compiler=gcc --src=${SRCDIR} --obj=${OBJDIR} --gcov=true --link=${LIBS}
	mv nishiki nishiki-test

external:
	ln -s ../build/external

external/cxxopts.hpp:
	wget -q -O ./external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp

external/toml.hpp:
	wget -q -O ./external/toml.hpp https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp

external/glob.hpp:
	wget -q -O ./external/glob.hpp https://raw.githubusercontent.com/p-ranav/glob/master/single_include/glob/glob.hpp

#-------------------------------------------------------------------------------
# Test commands
#-------------------------------------------------------------------------------

test:
	make test1
	make test2
	make test3
	make test4
	make test5
	lcov -c -d ./objects -o lcov.info
	lcov --remove lcov.info unittest* -o lcov.info
	genhtml lcov.info -o ./lcov

test1:
	bash ./scripts/run_test1.bash &
	./nishiki-test --config ./configs/config.toml

test2:
	bash ./scripts/run_test2.bash &
	./nishiki-test --config ./config/config_broken.toml

test3:
	bash ./scripts/run_test3.bash &
	./nishiki-test

test4:
	bash ./scripts/run_test4.bash &
	./nishiki-test --plugin filechooser
	bash ./scripts/run_test4.bash &
	./nishiki-test --plugin histchooser
	bash ./scripts/run_test4.bash &
	./nishiki-test --plugin procchooser
	./nishiki-test --plugin hoge

test5:
	./nishiki-test --hoge fuga
	./nishiki-test --help
	./nishiki-test --version

#-------------------------------------------------------------------------------
# Other utility commands
#-------------------------------------------------------------------------------

clean:
	rm -fr nishiki-test lcov.info ./lcov ${SRCDIR} ${OBJDIR} ./external compile.bash

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
