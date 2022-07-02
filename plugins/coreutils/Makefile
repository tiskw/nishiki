# Makefile

PROGRAM_NAME = coreutils
SOURCE_FILES = $(shell ls *.py)
CACHE_DIRS   = $(shell find . -type d | grep __pycache__)

release:
	rm -rf $(PROGRAM_NAME) $(PROGRAM_NAME).zip $(CACHE_DIRS)
	zip -9 -r $(PROGRAM_NAME).zip .
	echo '#!/usr/bin/env python3' | cat - $(PROGRAM_NAME).zip > $(PROGRAM_NAME)
	chmod +x $(PROGRAM_NAME)
	rm -f $(PROGRAM_NAME).zip

install:
	mkdir -p ~/.config/nishiki/plugins
	mv $(PROGRAM_NAME) ~/.config/nishiki/plugins/

check:
	pyflakes3 $(SOURCE_FILES)

clean:
	rm -rf $(PROGRAM_NAME) $(PROGRAM_NAME).zip $(CACHE_DIRS)

count:
	cloc --by-file $(SOURCE_FILES)

# vim: noexpandtab tabstop=4 shiftwidth=4
