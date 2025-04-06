################################################################################
# Makefile
#
# [Available commands]
#   - make gcc         Build NiShiKi using GNU C++ compiler.
#   - make clang       Build NiShiKi using Clang C++ compiler.
#   - make test        Run tests and compute code coverage.
#   - make check       Static code analysis.
#   - make count       Count physical lines of source code.
#   - make clean       Clean up build cache files.
#   - make uninstall   Uninstall NiShiKi (both binary and config files).
################################################################################

.PHONY: test check count clean

################################################################################
# Install path definitions and build options
################################################################################

# Get all source files.
SRC_FILES := $(shell find source -type f)

# Get the number of CPUs.
NUM_CPUS := $(shell cat /proc/cpuinfo | grep processor | wc -l)

################################################################################
# Build commands
################################################################################

build/nishiki: $(SRC_FILES) build/external/cxxopts.hpp
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Start building NiShiKi\033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	cd build; $(MAKE) nishiki -j $(NUM_CPUS); cd ..

################################################################################
# Test commands
################################################################################

test:
	cd tests; $(MAKE) test -j $(NUM_CPUS); cd ..

################################################################################
# Other utility commands
################################################################################

check:
	cppcheck --std=c++23 --enable=all -I./source --library=posix \
		     --suppress=missingIncludeSystem --suppress=useStlAlgorithm \
		     source/*.cxx

clean:
	cd build; make clean; cd ..
	cd tests; make clean; cd ..

count:
	cloc --by-file $(shell find source -type f | grep -v config)

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
