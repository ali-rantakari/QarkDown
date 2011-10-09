QarkDown
========

This is a GUI editor for [Markdown] files with syntax highlighting and the ability to execute Markdown compilers.

The following compilers are bundled with QarkDown:

- On OS X:
    - [Multimarkdown] by Fletcher T. Penney
    - [Discount] by David Parsons
- On Windows:
    - [Multimarkdown] by Fletcher T. Penney
    - [peg-markdown] by John MacFarlane
- On Linux:
    - [Multimarkdown] by Fletcher T. Penney

Copyright (c) 2011 Ali Rantakari
<http://hasseg.org/qarkdown>

[Markdown]: http://daringfireball.net/projects/markdown
[Multimarkdown]: http://fletcherpenney.net/multimarkdown/
[Discount]: http://www.pell.portland.or.us/~orc/Code/discount/
[peg-markdown]: https://github.com/jgm/peg-markdown



Installation on Windows
=======================

Copy all of the files that are in the distribution `.zip` package into the same folder (suggestion: `C:\Program Files\QarkDown\`). You can then create shortcuts to `qarkdown.exe` wherever you want.

## Uninstallation on Windows

- Delete QarkDown's "application resources" folder (the location depends on the version of Windows you're running; you can find it by clicking on the _"Show HTML Template File"_ link in the _"Compiling"_ section of QarkDown's preferences)
- Delete the folder where you moved QarkDown's files when you installed it.


Installation on OS X
====================

QarkDown on OS X is a self-contained `.app` bundle, so you can just drop it into `/Applications` (or wherever).

## Uninstallation on OS X

- Delete QarkDown's "application resources" folder in `~/Library/Application Support/QarkDown`
- Delete `qarkdown.app`.


Installation on Linux
=====================

As of this writing no ready-made packages exist for Linux package managers, so you'll have to build and install QarkDown manually:

- Install the Qt SDK (from Nokia's site or via your package manager if it's available there)
- Open the `src/qarkdown.pro` file in Qt Creator
- Select the "release" configuration and press the build button (both of these are in the toolbar on the left side of the screen, at the bottom)
- Copy the executable (`qarkdown-build-desktop/qarkdown`) wherever you'd like.

## Uninstallation on Linux

- Delete QarkDown's "application resources" folder (the location might depend on the distro you're running; you can find it by clicking on the _"Show HTML Template File"_ link in the _"Compiling"_ section of QarkDown's preferences)
- Delete QarkDown's executable from where you moved it at installation time.



