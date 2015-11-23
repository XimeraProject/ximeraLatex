% A LaTeX2e document class for preparing handouts and online material
% via Ximera.

%% xourse.cls Copyright (c) 2015 Bart Snapp and Jim Fowler

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

\ProvidesClass{xourse}[\filedate\space Version \fileversion\space by
  Bart Snapp, Oscar Levin, Hans Parshall]

\newif\ifnotoc
\notocfalse
\DeclareOption{notoc}{\notoctrue}

\newif\ifnonewpage
\nonewpagefalse
\DeclareOption{nonewpage}{\nonewpagetrue}

% Passes xourse class options to the underlying ximera class
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{ximera}}
\ProcessOptions

\LoadClass{ximera} 

\RequirePackage{verbatim} 


%%%%% ximera documents included with \activity{} will be included in the body of the xourse document
%%%%% \input commands within included ximera documents will be ignored
%%%%% \usepackage commands within included ximera documents will cause an error
%%%%% overlapping \newcommand definitions within multiple ximera documents included simultaneously will cause an error

% The core of the package. It works by redefining the |document| `environment,' thus making the |\begin| and |\end{document}| of the subfile `transparent' to the inclusion. The redefinition of |\documentclass| is analogous, just having a required and an optional arguments which mean nothing to |\subfile|.
\newcommand{\skip@preamble}{%
    \let\document\relax\let\enddocument\relax%
    \newenvironment{document}{\let\input\otherinput}{}%
    \renewcommand{\documentclass}[2][subfiles]{}}

% Note that the new command |\subfile| calls for |\skip@preamble| \emph{within a group}. The changes to |document| and |\documentclass| are undone after the inclusion of the subfile. 

\let\otherinput\input %% Numbering starts a page too soon without this


% Store usual maketitle as othermaketitle
\let\othermaketitle\maketitle

% % Redefine maketitle to give course packet title page and toc.
\renewcommand{\maketitle}{ %
\pagestyle{empty}
\begin{center}
~\\ %puts space at top of page to move title down.  Is there a better way to do this?
\vskip .25\textheight
\hrulefill\\
\vskip 1em
\bfseries{\Huge \@title} \\
\hrulefill\\
\vskip 3em
{\Large \@author}
\vskip 2em
{\large \@date}
\end{center}
\clearpage
\ifnotoc\else % When notoc option is used, we do not include a table of contents.  Otherwise:
	\tableofcontents\clearpage % we include a table of contents in every course packet.  
	\clearpage
\fi
\pagestyle{main} %switch to main pagestyle, just like ximera documents.
\let\maketitle\othermaketitle % renew maketitle to usual definition.
}
\relax




% \activity command inputs the file name provided without \documentclass, \being{document}/\end{document} and any inputs in the preamble of the included file.
\ifnonewpage
\newcommand\activity[1]{%
  \renewcommand{\input}[1]{}
  \begingroup\skip@preamble\otherinput{#1}\endgroup
  \let\input\otherinput}
\else
\newcommand\activity[1]{%
  \renewcommand{\input}[1]{}
  \begingroup\skip@preamble\otherinput{#1}\endgroup\clearpage
  \let\input\otherinput}
\fi
\relax


%% \newcommand{\chapter}{} %% makes chapter "work"
%% \relax

% makes the toc look a bit better... can be redefined in the preamble.
\makeatletter
\renewcommand*\l@section{\@dottedtocline{1}{1.5em}{4.2em}}
\renewcommand*\l@subsection{\@dottedtocline{2}{3.8em}{4.2em}}
\makeatother
\relax


%</package>



