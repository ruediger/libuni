.PHONY: all clean distclean install uninstall test

all: build
	$(MAKE) -C build

clean: build
	$(MAKE) -C build clean

distclean:
	rm -rf build/

install: build
	$(MAKE) -C build install

uninstall: build
	@if [ -f build/install_manifest.txt ]; then \
	echo 'Uninstalling' ;                       \
	xargs rm < build/install_manifest.txt ;     \
	else 					    \
	echo 'Librestd does not seem to be installed.'; \
	fi

test: all
	cd ./build/test/ && ctest --output-on-failure

build:
	@echo "Please run ./configure (with appropriate parameters)!"
	@exit 1
