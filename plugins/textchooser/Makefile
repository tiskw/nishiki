# Makefile

PROGRAM_NAME = textchooser
SOURCE_FILES = $(shell ls *.py)

release:
	zip -9 -r $(PROGRAM_NAME).zip $(SOURCE_FILES)
	echo '#!/usr/bin/env python3' | cat - $(PROGRAM_NAME).zip > $(PROGRAM_NAME)
	chmod +x $(PROGRAM_NAME)
	rm -f $(PROGRAM_NAME).zip

install:
	mkdir -p ~/.config/nishiki/plugins
	mv $(PROGRAM_NAME) ~/.config/nishiki/plugins/

check:
	pyflakes3 $(SOURCE_FILES)

clean:
	rm -fr $(PROGRAM_NAME) $(PROGRAM_NAME).zip version.py __pycache__ /tmp/${PROGRAM_NAME}

count:
	cat $(SOURCE_FILES) | grep -v '^\s*#' | grep -v '^\s*$$' | wc -l

# Author: Tetsuya Ishikawa <tiskw111@gmail.com>
# vim: noexpandtab tabstop=4 shiftwidth=4
