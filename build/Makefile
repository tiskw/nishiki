################################################################################
# Makefile
#
# [Available commands]
#   - make gcc      Build NiShiKi using GNU C++ compiler.
#   - make clean    Clean up build cache files.
################################################################################

.PHONY: gcc clean

#-------------------------------------------------------------------------------
# Compile settings
#-------------------------------------------------------------------------------

SOFTWARE := nishiki

# CC := g++ -std=c++23 -Ofast -Wall -Wextra -I../sources
CC := clang++ -std=c++23 -Ofast -mllvm -polly -march=native -Wall -Wextra -I../sources

SRC_DIR   := ../source
OBJ_DIR   := objects
SRC_FILES := $(wildcard $(SRC_DIR)/*.cxx)
OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.cxx,%.obj,$(SRC_FILES)))

#-------------------------------------------------------------------------------
# Build commands
#-------------------------------------------------------------------------------

$(SOFTWARE): $(OBJ_DIR) $(OBJ_FILES)
	$(CC) -o $(SOFTWARE) $(OBJ_FILES)
	strip $(SOFTWARE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.obj: $(SRC_DIR)/%.cxx $(SRC_DIR)/%.hxx
	$(CC) -c -o $(@) $(<)

external/cxxopts.hpp:
	wget -O external/cxxopts.hpp https://github.com/p-ranav/argparse/blob/master/include/argparse/argparse.hpp

external/json.hpp:
	wget -O - https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz | unxz -k - > external/json.hpp

#-------------------------------------------------------------------------------
# Other utility commands
#-------------------------------------------------------------------------------

clean:
	rm -f nishiki
	rm -fr ${OBJ_DIR}

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
