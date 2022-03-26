#!/bin/bash
set -e

main() {
    dpkg-deb --root-owner-group --build deb
    mv deb.deb gweb.deb
}

main "$@"
