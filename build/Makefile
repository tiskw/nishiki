################################################################################
# Makefile
#
# [Available commands]
#   - make gcc      Build NiShiKi using GNU C++ compiler.
#   - make clang    Build NiShiKi using Clang C++ compiler.
#   - make clean    Clean up build cache files.
################################################################################

.PHONY: gcc clang clean

################################################################################
# Compile settings
################################################################################

SRCDIR := ../sources
OBJDIR := ./objects
LIBS := ncursesw

################################################################################
# Build commands
################################################################################

gcc: external/cxxopts.hpp external/glob.hpp external/toml.hpp
	bash compile.bash --compiler=gcc --src=${SRCDIR} --obj=${OBJDIR} --link=${LIBS}

clang: external/cxxopts.hpp external/glob.hpp external/toml.hpp
	bash compile.bash --compiler=clang --src=../sources --obj=objects --link=${LIBS}

external/cxxopts.hpp:
	mkdir -p external
	wget -q -O external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp --no-check-certificate

external/glob.hpp:
	mkdir -p external
	wget -q -O external/glob.hpp https://raw.githubusercontent.com/p-ranav/glob/master/single_include/glob/glob.hpp --no-check-certificate

external/toml.hpp:
	mkdir -p external
	wget -q -O external/toml.hpp https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp --no-check-certificate

################################################################################
# Other utility commands
################################################################################

clean:
	rm -fr nishiki ${OBJDIR}

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
