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

\ProvidesClass{xourse}[\filedate\space Version \fileversion\space by
  Bart Snapp, Oscar Levin, Hans Parshall]

\RequirePackage{ximera} 

%\subsection{The class}
%    \begin{macrocode}
%<*class>
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{subfiles}[2012/05/23 Federico Garcia]
\RequirePackage{verbatim}
\DeclareOption*{\typeout{Preamble taken from file `\CurrentOption'}%
    \let\preamble@file\CurrentOption}
\ProcessOptions
%    \end{macrocode}
%
% The first thing to do is to save the regular \LaTeX\ definitions of |\document|, |\enddocument|, and  |\documentclass|:
%    \begin{macrocode}
\let\old@document@subfiles\document
\let\old@enddocument@subfiles\enddocument
\let\old@documentclass@subfiles\documentclass
%    \end{macrocode}
%
% Now the |document| `environment' is redefined and equaled to |comment|. As a consequence, the body of the main file is ignored by \LaTeX, and only the preamble is read (and anything that comes after |\end{document}|!). For |\documentclass|, having been already loaded one (|subfiles|), it is redefined and equaled to |\LoadClass|. The class and options of the main file are loaded identically.
%    \begin{macrocode}
\let\document\comment
\let\enddocument\endcomment
\let\documentclass\LoadClass\relax
%    \end{macrocode}
%
% Now it is possible to |\input| the main file, and then restore the original values of |\document|, |\enddocument| and |\documentclass|. The backup commands are |\undefined| to save memory. That's it.
%    \begin{macrocode}
\input{\preamble@file}
%    \end{macrocode}
%
% Here it comes something not so obvious. In the usual situations, the |\preamble@file| contains some |\usepackage| commands, which, at the end, make |@| no longer a letter. That is why the next part needs a |\catcode| command, grouping, and |\global|'s.
%    \begin{macrocode}
{\catcode`\@=11
\global\let\document\old@document@subfiles
\global\let\enddocument\old@enddocument@subfiles
\global\let\documentclass\old@documentclass@subfiles
\global\let\old@document@subfiles\undefined
\global\let\old@enddocument@subfiles\undefined
\global\let\old@documentclass@subfiles\undefined}
%</class>
%    \end{macrocode}
%
% \subsection{The package}
% Any option will be ignored. 
%    \begin{macrocode}
%<*package>
\NeedsTeXFormat{LaTeX2e}
\ProvidesPackage{subfiles}[2012/05/23 Federico Garcia]
\DeclareOption*{\PackageWarning{\CurrentOption ignored}}
\ProcessOptions
\RequirePackage{verbatim}
%    \end{macrocode}
%
% \DescribeMacro{\skip@preamble}
% The core of the package. It works by redefining the |document| `environment,' thus making the |\begin| and |\end{document}| of the subfile `transparent' to the inclusion. The redefinition of |\documentclass| is analogous, just having a required and an optional arguments which mean nothing to |\subfile|.
%    \begin{macrocode}
\newcommand{\skip@preamble}{%
    \let\document\relax\let\enddocument\relax%
    \newenvironment{document}{}{}%
    \renewcommand{\documentclass}[2][subfiles]{}}
%    \end{macrocode}
% 
% \DescribeMacro{\subfile}
% Note that the new command |\subfile| calls for |\skip@preamble| \emph{within a group}. The changes to |document| and |\documentclass| are undone after the inclusion of the subfile. 
%    \begin{macrocode}
\newcommand\subfile[1]{\begingroup\skip@preamble\input{#1}\endgroup}
%</package>
%    \end{macrocode}

