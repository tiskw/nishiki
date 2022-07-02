# Makefile

debug: nerdls.nim
	nim compile --run nerdls.nim -l '.'

release: nerdls.nim
	nim compile -d:release nerdls.nim

install:
	mkdir -p ~/.config/nishiki/plugins
	mv nerdls ~/.config/nishiki/plugins/

clean:
	rm -f nerdls
	cd test/ && sh remove.sh

# vim: noexpandtab tabstop=4 shiftwidth=4 fdm=marker
