Greetings! [![Build Status](https://travis-ci.org/XimeraProject/ximeraLatex.svg?branch=master)](https://travis-ci.org/XimeraProject/ximeraLatex)
==========

In this repository, we hope to supply potential authors of Ximera
activities with the LaTeX document classes they need to write Ximera
documents. 

To install the XimeraLaTeX package, see: 

[Installing Locally](./installingLocally.md)

For a list of different Ximera commands, see: 

[Examples of Ximera Envrionments](https://ximera.osu.edu/tester/exampleActivityCollection)


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

