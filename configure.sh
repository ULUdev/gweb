#!/bin/bash

set -e

log() {
	echo "$@" >&2
}

set_prefix() {
	log "setting prefixes"
	cat Makefile | sed "s/^PREFIX = .*$/PREFIX = $(echo "$1" | sed 's/\//\\\//g')/" > Makefile
	cat gweb.desktop | sed "s/^Exec=.*$/Exec=$(echo "$1" | sed 's/\//\\\//g')\/bin\/gweb %u/" > gweb.desktop
	log "done"
}

make_release() {
	log "configuring makefile (release)..."
	echo "$(sed 's/-ggdb/-O3/' Makefile)" > Makefile
	echo "$(sed 's/-ggdb/-O3/' linked_list/Makefile)" > linked_list/Makefile
	log "done"
}

make_debug() {
	log "configuring makefile (debug)..."
	echo "$(sed 's/-O3/-ggdb/' Makefile)" > Makefile
	echo "$(sed 's/-O3/-ggdb/' linked_list/Makefile)" > linked_list/Makefile
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
" >&2
}

main() {
	local opts;
	while getopts "hgp:rd" opts; do
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
		esac
	done
}

main "$@"
