https://dev-to-uploads.s3.amazonaws.com/uploads/articles/ehcz12x7ujdn14w7vek4.png

GNU PLEXYSHELL CORE SOURCE DISTRIBUTION
=======================================

This directory contains a minimal source distribution of the PlexyShell
core client libraries.

The purpose of this package is to provide the core reusable source code
for study, inspection, and rebuilding of the client-side Plexy libraries,
while excluding the full desktop environment, compositor, shell, bundled
applications, assets, and surrounding system integration code.

CONTENTS
--------

This package includes:

  * libplexy client protocol source
  * libplexycanvas rendering source
  * libplexyui client widget source
  * public header files needed to build the libraries
  * bundled stb headers required by the core rendering code
  * a minimal Makefile for rebuilding the shared libraries

This package does not include:

  * the PlexyShell desktop compositor
  * window-management and shell behavior
  * server-side backend and input stack
  * bundled applications
  * runtime assets, backgrounds, icons, or desktop data

BUILDING
--------

To build the libraries, run:

  make

The build expects a normal Unix-like toolchain with gcc/g++, make,
pkg-config, OpenGL, GLEW, EGL, GBM, libdrm, and FreeType development
packages available on the host system.

INSTALLATION
------------

To install the built libraries and headers under the selected prefix, run:

  make install PREFIX=/usr/local

LICENSE
-------

This source distribution is provided under the GNU Affero General Public
License, version 3 only.

See the file COPYING for the full license text.

NO WARRANTY
-----------

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

See COPYING for details.
