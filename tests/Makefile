################################################################################
# Makefile
#
# [Available commands]
#   - make clean       Clean up build cache files.
#   - make uninstall   Uninstall NiShiKi (both binary and config files).
################################################################################

.PHONY: test test1 test2 test3 test4 test5 test6 clean

#-------------------------------------------------------------------------------
# Compile settings
#-------------------------------------------------------------------------------

SRC_DIR   := ../source
OBJ_DIR   := objects
SRC_FILES := $(shell find $(SRC_DIR) -type f | grep cxx | grep -v main.cxx)
OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.cxx,%.obj,$(SRC_FILES)))

CC := g++ -g -coverage -std=c++23 -Wall -Wextra -I$(SRC_DIR) -I./external

#-------------------------------------------------------------------------------
# Test commands
#-------------------------------------------------------------------------------

test:
	$(MAKE) test1
	$(MAKE) test2
	lcov -c -d ./objects --include "*/nishiki/source/*" -o lcov.info
	genhtml lcov.info -o ./lcov

test1: test_nishiki
	./test_nishiki

test2: run_nishiki
	sh test_nishiki.sh

#-------------------------------------------------------------------------------
# Build commands
#-------------------------------------------------------------------------------

run_nishiki: external/cxxopts.hpp $(OBJ_DIR) $(OBJ_FILES) $(OBJ_DIR)/main.obj
	$(CC) -o $(@) $(OBJ_FILES) objects/main.obj

test_nishiki: external/cxxopts.hpp $(OBJ_DIR) $(OBJ_FILES) $(OBJ_DIR)/test_nishiki.obj
	$(CC) -o $(@) $(OBJ_FILES) objects/test_nishiki.obj

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.obj: $(SRC_DIR)/%.cxx $(SRC_DIR)/%.hxx
	$(CC) -c -o $(@) $(<)

$(OBJ_DIR)/main.obj: $(SRC_DIR)/main.cxx
	$(CC) -c -o $(@) $(<)

$(OBJ_DIR)/test_nishiki.obj: test_nishiki.cxx
	$(CC) -c -o $(@) $(<)

external/cxxopts.hpp:
	mkdir -p external
	wget -q -O external/cxxopts.hpp https://raw.githubusercontent.com/jarro2783/cxxopts/master/include/cxxopts.hpp --no-check-certificate

#-------------------------------------------------------------------------------
# Other utility commands
#-------------------------------------------------------------------------------

clean:
	rm -f run_nishiki test_nishiki test_performance lcov.info
	rm -fr objects ./lcov

# vim: noexpandtab shiftwidth=4 tabstop=4 fdm=marker
