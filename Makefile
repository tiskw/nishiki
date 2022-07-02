# Makefile

PROGRAM_NAME = nishiki

# Build a zipapp.
zipapp:
	zip -9 -r $(PROGRAM_NAME).zip *.py
	echo '#!/usr/bin/env python3' | cat - $(PROGRAM_NAME).zip > $(PROGRAM_NAME)
	chmod +x $(PROGRAM_NAME)
	rm -f $(PROGRAM_NAME).zip

# Python syntax check.
check:
	pyflakes3 *.py

# Clear unnecessary files.
clean:
	rm -rf $(PROGRAM_NAME) __pycache__

# Count number of effective source code lines.
count:
	cloc --by-file `ls | grep \.py | grep -v config.py`

# vim: noexpandtab
