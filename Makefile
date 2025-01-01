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

.PHONY: gcc clang test check count clean

################################################################################
# Install path definitions and build options
################################################################################

# Install directory of a binary file.
BINDIR := $(HOME)/bin

# Install directory of config files and plugins.
CFGDIR := $(HOME)/.config/nishiki

################################################################################
# Build commands
################################################################################

gcc:
	cd build; make gcc; cd ..

clang:
	cd build; make clang; cd ..

################################################################################
# Test commands
################################################################################

test:
	cd tests; make; cd ..

################################################################################
# Install/uninstall commands
################################################################################

install:
	mkdir -p $(BINDIR) $(CFGDIR)/plugins
	cp -f build/nishiki $(BINDIR)
	cp -n config.toml $(CFGDIR)/

uninstall:
	rm -f $(BINDIR)/nishiki

purge:
	rm -f $(BINDIR)/nishiki
	rm -fr $(CFGDIR)

################################################################################
# Other utility commands
################################################################################

check:
	cppcheck --std=c++23 --enable=all -I./sources --library=posix \
		--suppress=missingIncludeSystem --suppress=useStlAlgorithm \
		sources/*.cxx

clean:
	cd build; make clean; cd ..
	cd tests; make clean; cd ..

count:
	cloc --by-file sources/*.cxx sources/*.hxx

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
