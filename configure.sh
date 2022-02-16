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

print_help() {
	echo "
SYNOPSIS
  ./configure.sh -[hp <prefix>]
OPTIONS
  -h: print this help and exit
  -p <prefix>: set the prefix to <prefix>
" >&2
}

main() {
	local opts;
	while getopts "hp:" opts; do
		case $opts in
			h)
				print_help
				exit 0
				;;
			p)
				set_prefix "$OPTARG"
				;;
		esac
	done
}

main "$@"
