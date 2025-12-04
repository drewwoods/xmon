xmon - graphical system monitor
===============================

![xmon](http://nuclear.mutantstargoat.com/sw/xmon/img/xmon-shot002.png)

Project status: prototype/experiment.

Xmon started as an experiment to find a good way to present CPU usage of more
than a handful of cores. I was disappointed by existing system monitors which
either show the average usage (which for a 32-thread processor will be mostly
blank if you're not maxing most cores), or attempt to fit N different CPU
graphs onto my screen which are just meaningless noise and clutter.

The main idea I wanted to try, was to use a waterfall plot display, similar to
plots often use in frequency analysis, where the horizontal axis is split among
the CPUs/cores, the vertical axis is time, and the plot intensity/color is the
usage percentage of each core.

Other than the main experiment, my goals with this project are:

  - extend it to show other useful statistics (memory, network traffic, disk
    I/O, etc).
  - maximum configurability.
  - wide compatibility: cross-platform, use few colors, test on PseudoColor X
    servers.
  - be lightweight, so that it will be useful on retro UNIX workstations too.
  - make it look nice, for multiple definitions of nice looks (see
    configurability above).

Currently xmon supports Linux, IRIX, and MacOS X.

License
-------
Copyright (C) 2025 John Tsiombikas <nuclear@mutantstargoat.com>

Xmon is Free Software. Feel free to use, modify, and/or redistribute under the
terms of the GNU General Public License v3, or at your option any later version
published by the Free Software Foundation. See COPYING for details.

Build instructions
------------------

### GNU/Linux
Just run `make`, and optionally `make install` to install xmon system-wide. The
default installation prefix is `/usr/local`, which can be changed by modifying
the first line of `GNUmakefile`.

### IRIX
To build on IRIX, run `make -f Makefile.sgi`, and then `make -f Makefile.sgi
install` if you wish to install xmon. Installation prefix can be changed by
modifying the first line of `Makefile.sgi`.
