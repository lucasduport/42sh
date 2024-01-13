all:
	@cd src && $(MAKE) -s all
	if [ $enable_debug = "yes" ]; then
		mv src/42sh src/42sh_debug
	fi

clean:
	@cd src && $(MAKE) -s clean
	@cd tests && $(MAKE) -s clean

check: all
	@cd tests && $(MAKE) -s check-local
