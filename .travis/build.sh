#!/bin/sh
set -e
tlmgr update --self
tlmgr install cancel enumitem titlesec titling xcolor pgfplots fancyvrb forloop environ xifthen multido listings xkeyval comment  epstopdf
cd /repo
mkdir .original
cp ximera.cls .original/
cp xourse.cls .original/
pdflatex -shell-escape -recorder -halt-on-error ximera.dtx
makeindex -q -s gglo.ist -o ximera.gls ximera.glo
makeindex -q -s gind.ist -o ximera.ind ximera.idx
pdflatex -shell-escape -recorder -halt-on-error ximera.dtx
pdflatex -shell-escape -recorder -halt-on-error ximera.dtx 
diff ximera.cls .original/ximera.cls
diff xourse.cls .original/xourse.cls
