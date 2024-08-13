Greetings! [![Build Status](https://travis-ci.org/XimeraProject/ximeraLatex.svg?branch=master)](https://travis-ci.org/XimeraProject/ximeraLatex)
==========

In this repository, we develop the Ximera LaTeX document class. As of summer of 2024, the Ximera document class is available in [CTAN](https://ctan.org/pkg/ximera?lang=en).

However, if a user should wish to install locally at this point (not recommended) see: 

[Installing Locally](./installingLocally.md)

For a list of different Ximera commands, see: 

[Examples of Ximera Envrionments](https://go.osu.edu/ximera-examples)


Since Ximera is built on LaTeX source, we want to use LaTeX as a
method of validating the code authors write. Hence, if you want to
write a Ximera online activity, the first step is constructing LaTeX
documents.

Once you have the LaTeX documents, and you have checked them for
typos, accuracy, etc, the fact that they compile should be reasonable
evidence that they will display correctly in Ximera.



Contents of the repository
---------------------------

* This README.md file. 

* The LPPL 1.3c license.

* The Ximera documented LaTeX file type, ximera.dtx. This file
  generates ximera.cls, xourse.cls, and ximeraLaTeX.pdf, as well as a
  few other files.


Compiling the files
-------------------

Running "make" generates the derived files README, ximera.pdf, ximera.cls, xourse.cls.

Running "make ctan" generates a submission suitable for CTAN

Running "make inst" installs the files in the user's TeX tree.

Running "make install" installs the files in the local TeX tree.

Staying up-to-date
------------------

While we hope to solidify the ximera.cls file, at this point we are
still in development stages.

To keep your file up-to-date, you may need to periodically sync or
pull the the ximera.cls within the ximeraLatex directory.

`ximeraLatex$ git fetch --all`

`ximeraLatex$ git reset --hard origin/master`

will reset your ximeraLatex directory. Note it will also overwrite
*any* modifications you have made in this directory. You should not be
building your activities in this directory.



Future Features
---------------

- Ability to include \activities and \practice within a ximera document
  - when adding Xourses, the path to the xourse/file needs to be given
in some way. We have an example of this in the preamble of
examples/exerciseCollection/exerciseCollection.tex This enables an
author to print the file and know where to find the parts.
  - Perhaps by default, all Xourse files appear on the top page, but if modified with `\documentclass[hidden]{xourse}, they would no longer appear
  - We'd like a separate, perhaps password protected page with ALL content on it.
- Perhaps xourse as an option for ximera documents
- \xsection \xsubsection (for formatting mixed xourses)
