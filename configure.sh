#!/bin/bash


VERSION="1.3.4"
DEPS=("ctags" "sed" "cat" "make" "git")

log() {
	echo "$@" >&2
}

check_deps() {
	for dep in $DEPS
	do
		which $dep 2>/dev/null 1>/dev/null
		if [ ! "$?" = 0 ]; then
			log "dependency $dep missing"
			exit 1
		fi
	done
}

version() {
	echo "$(sed "s/#define GWEB_VERSION_STR .*$/#define GWEB_VERSION_STR \"$VERSION\"" src/main.c)" > src/main.c
}

set_prefix() {
	log "setting prefixes"
	cat Makefile | sed "s/^PREFIX = .*$/PREFIX = $(echo "$1" | sed 's/\//\\\//g')/" > Makefile
	log "done"
}

make_release() {
	log "configuring makefile (release)..."
	echo "$(sed 's/-ggdb/-O3/' Makefile)" > Makefile
	echo "$(sed 's/-ggdb/-O3/' linked_list/Makefile)" > linked_list/Makefile
	echo "$(sed 's/-ggdb/-O3/' hashmap/Makefile)" > hashmap/Makefile
	echo "$(sed 's/-DGWEB_LOG_DEBUG//' Makefile)" > Makefile
	log "done"
}

make_debug() {
	log "configuring makefile (debug)..."
	echo "$(sed 's/-O3/-ggdb/' Makefile)" > Makefile
	echo "$(sed 's/-O3/-ggdb/' linked_list/Makefile)" > linked_list/Makefile
	echo "$(sed 's/-O3/-ggdb/' hashmap/Makefile)" > hashmap/Makefile
	echo "$(sed 's/CFLAGS =/CFLAGS = -DGWEB_LOG_DEBUG/' Makefile)" > Makefile
	log "done"
}

make_debian() {
    log "configuring for debian (release)..."
    mkdir -p "$PWD/deb/DEBIAN"
    set_prefix "$PWD/deb/usr"
    make_release
    cp control "$PWD/deb/DEBIAN/control"
    log "done"
    
}

tags() {
	log "generating tag file"
	ctags -R src include linked_list/src linked_list/include hashmap/src hashmap/include --exclude="*aur*"
	log "done"
}

print_help() {
	echo "
SYNOPSIS
  ./configure.sh -[hp <prefix>]
OPTIONS
  -h: print this help and exit
  -p <prefix>: set the prefix to <prefix>
  -g: prepare repository to a git state and exit. (format code)
  -r: make makefile act in release mode
  -d: make makefile act in debug mode
  -t: generate tags for development (also done with -d)
  -D: prepare for debian packaging
" >&2
}

main() {
	local opts;
	check_deps
	set -e
	while getopts "htgp:rdD" opts; do
		case $opts in
			h)
				print_help
				exit 0
				;;
			p)
				set_prefix "$OPTARG"
				;;
			g)
				make_release
				make format
				exit 0
				;;
			r)
				make_release
				;;
			d)
				make_debug
				tags
				;;
			t)
				tags
				;;
			D)
			    make_debian
			    ;;
		esac
	done
}

main "$@"
