This repository contains 'ximera' and 'xourse' LaTeX document classes, that provide support 
for authoring and publishing educational content such as textbooks, assessments, and courses.

Every LaTeX source file generates both a PDF and an HTML page, 
where the HTML page can have several types of interactivity.

As of summer of 2024, Ximera is available in [CTAN](https://ctan.org/pkg/ximera?lang=en).

For a list of different Ximera commands, see these
[examples](https://go.osu.edu/ximera-examples)


Ximera courses are built from LaTeX source, and compile to PDF just as any other LaTeX source file.
By using a Ximera build script 'xake' and a Ximera-specific webserver, this same LaTeX source
genenerates an interactive online course.


Contents of the repository
---------------------------

* This README.md file. 

* The LPPL 1.3c license.

* A collection of .dtx files in the src folder
  
* The main ximera.dtx file that generates ximera.cls, xourse.cls, and .4ht files for the generation of HTML.

* A PDF ximeraLaTeX.pdf with technical documentation, also generated from the files in the src folder.

  
Manual installation (not recommended)
-------------------

[Installing Locally](./installingLocally.md)


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
