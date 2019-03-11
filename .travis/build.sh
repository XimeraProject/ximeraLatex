#!/bin/sh
tlmgr update --self
tlmgr install cancel enumitem titlesec titling xcolor pgfplots fancyvrb forloop environ xifthen multido listings xkeyval comment  epstopdf
cd /repo
pdflatex -shell-escape -recorder -halt-on-error ximera.dtx
makeindex -q -s gglo.ist -o ximera.gls ximera.glo
makeindex -q -s gind.ist -o ximera.ind ximera.idx
pdflatex -shell-escape -recorder -halt-on-error ximera.dtx
pdflatex -shell-escape -recorder -halt-on-error ximera.dtx

