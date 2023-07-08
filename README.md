# gweb
## a simple browser written in C
[![pipeline status](https://gitlab.sokoll.com/moritz/gweb/badges/main/pipeline.svg)](https://gitlab.sokoll.com/moritz/gweb/-/commits/main)
## Installation
### From the AUR
`gweb` has a version in the aur. You can install that one. It will be configured
with `./configure.sh -r` and prefixed to `/usr`.
### For Debian based distributions
If you are on a Debian based distribution of Linux your steps will look like this:
1. `./configure.sh -D`: prepare the repository for Debian packaging
2. `make install`: move the necessary files into the package directory (no root access required)
3. `./debian.sh`: build the actual package (make sure that `libgtk-3-0` and `libwebkit2gtk-4.0-37` are installed)
4. `dpkg -i gweb.deb`: install the Debian package (requires root access)
### From source
If you want to adjust the prefix use `./configure.sh -p <prefix>`.  For release
settings use `./configure.sh -r`. This will remove all debugging flags from the
compiler and add optimization Just run `make install`. If you want to change
other build settings edit the `Makefile`
### More compact build messages
If you wan't to have a more compact output from `make` run every `make` command
as `COMPACT_OUTPUT=1 make <options>`.

## Usage
Simply running `gweb` will start the browser. Currently the options are:
- `-h`, `--help`: print help information and exit
- `-V`, `--version`: print the version and exit
- `-v`, `--verbose`: more verbose logging level (verbosity +1 default is 0)
- `-vv`, `--vverbose`: even more verbose logging level (verbosity +2 default is
  0)
- `--nojs`: disable javascript
- `--nodev`: disable developer tools
- `--private`: enable private browsing mode
- `--noconf`: do not read the configuration file
- `--headless`: launch a minimal gui version that uses the last url supplied
### Verbosity
The default verbosity level is 0 meaning only error events of `gweb` are logged
(GTK has independent settings). The next level is 1 meaning warnings are also
logged. The highest currently supported logging level is 2 meaning everything
gets logged. This level is used to log important activity conducted by `gweb`
including creation of tabs, allocation and deallocation of memory and more.

## Contributing
If you wish to contribute to this project feel free to do so. The open issues
are found mostly on a private GitLab instance
[here](https://gitlab.sokoll.com/moritz/gweb/issues). Contribute by making a
pull request at either [GitHub](https://github.com/ULUdev/gweb) or
[codeberg](https://codeberg.org/UwUdev/gweb).
### Code formatting and such
The Makefile has a target for formatting. Just run `make format` which will
format most of the components. If you add some that aren't recognized modify
the makefile accordingly. In general, the Makefile should work for most things
already. Simply adding a file in the `include` or `src` directories will
automatically be picked up by it.
### Configuring your environment
The `configure.sh` script has a flag `-d`. This will automatically update the
makefile to compile everything with debug flags. I personally debug everything
with `gdb` so it will be optimized for use with that. If you want the detailed
compiler output run `./configure.sh -c 0` or run every make command as
`COMPACT_OUTPUT=0 make <options>` to disable compact output for everything.
