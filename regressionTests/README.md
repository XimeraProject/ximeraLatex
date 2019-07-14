# Regression tests

To make sure that nothing is broken when editing ximeraLatex, we
include these regression tests.

This depends on [pdiff](https://github.com/kisonecat/pdiff), a
command-line tool for visually comparing two PDFs.

## Getting started

Go to the directory `$TEXMF/tex/latex/ximeraLatex/regressionTests`.

Make a directory `mkdir pdfFiles` to hold the generated PDFs from the
known good version.

Run `npm install` to install the various dependencies.

Run `node generate.js` to produce known good PDFs in `pdfFiles` from the files in `texFiles`

When you want to test changes, run `npm test`.
