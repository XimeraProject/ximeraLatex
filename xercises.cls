% A LaTeX2e document class for preparing handouts and online material
% via Ximera.

%% xercises.cls Copyright (c) 2015 Bart Snapp and Jim Fowler

% This work may be distributed and/or modified under the conditions of
% the LaTeX Project Public License, either version 1.3c of this
% license or (at your option) any later version.  The latest version
% of this license is in http://www.latex-project.org/lppl.txt and
% version 1.3c or later is part of all distributions of LaTeX version
% 2013/12/01 or later.  It is based on the subfiles class and package
% LaTeX Copyright 2002, 2012 Federico Garcia (feg8@pitt.edu,
% fedegarcia@hotmail.com) This work has the LPPL maintenance status
% "author-maintained".  

%%% Bart Snapp
%%% Department of Mathematics
%%% The Ohio State University

%%% Oscar Levin
%%% School of Mathematical Sciences
%%% University of Northern Colorado

%%% Hans Parshall
%%% Department of Mathematics
%%% University of Georgia

\def\fileversion{.57}
\def\filedate{2015/07/15}

\NeedsTeXFormat{LaTeX2e}

\ProvidesClass{xercises}[\filedate\space Version \fileversion\space by
  Bart Snapp, Oscar Levin, Hans Parshall]

% Passes xercises class options to the underlying ximera class
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{xercises}}
\ProcessOptions

\LoadClass{xourse}




%</package>



