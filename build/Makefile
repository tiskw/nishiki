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

gcc: external/cxxopts.hpp external/glob.hpp external/pocketpy.o
	bash compile.bash --compiler=gcc --src=${SRCDIR} --obj=${OBJDIR} --link=${LIBS}

clang: external/cxxopts.hpp external/glob.hpp external/pocketpy.o
	bash compile.bash --compiler=clang --src=../sources --obj=objects --link=${LIBS}

external/cxxopts.hpp:
	mkdir -p external
	wget -q -O external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp --no-check-certificate

external/glob.hpp:
	mkdir -p external
	wget -q -O external/glob.hpp https://raw.githubusercontent.com/p-ranav/glob/master/single_include/glob/glob.hpp --no-check-certificate

external/pocketpy.c:
	mkdir -p external
	wget -q -O external/pocketpy.c https://github.com/pocketpy/pocketpy/releases/download/v2.0.5/pocketpy.c

external/pocketpy.h:
	mkdir -p external
	wget -q -O external/pocketpy.h https://github.com/pocketpy/pocketpy/releases/download/v2.0.5/pocketpy.h

external/pocketpy.o: external/pocketpy.c external/pocketpy.h
	gcc -std=c11 -c -O3 -DNDEBUG -DPK_ENABLE_OS=1 -I./external -o external/pocketpy.o external/pocketpy.c

################################################################################
# Other utility commands
################################################################################

clean:
	rm -fr nishiki ${OBJDIR}

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
