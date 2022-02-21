#!/bin/bash
set -e

compile() {
	echo "[compiling] gweb" >&2
	make -B 2> compilelog.txt 1> compilelog.txt
}

test_cmd() {
	echo "[test] $@" >&2
	$@ 2> /dev/null 1> /dev/null
}

main() {
	if [ ! -f "gweb" ]; then
		compile
	fi
	test_cmd "./gweb -V"
	test_cmd "./gweb -h"
}

main "$@"
