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

build/nishiki: $(SRC_FILES)
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
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Check C++ code \033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	cppcheck --std=c++23 --enable=all -I./source --library=posix \
		     --suppress=missingIncludeSystem --suppress=useStlAlgorithm \
		     source/*.cxx
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Check C++ plugin \033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	cppcheck --std=c++23 --enable=all -I./source --library=posix \
		     --suppress=missingIncludeSystem --suppress=useStlAlgorithm \
		     plugins/*.cxx
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Check Python plugin - chooser \033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	pyflakes3 plugins/chooser
	pylint plugins/chooser
	python3 -m mypy plugins/chooser
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Check Python plugin - callcmd \033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	pyflakes3 plugins/callcmd
	pylint plugins/callcmd
	python3 -m mypy plugins/callcmd
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Check Python plugin - getpstr \033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	pyflakes3 plugins/getpstr
	pylint plugins/getpstr
	python3 -m mypy plugins/getpstr
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	@echo "\033[38;5;140m// Check Python plugin - welcome \033[m"
	@echo "\033[38;5;140m////////////////////////////////////////////////////////////////////////////////\033[m"
	pyflakes3 plugins/welcome
	pylint plugins/welcome
	python3 -m mypy plugins/welcome

clean:
	cd build; make clean; cd ..
	cd tests; make clean; cd ..

count:
	cloc --by-file $(shell find source -type f | grep -v config)

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
