# gweb
## a simple browser written in C
[![pipeline status](https://gitlab.sokoll.com/moritz/gweb/badges/main/pipeline.svg)](https://gitlab.sokoll.com/moritz/gweb/-/commits/main)
## Installation
### From source
If you want to adjust the prefix use `./configure.sh -p <prefix>`.
For release settings use `./configure.sh -r`. This will remove all debugging flags from the compiler and add optimization
Just run `make install`. If you want to change other build settings edit the `Makefile`
### From the AUR
`gweb` has a version in the aur. You can install that one. It will be configured with `./configure.sh -r` and prefixed to `/usr`.

## Usage
Simply running `gweb` will start the browser. Currently the options are:
- `-h`, `--help`: print help information and exit
- `-V`, `--version`: print the version and exit
- `-v`, `--verbose`: more verbose logging level (verbosity +1 default is 0)
- `-vv`, `--vverbose`: even more verbose logging level (verbosity +2 default is
  0)
- `--nojs`: disable javascript
- `--nodev`: disable developer tools
### Verbosity
The default verbosity level is 0 meaning only error events of `gweb` are logged
(GTK has independent settings). The next level is 1 meaning warnings are also
logged. The highest currently supported logging level is 2 meaning everything
gets logged. This level is used to log important activity conducted by `gweb`
including creation of tabs, allocation and deallocation of memory and more.
