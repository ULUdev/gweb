# gweb
## a simple browser written in C
[![pipeline status](https://gitlab.sokoll.com/moritz/gweb/badges/main/pipeline.svg)](https://gitlab.sokoll.com/moritz/gweb/-/commits/main)
## Installation
### From the AUR
`gweb` has a version in the aur. You can install that one. It will be configured
with `./configure.sh -r` and prefixed to `/usr`.
### From source
To change the install prefix use `meson setup build . --buildtype=release --prefix=<prefix>`. If
you don't care about installing you can skip the `--prefix` flag. It is
recommended to also add the flag `--default-library=static` to ensure that the
dependencies included in this repository are built as static libraries. There
is currently no known application to also use these dependencies.

When you have done that you can just run `meson compile -C build`. This will
place the resulting binary in `build`.

If you wan't to install the program use `meson install -C build`.
### For Debian based distributions
**WARNING: With the new meson build configuration we have no currently maintained way of packaging for debian**
If you want to package it anyway use the steps from the old commit (latest one
being 4cf5477f6ab474be6aa2d7fdb005224e67f9947a)

If you are on a Debian based distribution of Linux your steps will look like this:
1. `./configure.sh -D`: prepare the repository for Debian packaging
2. `make install`: move the necessary files into the package directory (no root access required)
3. `./debian.sh`: build the actual package (make sure that `libgtk-3-0` and `libwebkit2gtk-4.0-37` are installed)
4. `dpkg -i gweb.deb`: install the Debian package (requires root access)

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
This project uses meson and meson allows you to use the following command to
format your source code: `ninja -C build clang-format`.

If you add or remove a file you should modify the `meson.build` in the
directory accordingly.

If you wish to add tests do so in a directory called `test`. There you can
place your unit tests and write a simple `meson.build` file to generate these
unit tests. Then use `meson test -C build`.
