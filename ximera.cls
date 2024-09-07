%%
%% This is file `ximera.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% ximera.dtx  (with options: `classXimera')
%% src/banner.dtx  (with options: `classXimera')
%% src/options.dtx  (with options: `classXimera')
%% src/packages.dtx  (with options: `classXimera')
%% src/tikzexport.dtx  (with options: `classXimera')
%% src/hyperref.dtx  (with options: `classXimera')
%% src/suppress.dtx  (with options: `classXimera')
%% src/makeCounter.dtx  (with options: `classXimera')
%% src/pagesetup.dtx  (with options: `classXimera')
%% src/metadata.dtx  (with options: `classXimera')
%% src/outcomes.dtx  (with options: `classXimera')
%% src/macros.dtx  (with options: `classXimera')
%% src/theorems.dtx  (with options: `classXimera')
%% src/enumerate.dtx  (with options: `classXimera')
%% src/proof.dtx  (with options: `classXimera')
%% src/only.dtx  (with options: `classXimera')
%% src/problem.dtx  (with options: `classXimera')
%% src/hints.dtx  (with options: `classXimera')
%% src/abstract.dtx  (with options: `classXimera')
%% src/title.dtx  (with options: `classXimera')
%% src/image.dtx  (with options: `classXimera')
%% src/interactives/javascript.dtx  (with options: `classXimera')
%% src/interactives/include.dtx  (with options: `classXimera')
%% src/interactives/geogebra.dtx  (with options: `classXimera')
%% src/interactives/desmos.dtx  (with options: `classXimera')
%% src/interactives/google.dtx  (with options: `classXimera')
%% src/interactives/graph.dtx  (with options: `classXimera')
%% src/link.dtx  (with options: `classXimera')
%% src/interactives/video.dtx  (with options: `classXimera')
%% src/answer.dtx  (with options: `classXimera')
%% src/choice.dtx  (with options: `classXimera')
%% src/solution.dtx  (with options: `classXimera')
%% src/feedback.dtx  (with options: `classXimera')
%% src/freeresponse.dtx  (with options: `classXimera')
%% src/verbatim.dtx  (with options: `classXimera')
%% src/dialogue.dtx  (with options: `classXimera')
%% src/instructornotes.dtx  (with options: `classXimera')
%% src/xkcd.dtx  (with options: `classXimera')
%% src/foldable.dtx  (with options: `classXimera')
%% src/leash.dtx  (with options: `classXimera')
%% src/interactives/sagemath.dtx  (with options: `classXimera')
%% src/jax.dtx  (with options: `classXimera')
%% src/ungraded.dtx  (with options: `classXimera')
%% src/clearEnv.dtx  (with options: `classXimera')
%% src/hideEnv.dtx  (with options: `classXimera')
%% ------------:| ------------------------------------------------------------
%%       ximera:| Simultaneously writing print and online interactive materials
%%       Author:| Jim Fowler and Oscar Levin and Jason Nowell and Wim Obbels and Hans Parshall and Bart Snapp
%%       E-mail:| bart.snapp@gmail.com
%%      License:| Released under the LaTeX Project Public License v1.3c or later
%%          See:| http://www.latex-project.org/lppl.txt
%% 
\NeedsTeXFormat{LaTeX2e}[1999/12/01]
\ProvidesClass{ximera}
 [2024/05/12 v1.5.1
 Simultaneously write print and online interactive materials]


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\let\m\message
\def\w{\m{::}}\def\t{\m{:::}}\def\s{\m{ }}\def\n{\m{^^J}}\def\u{::'}\def\d{'::}\def\v{:::::}
\n
\m{\d.}\s\m{.\u}\w\m{}\m{\v}\s\s\m{\v}\m{}\m{.\v\v\v::::.}\s\m{}\t\n
\s\m{\d.\u}\s\w\t\t\s\t\t\w\m{}\s\s\s\s\s\s\s\t\s\m{\u::}\n
\s\s\t\s\s\w\t\m{}\t\t\m{}\t\m{ ::::\v}\m{.\v:\u}\m{}\m{\u}\m{\d}\n
\s\m{.\u::.}\s\w\t\m{}\t\t\m{}\t\w\s\s\s\s\w\m{}\m{\d.}\s\m{\u}\s\m{\d}\n
\m{.\u}\s\m{\d.}\w\t\s\m{':\u}\s\t\m{\d\v::}\w\s\m{}\m{\d.\u}\s\s\m{\d}\n
\n
\let\w\undefined
\let\t\undefined
\let\s\undefined
\let\n\undefined
\let\u\undefined
\let\d\undefined
\let\v\undefined
\let\m\undefined
\newif\ifhandout
\handoutfalse
\DeclareOption{handout}{\handouttrue}
\newif\ifauthor%                                        Flag for whether or not any author is given
    \authorfalse%                                       Defaults to false.
\newif\ifsuppressAuthorDisplay%                         A flag for deliberately suppressing author even if it is given.
    \suppressAuthorDisplayfalse%                        Default to not suppressing the display of the author.
\DeclareOption{noauthor}{\suppressAuthorDisplaytrue}
\newif\ifoutcomes%                                      Flag for whether or not outcomes are given.
    \outcomesfalse%                                     Default to false.
\newif\ifsuppressOutcomesDisplay%                       Flag for if outcome displays should be suppressed, even if given.
    \suppressOutcomesDisplayfalse%                      Default to not supressing the display of outcomes
\newif\ifnumberedProblems
    \numberedProblemsfalse%                             Default to no numbers, as that was previous behavior.
\DeclareOption{nooutcomes}{\suppressOutcomesDisplaytrue}
\newif\ifinstructornotes
\instructornotesfalse
\DeclareOption{instructornotes}{\instructornotestrue}
\DeclareOption{noinstructornotes}{\instructornotestrue}
\newif\ifhints
\hintsfalse
\DeclareOption{hints}{\hintstrue}
\newif\ifnewpage
\newpagefalse
\DeclareOption{newpage}{\newpagetrue}
\newif\ifnumbers
\numbersfalse
\DeclareOption{numbers}{\numberstrue}
\newif\ifwordchoicegiven
\wordchoicegivenfalse
\DeclareOption{wordchoicegiven}{\wordchoicegiventrue}
\newif\iffirstinlinechoice% Support for other wordchoice command contents.
\firstinlinechoicetrue

\newif\ifxake
\xakefalse
\DeclareOption{xake}{\xaketrue}

\newif\iftikzexport
\tikzexportfalse
\DeclareOption{tikzexport}{%
  \tikzexporttrue%
  \handoutfalse%
  \numbersfalse%
  \newpagefalse%
  \hintsfalse%
  \outcomestrue%
}

\DeclareOption*{%
  \PassOptionsToClass{\CurrentOption}{article}%
}
\ProcessOptions\relax
\LoadClass{article}

\ifdefined\HCode
  \xaketrue%
  \tikzexporttrue%
  \handoutfalse%
  \numbersfalse%
  \newpagefalse%
  \hintsfalse%
  \outcomestrue%
\fi


\RequirePackage[makeroom]{cancel}
\RequirePackage[inline]{enumitem}
\RequirePackage[pagestyles]{titlesec}
\RequirePackage{titletoc}
\RequirePackage{titling}
\RequirePackage{url}
\RequirePackage[table]{xcolor}
\RequirePackage{tikz}
\RequirePackage{pgfplots}
\usepgfplotslibrary{groupplots}
\usetikzlibrary{calc}
\RequirePackage{fancyvrb}
\RequirePackage{forloop}
\RequirePackage{environ}% Included to allow saving of environment contents. This does *not* play well with various verbatem environments however and would be great to find a way around utilizing this package. One option is the "Suppress" command included below.
\RequirePackage{amssymb}% Included to have access to math typeset.
\RequirePackage{amsmath}% Included to have access to math typeset.
\RequirePackage{amsthm}%  Included to have access to math typeset.
\RequirePackage{xifthen}% http://ctan.org/pkg/xifthen
\RequirePackage{multido}% http://ctan.org/pkg/multido
\RequirePackage{listings} %% is this required???

\RequirePackage{xkeyval}

\RequirePackage{comment}
\RequirePackage{gettitlestring}
\RequirePackage{nameref}
\RequirePackage{epstopdf}
\ifdefined\HCode
  \tikzexporttrue
\fi

\iftikzexport
  \usetikzlibrary{external}

  \ifdefined\HCode
    % in htlatex, just include the svg files
    \def\pgfsys@imagesuffixlist{.svg}

    \tikzexternalize[prefix=./,mode=graphics if exists]
  \else
    % in pdflatex, actually generate the svg files
    \tikzset{
      /tikz/external/system call={
        pdflatex \tikzexternalcheckshellescape
        -halt-on-error -interaction=batchmode
        -jobname "\image" "\\PassOptionsToClass{tikzexport}{ximera}\texsource";
        mutool draw -F svg \image.pdf > \image.svg ;     % mutool adds "1" to filename ????
        mutool draw -o \image.svg \image.pdf ;
        mutool draw -r 150 -c rgbalpha -o \image.png \image.pdf ;
        ebb -x \image.png
      }
    }
    \tikzexternalize[optimize=false,prefix=./]
  \fi

  \fi

\ifdefined\HCode
\RequirePackage{hyperref}
\else
\RequirePackage[pdfpagelabels,colorlinks=true,allcolors=blue!30!black]{hyperref}
\pdfstringdefDisableCommands{\def\hskip{}}%% quiets warning
\fi
\font\dummyft@=dummy \relax
\def\suppress{%
  \begingroup\par
  \parskip\z@
  \offinterlineskip
  \baselineskip=\z@skip
  \lineskip=\z@skip
  \lineskiplimit=\maxdimen
  \dummyft@
  \count@\sixt@@n
  \loop\ifnum\count@ >\z@
    \advance\count@\m@ne
    \textfont\count@\dummyft@
    \scriptfont\count@\dummyft@
    \scriptscriptfont\count@\dummyft@
  \repeat
  \let\selectfont\relax
  \let\mathversion\@gobble
  \let\getanddefine@fonts\@gobbletwo
  \tracinglostchars\z@
  \frenchspacing
  \hbadness\@M}
\def\endsuppress{\par\endgroup}
\newcommand{\Make@Counter}[1]{% This is an internal macro that should probably be depreciated for the below universal macro.
  \@ifundefined{c@#1}% Check to see if counter exists
       {     % If not, create it and set it to 0.
\newcounter{#1}
\setcounter{#1}{0}
}
{%If so, reset to 0, but throw a warning.
\setcounter{#1}{0}
        \PackageWarning{MakeCounter}{The Counter #1 already existed so we reset it to zero. Hope this doesn't break anything!}
}
}

\newcommand{\MakeCounter}[1]{%% Code located in "Utilitymacros.dtx"
  \@ifundefined{c@#1}% Check to see if counter exists
       {     % If not, create it and set it to 0.
\newcounter{#1}
\setcounter{#1}{0}
}
{%If so, reset to 0, but throw a warning.
\setcounter{#1}{0}
        \PackageWarning{MakeCounter}{The Counter #1 already existed so we reset it to zero. Hope this doesn't break anything!}
}
}

\setlength{\parindent}{0pt}
\setlength{\parskip}{5pt}
\oddsidemargin 62pt
\evensidemargin 62pt
\textwidth 345pt
\headheight 14pt

\newcommand{\license}{\excludecomment}

\newcommand{\acknowledgement}{\excludecomment}

\renewcommand{\tag}{\excludecomment}
\def\theoutcomes{}

\ifdefined\HCode%
  \newcommand{\outcome}[1]{}
\else%
  \newwrite\outcomefile
  \immediate\openout\outcomefile=\jobname.oc

  \newcommand{\outcome}[1]{
      \outcomestrue% If we invoke |\outcome|, then we should have outcomes and thus we will flag it as true.
      \edef\theoutcomes{\theoutcomes #1~}%
      \immediate\write\outcomefile{\unexpanded{\outcome}{#1}}
      }
  \fi%
\everymath{\displaystyle}
\let\prelim\lim
\renewcommand{\lim}{\displaystyle\prelim}
\theoremstyle{definition} % No italic (because this makes also text in TikZ italic !!!)


      \newtheorem{theorem}{Theorem}

      \newtheorem{algorithm}{Algorithm}

      \newtheorem{axiom}{Axiom}

      \newtheorem{claim}{Claim}

      \newtheorem{conclusion}{Conclusion}

      \newtheorem{condition}{Condition}

      \newtheorem{conjecture}{Conjecture}

      \newtheorem{corollary}{Corollary}

      \newtheorem{criterion}{Criterion}

      \newtheorem{definition}{Definition}

      \newtheorem{example}{Example}

      \newtheorem*{explanation}{Explanation}

      \newtheorem{fact}{Fact}

      \newtheorem{lemma}{Lemma}

      \newtheorem{formula}{Formula}

      \newtheorem{idea}{Idea}

      \newtheorem{notation}{Notation}

      \newtheorem{model}{Model}

      \newtheorem{observation}{Observation}

      \newtheorem{proposition}{Proposition}

      \newtheorem{paradox}{Paradox}

      \newtheorem{procedure}{Procedure}

      \newtheorem{remark}{Remark}

      \newtheorem{summary}{Summary}

      \newtheorem{template}{Template}

      \newtheorem{warning}{Warning}

\renewcommand{\theenumi}{\textup{(\alph{enumi})}}
\renewcommand{\labelenumi}{\theenumi}
\renewcommand{\theenumii}{\textup{(\roman{enumii})}}
\renewcommand{\labelenumii}{\theenumii}
\renewcommand{\qedsymbol}{$\blacksquare$}
\renewenvironment{proof}[1][\proofname]
  {\begin{trivlist}\item[\hskip \labelsep \itshape \bfseries #1{}\hspace{2ex}]}
{\qed\end{trivlist}}

\ifxake
        \newenvironment{prompt}{}{}
\else
\ifhandout
\NewEnviron{prompt}{}
\else
\newenvironment{prompt}
     {\bgroup\color{gray!50!black}}
        {\egroup}
\fi
\fi

\ifhandout
\NewEnviron{onlineOnly}{
\iftikzexport
\BODY
\else
\fi
}
\else
\newenvironment{onlineOnly}
     {\bgroup\color{red!50!black}}
{\egroup}
\fi

\newcommand{\pdfOnly}[1]{\iftikzexport\else #1\fi}


\providecommand{\latexProblemContent}[1]{#1}
\Make@Counter{Iteration@probCnt}
\newcommand{\hang}{% top theorem decoration
  \begingroup%
  \setlength{\unitlength}{.005\linewidth}% \linewidth/200
    \begin{picture}(0,0)(1.5,0)%
      \linethickness{1pt} \color{black!50}%
      \put(-3,2){\line(1,0){206}}% Top line
      \multido{\iA=2+-1,\iB=50+-10}{5}{% Top hangs
        \color{black!\iB}%
        \put(-3,\iA){\line(0,-1){1}}% Top left hang
        %\put(203,\iA){\line(0,-1){1}}% Top right hang
      }%
    \end{picture}%
  \endgroup%
}%
\newcommand{\hung}{% bottom theorem decoration
  \nobreak
  \begingroup%
    \setlength{\unitlength}{.005\linewidth}% \linewidth/200
    \begin{picture}(0,0)(1.5,0)%
      \linethickness{1pt} \color{black!50}%
      \put(60,0){\line(1,0){143}}% Bottom line
      \multido{\iA=0+1,\iB=50+-10}{5}{% Bottom hangs
        \color{black!\iB}%
        %\put(-3,\iA){\line(0,1){1}}% Bottom left hang
        \put(203,\iA){\line(0,1){1}}% Bottom right hang
        \put(\iB,0){\line(60,0){10}}% Left fade out
      }%
    \end{picture}%
  \endgroup%
}%

\MakeCounter{problem}
\newcommand{\problemNumber}{
\ifcsname c@depth\Roman{problem@Depth}Count\endcsname% Check to see if counter exists
\else
\expandafter\newcounter{depth\Roman{problem@Depth}Count}
\expandafter\setcounter{depth\Roman{problem@Depth}Count}{0}
\fi

\expandafter\stepcounter{depth\Roman{problem@Depth}Count}
\arabic{depthICount}% The first problem depth, what use to be |\theproblem|.
\forloop{Iteration@probCnt}{2}{\arabic{Iteration@probCnt} < \numexpr \value{problem@Depth} + 1 \relax}{%
    .\expandafter\arabic{depth\Roman{Iteration@probCnt}Count}% Get the problem number of the next depth level and append it with a ".".
}
}

%%%%%% Configure various problem environment commands
\Make@Counter{problem@Depth}

%%%% Configure environments start content

\newcommand{\problemEnvironmentStart}[2]{%
\stepcounter{problem@Depth}% Started a problem, so we've sunk another problem layer.
\def\spaceatend{#1}%
\begin{trivlist}%
\item%
  [%
    \hskip\labelsep\sffamily\bfseries
    #2 \problemNumber% Determine the correct number of the problem, and the format of that number.
]%
\slshape
}

%%%%% Configure environments end content

\newcommand{\problemEnvironmentEnd}{%This configures all the end content for a problem.
\stepcounter{problem@Depth}
\ifcsname c@depth\Roman{problem@Depth}Count\endcsname
\expandafter\ifnum\expandafter\value{depth\Roman{problem@Depth}Count}>0
\expandafter\setcounter{depth\Roman{problem@Depth}Count}{0}
\fi
\fi

\addtocounter{problem@Depth}{-2}% Exited a problem so we've exited a problem layer. Need -2 because we steppped once at the start to check for needing a depth-level count reset.

\par\addvspace{.5ex}\nobreak\noindent\hung %% line at the bottom

\ifhandout
\ifnewpage
\newpage
\fi
\fi
\end{trivlist}
}

%%%% Now populate the old environment names

\newenvironment{problem}[1][2in]%
{%Env start code
\problemEnvironmentStart{#1}{Problem}
}
{%Env end code
\problemEnvironmentEnd
}

\newenvironment{exercise}[1][2in]%
{%Env start code
\problemEnvironmentStart{#1}{Exercise}
}
{%Env end code
\problemEnvironmentEnd
}

\newenvironment{exploration}[1][2in]%
{%Env start code
\problemEnvironmentStart{#1}{Exploration}
}
{%Env end code
\problemEnvironmentEnd
}

\newenvironment{question}[1][2in]%
{%Env start code
\problemEnvironmentStart{#1}{Question}
}
{%Env end code
\problemEnvironmentEnd
}

\newcounter{hintLevel}
\setcounter{hintLevel}{0}
\newenvironment{hint}{}{}
\renewenvironment{hint}
{
\ifhandout
\setbox0\vbox\bgroup
\else
\begin{trivlist}\item[\hskip \labelsep\small\slshape\bfseries Hint:\hspace{2ex}]
\small\slshape
\fi
\stepcounter{hintLevel}
}
{
\ifhandout
\egroup\ignorespacesafterend
\else
\end{trivlist}
\fi
\addtocounter{hintLevel}{-1}
}

\ifhints
\renewenvironment{hint}{
\begin{trivlist}\item[\hskip \labelsep\small\slshape\bfseries Hint:\hspace{2ex}]
\small\slshape}
{\end{trivlist}}
\fi

\let\abstract\relax% We kill off abstract as we will want to remake it as an Environ to capture the BODY
\let\endabstract\relax
\NewEnviron{abstract}{\protected@xdef\theabstract{\BODY}}% Define abstract as an Environ and capture BODY
\let\@emptyauthor\@author
\def\author#1{
    \gdef\@author{#1}% Defines activity author
    \authortrue% Flags author as given.
    }
\def\@author{\@latex@warning@no@line{No \noexpand\author given}}
\def\and{and }

\let\title\relax
\newcommand{\title}[1][]{{\protected@xdef\@pretitle{#1}}\protected@xdef\@title}

\title{}

\newcounter{titlenumber}
\renewcommand{\thetitlenumber}{\arabic{titlenumber}}
\setcounter{titlenumber}{0}

\newpagestyle{main}{
\sethead[\textsl{\ifnumbers\thetitlenumber\hspace{1em}\fi\@title}][][] % even
{}{}{\textsl{\ifnumbers\thetitlenumber\hspace{1em}\fi\@title}} % odd
\setfoot[\thepage][][] % even
{}{}{\thepage} % odd
}
\pagestyle{main}
\renewcommand\maketitle{%
    \addtocounter{titlenumber}{1}%
    {\flushleft\large\bfseries \@pretitle\par\vspace{-1em}}
    {\flushleft\LARGE\bfseries {\ifnumbers\thetitlenumber\fi}{\ifnumbers\hspace{1em}\else\hspace{0em}\fi}\@title \par }
    \phantomsection%
    \ifnumbers\addcontentsline{toc}{section}{\thetitlenumber~\@title}\else\addcontentsline{toc}{section}{\@title}\fi
    \@ifundefined{theabstract}{% Only include the abstract if it is defined.
            \relax% if the abstract is not included, then we throw in a dummy command to make sure that followup actions don't throw errors  
            %           due to expecting something to happen from the abstract.
        }
        {% If the abstract is defined, then we can print it with some formatting.
            \vskip .6em\noindent\textit\theabstract
        }
    \setcounter{problem}{0}\setcounter{section}{0}\setcounter{subsection}{0}\par\vspace{2em}
  %%% Design Question: 
  % Would it make more sense to have author/outcomes in the header and reserve footer for footnotes? 
  % Or some other method to distinguish between the two now that footnotes are viable?
  \ifoutcomes%                                          If we have learning outcomes, we should display them.
      \bgroup
          \let\thefootnote\relax%                       Kills off the symbol of the footnote
          \footnote{Learning outcomes: \theoutcomes}
          \setcounter{footnote}{0}%                     Reset the footnote to zero to fix numberings.
      \egroup
      \fi
  \ifauthor%                                            If we have an author, we should display it.
      \bgroup
          \let\thefootnote\relax%                       Kills off the symbol of the footnote
          \footnote{Author(s):~\@author}
          \setcounter{footnote}{0}%                     Reset the footnote to zero to fix numberings.
      \egroup
      \fi
  \aftergroup\@afterindentfalse
  \aftergroup\@afterheading}

\ifnumbers
\setcounter{secnumdepth}{2}
\renewcommand{\thesection}{\arabic{titlenumber}.\arabic{section}}
\renewcommand{\thesubsection}{\arabic{titlenumber}.\arabic{section}.\arabic{subsection}}
\else
\setcounter{secnumdepth}{-2}
\fi

\def\activitystyle{}
\newcounter{sectiontitlenumber}
\setcounter{secnumdepth}{2}
\setcounter{tocdepth}{2}
\newcommand\chapterstyle{%
  \def\activitystyle{activity-chapter}
  \def\maketitle{%
    \addtocounter{titlenumber}{1}%
                    {\flushleft\small\sffamily\bfseries\@pretitle\par\vspace{-1.5em}}%
                    {\flushleft\LARGE\sffamily\bfseries\thetitlenumber\hspace{1em}\@title \par }%
                    {\vskip .6em\noindent\textit\theabstract\setcounter{problem}{0}\setcounter{section}{0}}%
                    \par\vspace{2em}
                    \phantomsection\addcontentsline{toc}{section}{\textbf{\thetitlenumber\hspace{1em}\@title}}%
}}

\newcommand\sectionstyle{%
  \def\activitystyle{activity-section}
  \def\maketitle{%
    \addtocounter{section}{1}
    \setcounter{sectiontitlenumber}{\value{section}}
    {\flushleft\small\sffamily\bfseries\@pretitle\par\vspace{-1.5em}}%
    {\flushleft\Large\sffamily\bfseries\thetitlenumber.\thesectiontitlenumber\hspace{1em}\@title \par}%
    {\vskip .6em\noindent\textit\theabstract\setcounter{subsection}{0}}%
    \par\vspace{2em}
    \phantomsection\addcontentsline{toc}{section}{\thetitlenumber.\thesectiontitlenumber\hspace{1em}\@title}%
 \renewcommand\section{\@startsection{subsection}{2}{\z@}%
                                       {-3.25ex\@plus -1ex \@minus -.2ex}%
                                       {1.5ex \@plus .2ex}%
                                       {\normalfont\large\bfseries}}

 \renewcommand\subsection{\@startsection{subsubsection}{3}{\z@}%
                                          {-3.25ex\@plus -1ex \@minus -.2ex}%
                                          {1.5ex \@plus .2ex}%
                                          {\normalfont\normalsize\bfseries}}

}}

\iftikzexport%% allows xake to handle \chapterstyle and \sectionstye
\renewcommand\chapterstyle{\def\activitystyle{chapter}}
\renewcommand\sectionstyle{\def\activitystyle{section}}
\else
\fi

\NewEnviron{image}[1][3in]{%
  \begin{center}\resizebox{#1}{!}{\BODY}\end{center}% resize and center
}
\newcommand{\alt}[1]{}


\DefineVerbatimEnvironment{javascript}{Verbatim}{numbers=left,frame=lines,label=JavaScript,labelposition=topline}


\def\js#1{\mbox{\texttt{\detokenize{#1}}}}


\define@key{interactive}{id}{\def\interactive@id{#1}}
\setkeys{interactive}{id=}
\newcommand{\includeinteractive}[2][]{
\setkeys*{interactive}{#1}%
\ifthenelse{\equal{\interactive@id}{}}{}{\recordvariable{\interactive@id}}
Interactive
}

\newcommand{\geogebra}[3]{Geogebra link: \url{https://www.geogebra.org/m/#1}}
\newcommand{\desmos}[3]{Desmos link: \url{https://www.desmos.com/calculator/#1}}
\newcommand{\desmosThreeD}[3]{Desmos3D link: \url{https://www.desmos.com/3d/#1}}
\newcommand{\googleSheet}[5]{%
  Google Spreadsheet link: \url{https://docs.google.com/spreadsheets/d/#1}%
}


\newcommand{\graph}[2][]{\text{Graph of $#2$}}


\newcommand*{\link}[2][]{#1%
\ifthenelse{\equal{#1}{}}%
{\footnote{Link: \url{#2}}}%
{\footnote{See #1 at \url{#2}}}
}

\newcommand{\youtube}[1]{YouTube link: \url{https://www.youtube.com/watch?v=#1}}

\ifdefined\HCode
\newcommand{\recordvariable}[1]{}
\else
\newwrite\idfile
\immediate\openout\idfile=\jobname.ids
\newcommand{\recordvariable}[1]{\ifthenelse{\equal{#1}{}}{}{\immediate\write\idfile{var #1;}}}
\fi
\define@key{answer}{given}[true]{\def\ans@given{#1}}
\define@key{answer}{tolerance}{\def\ans@tol{#1}}
\define@key{answer}{validator}{}
\define@key{answer}{id}{\def\ans@id{#1}}
\define@key{answer}{format}{}
\define@key{answer}{onlinenoinput}[false]{}
\define@key{answer}{onlineshowanswerbutton}[false]{}
\setkeys{answer}{id=,given=false,onlinenoinput=false,onlineshowanswerbutton=false}

\newcommand{\answerFormatLength}{2cm}

\newcommand{\answerFormatDots}[1]{\ldots\ldots}
\newcommand{\answerFormatLine}[1]{\protect\rule{\answerFormatLength}{0.4pt}}
\newcommand{\answerFormatFlexibleLine}[1]{\protect\rule{\widthof{$#1$}*2}{0.4pt}}
\newcommand{\answerFormatFlexibleBox}[1]{\fbox{\scalebox{2}{\phantom{$#1$}}}}

\newcommand{\answerFormatPlain}[1]{\ensuremath{#1}}
\newcommand{\answerFormatBlue}[1]{\color{blue}\ensuremath{#1}}
\newcommand{\answerFormatBoxed}[1]{\fbox{\ensuremath{#1}}}
\newcommand{\answerFormatBoxedGiven}[1]{\underset{\scriptstyle\mathrm{given}}{\fbox{\ensuremath{#1}}}}

\let\handoutAnswerFormat\answerFormatDots
\let\defaultAnswerFormat\answerFormatBlue
\let\givenAnswerFormat\answerFormatBoxedGiven

\newcommand{\answer}[2][]{%
\ifmmode%
\setkeys{answer}{#1}%
\recordvariable{\ans@id}
\ifthenelse{\boolean{\ans@given}}e s
{% Start then statement
\ifhandout
#2
\else
\givenAnswerFormat{#2} %% in case the argument helps formatting
\fi
}% End then statement
{% Start else statement
\ifhandout
\handoutAnswerFormat{#2} %% in case the argument helps formatting
\else% show answer in box outside handout mode
\defaultAnswerFormat{#2} %% in case the argument helps formatting
\fi
}% End else statement
\else%
\GenericError{\space\space\space\space}% Throw an error if the \answer command is not in mathmode -- Jason
{Attempt to use \@backslashchar answer outside of math mode}
{See https://github.com/ximeraProject/ximeraLatex for explanation.}
{Need to use either inline or display math.}%
\fi
}
\define@key{choice}{value}[]{\def\choice@value{#1}}
\define@boolkey{choice}{correct}[true]{\def\choice@correct{#1}}
\define@key{multipleChoice}{id}{\def\mc@id{#1}}
\define@key{otherchoice}{value}[]{\def\otherchoice@value{#1}}
\define@boolkey{otherchoice}{correct}[true]{\def\otherchoice@correct{#1}}
\setkeys{choice}{correct=false,value=}
\setkeys{multipleChoice}{id=}
\setkeys{otherchoice}{correct=false,value=}
\newcommand{\choice}[2][]{%
\setkeys{choice}{#1}%
\item{#2}
\ifthenelse{\boolean{\choice@correct}}
    {% Begin then result
    \ifhandout% if it's a handout do nothing.
    \else% otherwise place a checkmark when you select the "correct choice"... maybe? -- Jason
        \,\checkmark\,\setkeys{choice}{correct=false}
    \fi
    }% End then result
    {}% Begin/End else result.
}

\newcommand{\choiceEXP}[2][]{%
\expandafter\setkeys\expandafter{choice}{#1}%
\item{#2}
\ifthenelse{\boolean{\choice@correct}}
{% Begin then result
\ifhandout
\else
\,\checkmark\,\setkeys{choice}{correct=false}
\fi
}% End then result
{}% Begin/End else result.
} %% note all the {} are needed in case the choice has [] in it.

\newcommand{\otherchoice}[2][]{%
\ignorespaces%
\setkeys{otherchoice}{#1}%
\ifthenelse{\boolean{\otherchoice@correct}}%
{% Start then result
#2\ignorespaces\setkeys{otherchoice}{correct=false}\ignorespaces%
}% End then result
{}% Start/End else result
\ignorespaces%
}%

\newcommand{\inlinechoice}[2][]{%
\setkeys{choice}{#1}%
\iffirstinlinechoice
(\hspace{-.25em}
\firstinlinechoicefalse
\else
/
\fi
#2
\ifthenelse{\boolean{\choice@correct}}%
{% Start then result
\ifhandout\else\checkmark\ignorespaces\setkeys{choice}{correct=false}\ignorespaces\fi%
}% End then result
{}% Start/End else result
\hspace{-.25em}\ignorespaces%
}


\newenvironment{multipleChoice}[1][]
{% Environment Start Code
\setkeys{multipleChoice}{#1}%
\recordvariable{\mc@id}%
\begin{trivlist}
\item[\hskip \labelsep\small\bfseries Multiple Choice:]\hfil
\begin{enumerate}
}% Note this means that \item has to be the first line after \begin{multipleChoice}.
{% Environment End Code
\end{enumerate}
\end{trivlist}
}

\newenvironment{multipleChoice@}[1][]{}{)}


\newcommand{\wordChoice}[1]{%
\let\choicetemp\choice% Assign a "choicetemp" command to duplicate choice.
\ifwordchoicegiven% If wordchoice option is on, we need to juggle around some definitions.
\let\choice\otherchoice%
#1
\else% If it isn't the regular "choice" command should work.
\let\choice\inlinechoice%
\begin{multipleChoice@}%
#1%
\end{multipleChoice@}%
\fi%
\let\choice\choicetemp% Now that choicetmp has been manipulated to what we want, replace choice with it.
}%


\newenvironment{selectAll}[1][]
{\begin{trivlist}\item[\hskip \labelsep\small\bfseries Select All Correct Answers:]\hfil\begin{enumerate}}
    {\end{enumerate}\end{trivlist}}



%% solution environment
\ifhandout % what follows is handout behavior
\newenvironment{solution}%
       {%
 \setbox0\vbox\bgroup
       }
               {%
 \egroup
       }
\else
\newenvironment{solution}%
       {%
 \begin{trivlist}
 \item[\hskip \labelsep\bfseries Solution:\hspace{2ex}]
       }
       % %% line at the bottom}
       {
\end{trivlist}
 \par\addvspace{.5ex}\nobreak\noindent\hung
       }
\fi

\newcommand{\PH@Command}{}
\newenvironment{validator}[1][]
    {% Start env code.
   \def\PH@Command{#1}% Use PH@Command to hold the content and be a target for "\expandafter" to expand once.
   \mbox{\texttt{\detokenize\expandafter{\PH@Command}}}% Now expand PH@Command once and then detokenize.
}
    {% End env code.
    }
\ifhandout%
\newenvironment{feedback}
    {% Start env code.
        \setbox0\vbox\bgroup
    }
    {% End env code.
        \egroup
    }
\else
\newenvironment{feedback}[1][attempt]{

\def\PH@Command{#1}% Use PH@Command to hold the content and be a target for "\expandafter" to expand once.

\begin{trivlist}% Begin the trivlist to use formating of the "Feedback" label.
\item[\hskip \labelsep\small\slshape\bfseries Feedback% Format the "Feedback" label. Don't forget the space.
(\texttt{\detokenize\expandafter{\PH@Command}}):% Format (and detokenize) the condition for feedback to trigger
\hspace{2ex}]\small\slshape% Insert some space before the actual feedback given.
}{
\end{trivlist} 
}

\fi
\newboolean{given} %% required for freeResponse
\setboolean{given}{true} %% could be replaced by a key=value pair later if needed

\ifhandout
\newenvironment{freeResponse}[1][false]%
{%
\def\givenatend{\boolean{#1}}
\ifthenelse{\boolean{#1}}
{% Begin then result
\begin{trivlist}
\item
}% End then result
{% Begin else result
\setbox0\vbox\bgroup
}% End else result
}
{%
\ifthenelse{\givenatend}
{% Begin then result
\end{trivlist}
}% End then result
{% Begin else result
\egroup
}% End else result
}
\else
\newenvironment{freeResponse}[1][false]%
{% Environment Beginning Code
  \ifthenelse{\boolean{#1}}%% Could probably change this with just putting the (given) in the boolean?-- Jason
   {% Begin then result
   \begin{trivlist}
   \item[\hskip \labelsep\bfseries Free Response (Given):\hspace{2ex}]
   }% End then result
{% Begin else result
\begin{trivlist}
\item[\hskip \labelsep\bfseries Free Response:\hspace{2ex}]
}% End else result
}
{% Environment Ending Code
\end{trivlist}
}
\fi


\DefineVerbatimEnvironment{code}{Verbatim}{numbers=left,frame=lines,label=Code,labelposition=topline}
\DefineVerbatimEnvironment{python}{Verbatim}{numbers=left,frame=lines,label=Python,labelposition=topline}
\DefineVerbatimEnvironment{javascriptCode}{Verbatim}{numbers=left,frame=lines,label=JavaScript,labelposition=topline}
\newenvironment{dialogue}{%
   \renewcommand\descriptionlabel[1]{\hspace{\labelsep}\textbf{##1:}}
   \begin{description}%
}{%
   \end{description}%
}

%% instructor intro/instructor notes
%%
\ifhandout % what follows is handout behavior
\ifinstructornotes
\newenvironment{instructorIntro}%
       {%
 \begin{trivlist}
 \item[\hskip \labelsep\bfseries Instructor Introduction:\hspace{2ex}]
}
       % %% line at the bottom}
       {
 \end{trivlist}
 \par\addvspace{.5ex}\nobreak\noindent\hung
       }
\else
\newenvironment{instructorIntro}%
       {%
 \setbox0\vbox\bgroup
       }
       {%If this mysteriously starts breaking
                        % remove \ignorespacesafterend
 \egroup\ignorespacesafterend
       }
               \fi
\else% for handout, so what follows is default
\ifinstructornotes
\newenvironment{instructorIntro}%
       {%
         \setbox0\vbox\bgroup
       }
{%
  \egroup
}
                \else
        \newenvironment{instructorIntro}%
{%
  \begin{trivlist}
  \item[\hskip \labelsep\bfseries Instructor Introduction:\hspace{2ex}]
}
{
  \end{trivlist}
  \par\addvspace{.5ex}\nobreak\noindent\hung
}
                \fi
\fi

%% instructorNotes environment
\ifhandout % what follows is handout behavior
\ifinstructornotes
\newenvironment{instructorNotes}%
       {%
 \begin{trivlist}
 \item[\hskip \labelsep\bfseries Instructor Notes:\hspace{2ex}]
       }
       % %% line at the bottom}
       {
\end{trivlist}
 \par\addvspace{.5ex}\nobreak\noindent\hung
       }
       \else
\newenvironment{instructorNotes}%
       {%
         \setbox0\vbox\bgroup
       }
{%
  \egroup
}
                \fi
\else% for handout, so what follows is default
\ifinstructornotes
\newenvironment{instructorNotes}%
       {%
 \setbox0\vbox\bgroup
       }
       {%
 \egroup
       }
       \else
       \newenvironment{instructorNotes}%
              {%
        \begin{trivlist}
        \item[\hskip \labelsep\bfseries Instructor Notes:\hspace{2ex}]
              }
              % %% line at the bottom}
              {
        \end{trivlist}
        \par\addvspace{.5ex}\nobreak\noindent\hung
              }
                      \fi
                              \fi

\newcommand{\xkcd}[1]{#1}

\colorlet{textColor}{black} % since textColor is referenced below
\colorlet{background}{white} % since background is referenced below

%% pretty-foldable
\newenvironment{foldable}{%
}{%
}

%% pretty-expandable
\newenvironment{expandable}{%
}{%
}

\newcommand{\unfoldable}[1]{#1}


\newenvironment{leash}[1]{%
}{%
}


\IfFileExists{sagetex.sty}{\RequirePackage{sagetex}}{}

\DefineVerbatimEnvironment{sageCell}{Verbatim}{numbers=left,frame=lines,label=SAGE,labelposition=topline}

\DefineVerbatimEnvironment{sageOutput}{Verbatim}{numbers=left,frame=lines,label=SAGE-Output,labelposition=topline}


\ifdefined\HCode
  \else
    \newwrite\myfile
    \immediate\openout\myfile=\jobname.jax
\fi
\ifdefined\HCode
  \else
    \immediate\write\myfile{\unexpanded{\newenvironment}{prompt}{}{}}
\fi
\ifdefined\HCode
  \else
\let\@oldargdef\@argdef
\long\def\@argdef#1[#2]#3{%
\immediate\write\myfile{\unexpanded{\newcommand}{\unexpanded{#1}}[\unexpanded{#2}]{\unexpanded{#3}}}%
\@oldargdef#1[#2]{#3}%
}

\let\@OldDeclareMathOperator\DeclareMathOperator
\renewcommand{\DeclareMathOperator}[2]{\@OldDeclareMathOperator{#1}{#2}\immediate\write\myfile{\unexpanded{\DeclareMathOperator}{\unexpanded{#1}}{\unexpanded{#2}}}}

\fi
\newenvironment{ungraded}{}{}
\newcommand{\clearEnv}[1]{
    %% Syntax: \clearEnv{envName}
    %   #1 is envName is the name of the environment you want to clear. This will (re)define the
    %       begin and end environments, as well as the internal counter from \newtheoremenv to \relax.
    \expandafter\let\csname #1\endcsname\relax
    \expandafter\let\csname end#1\endcsname\relax
    \expandafter\let\csname c@#1\endcsname\relax
    }

\newcommand{\hideEnv}[2][]{
    %% Syntax: \hideEnv[NewCode]{environmentName}

    \expandafter\def\csname temp@#2\endcsname\csname #2\endcsname
    \expandafter\def\csname temp@end#2\endcsname\csname end#2\endcsname
    \expandafter\def\csname#2\endcsname{
        \ifnum\value{depth@Check}<1
        Suppress Should start here.
        \suppress
        \fi
        \stepcounter{depth@Check}

        }
    \expandafter\def\csname end#2\endcsname{
        \addtocounter{depth@Check}{-1}
        \ifnum\value{depth@Check}<1
        Suppress Should End Here.
        \endsuppress
        \fi

    \noexpand{#1}
    }
    }


%%%% Below this is testing content.

%%======================Code for \UDcollectverbarg=============================
%% \UDcollectverbarg{<non-optional 1>}{<non-optional 2>}|<verbatim arg>|
%% 
%% reads <verbatim arg> under verbatim-catcode-regime and delivers:
%%
%%    <non-optional 1>{<non-optional 2>{<verbatim arg>}}
%%-----------------------------------------------------------------------------
%% \UDcollectverbarg*{<non-optional 1>}{<non-optional 2>}|<verbatim arg>|
%% 
%% reads <verbatim arg> under verbatim-catcode-regime and delivers:
%%
%%    <non-optional 1>{<non-optional 2>{|<verbatim arg>|}}
%%-----------------------------------------------------------------------------
%% Instead of using verbatim-delimiter | or the like the <verbatim arg> can 
%% be nested in braces.
%%
%% You cannot use percent or spaces or horizontal tab as verbatim-delimiter.
%%
%% You can use <non-optional 1> for nesting calls to \UDcollectverbarg.
%%
%% The unstarred variant removes the verbatim-delimiters/braces that surround
%% <verbatim arg>.
%% The starred variant keeps the verbatim-delimiters/braces that surround
%% <verbatim arg>.
%% Reason: When you feed things to \scantokens you don't need the verbatim-
%%         delimiters.
%%         When you use things for writing to temporary files and reading back
%%         verbatimized, you may need them.
%%=============================================================================
\@ifdefinable\UDcollectverbarg{%
  \DeclareRobustCommand\UDcollectverbarg{%
    \@ifstar{\UD@collectverbarg{\@secondoftwo}}{\UD@collectverbarg{\@firstoftwo}}%
  }%
}%
\newcommand\UD@collectverbarg[3]{%
  % #1 - indicator whether to remove/keep the verb-delimiter (\@firstoftwo/\@secondoftwo)
  % #2 - non-optional 1
  % #3 - non-optional 2
  \begingroup
  \let\do\@makeother % <- this and the next line switch to
  \dospecials        %    verbatim-category-code-régime.
  \catcode`\{=1      % <- give opening curly brace the usual catcode so a 
                     %    curly-brace-balanced argument can be gathered in
                     %    case of the first thing of the verbatimized-argument 
                     %    being a curly opening brace.
  \catcode`\ =10     % <- give space and horizontal tab the usual catcode so \UD@@collectverbarg
  \catcode`\^^I=10   %    cannot catch a space or a horizontal tab as its 4th undelimited argument.
                     %    (Its 4th undelimited argument denotes the verbatim-
                     %     syntax-delimiter in case of not gathering a
                     %     curly-brace-nested argument.)
  \catcode`\%=14     % <- make percent comment.
  \kernel@ifnextchar\bgroup
  {% seems a curly-brace-nested argument is to be picked:
    \catcode`\}=2    % <- give closing curly brace the usual catcode also.
    \UD@@collectverbarg{#1}{\@firstoftwo}{#2}{#3}{}%
  }{% seems an argument with verbatim-syntax-delimiter is to be picked:
    \do\{% <- give opening curly brace the verbatim-catcode again.
    \UD@@collectverbarg{#1}{\@secondoftwo}{#2}{#3}%
  }%
}%
\newcommand\UD@@collectverbarg[5]{%
  % #1 - indicator whether to remove/keep the verb-delimiter (\@firstoftwo/\@secondoftwo)
  % #2 - indicator whether braces or verb-delimiter (\@firstoftwo/\@secondoftwo)
  % #3 - non-optional 1
  % #4 - non-optional 2
  % #5 - verb-delimiter or emptiness
  \do\ %   <- Now that \UD@@collectverbarg has the delimiter or
  \do\^^I%    emptiness in its 4th arg, give space and horizontal tab
         %    the verbatim-catcode again.
  \do\^^M% <- Give the carriage-return-character the verbatim-catcode.
  \do\%%   <- Give the percent-character the verbatim-catcode.
  \long\def\@tempb##1#5{%
    #1{\def\@tempb{##1}}{#2{\def\@tempb{{##1}}}{\def\@tempb{#5##1#5}}}%
    \@onelevel@sanitize\@tempb % <- Turn characters into their "12/other"-pendants.
                               %    This may be important with things like the 
                               %    inputenc-package which may make characters 
                               %    active/which give them catcode 13(active).
    \expandafter\UD@@@collectverbarg\expandafter{\@tempb}{#3}{#4}% <- this "spits out the result.
  }%
  \@tempb
}%
\newcommand\UD@@@collectverbarg[3]{\endgroup#2{#3{#1}}}%
%%================= End of code for \UDcollectverbarg =========================

\RequirePackage{trimclip}

\newbox\MYverbbox
\newtoks\MYverbtoks
\global\MYverbtoks={}%
\newcommand\exchange[2]{#2#1}%
\@ifdefinable\stopromannumeral{\chardef\stopromannumeral=`\^^00}%
\begingroup
\catcode`\^^M=12\relax%
\@firstofone{%
  \endgroup%
  \newcommand\@@appendtoverbtoks[1]{\global\MYverbtoks=\expandafter{\the\MYverbtoks#1^^M}\@esphack}%
  \newcommand\@@prependtoverbtoks[1]{%
    \global\MYverbtoks=%
    \expandafter\expandafter\expandafter{\expandafter\exchange\expandafter{\the\MYverbtoks}{#1^^M}}%
    \@esphack%
  }%
}%
\newcommand\appendtoverbtoks{\@bsphack\UDcollectverbarg{\@firstofone}{\@@appendtoverbtoks}}%
\newcommand\prependtoverbtoks{\@bsphack\UDcollectverbarg{\@firstofone}{\@@prependtoverbtoks}}%
\newcommand\scantokenssetvbox[4]{%
  \setbox#2=\vbox{%
    \begingroup\newlinechar=\endlinechar\relax\scantokens{\endgroup#3#1#4}%
  }%
  %---------------------------------------------------------------------------
  %Just in case - who knows what else already is in the \AtEndDocument-hook...
  \ifhmode\@bsphack\clipbox*{0pt 0pt 0pt 0pt}{\box#2}\@esphack
  \else\clipbox*{0pt 0pt 0pt 0pt}{\box#2}\fi
  %---------------------------------------------------------------------------
  %Uncomment to typeset the box:
  %\ifvmode\else\par\fi\box#2%
  %You may wish to check for restricted horizontal mode and raise an error...
}%

\UDcollectverbarg{%
  \UDcollectverbarg{\AtEndDocument}%
}{\expandafter\scantokenssetvbox\expandafter{\the\MYverbtoks}{\MYverbbox}}_\begin{javascript}
__\end{javascript}_%
\appendtoverbtoks?
var debugInfo=false;

/*
HOW THIS SHOULD WORK:
    Initially check to make sure the submitted answer (and proposed answer) are at least in some kind of theoretically factored form.
    
    Next duplicate the raw trees so we can mess with them without worrying about changing the original.
    
    Fold up any exponents on both trees so that our root node is of the form: ['*',factor1,factor2,factor3,...]. 
        This includes killing off any leading negative signs (again we'll compare for equality using the original raw string)
    
    Now call a recursive function to deep-dive into each factor to find what degree that factor actually is.
        Will also identify if the factor isn't even a factor, in which case we will return a negative value to negate the answer.
        
    Once we have the degree for each factor, now we can compare the instructor degree list and student degree list as intended.
*/


// Subfunction to identify if something is a number:

function isNum(numb) {
    if ((typeof numb === 'number')||(numb=='e')||(numb=='pi'))
    {return true} else {return false}
}

// Subfunction to identify if something is a non-negative integer:

function isPosInt(numb) {
    if ((isNum(numb))&&((numb>=0)&&(numb%1==0)))
    {return true} else {return false}
}


// This does a recursion through a factor to eventually find it's degree - assuming it's a polynomial.

function degreeHunt(tree,position,curDeg) {
    //(Re)set curDeg just in case:
    var curDeg=0;
    
    // First, let's figure out what to do about negative signs, since they can be annoying.
    //  I think there's three possibilities, it's a negative array, a negative x, or a negative number.
    
    if (tree[position][0]=='-') {
        debugText('Processing a minus sign.');
        if (tree[position][1]=='x') {
            // We found a ``-x'' term within our factor, so that's degree 1 I guess!
            
            debugText('Found a -x term!');
            curDeg = Math.max(curDeg,1);
            
        } else if (Array.isArray(tree[position][1])) {
            // Else if there's a negative outside of an array, just bypass the negative and keep digging for an exponent sign.
            
            debugText('Found a negative Array term!');
            let tempVal = degreeHunt(tree[position],1)
            if (tempVal<0) { return (-1)} else {
                curDeg = Math.max(curDeg,tempVal);
            }
            
            
        }// Note that, if it's a negative number, I don't care about it, so no need for an ``else''.
    } else if (tree[position][0]=='apply') {
        debugText('Processing an apply symbol.');
        // any 'apply' is inevitably a function that isn't a polynomial.
        //  Although, only if it actually has an 'x' in there - so I need to fix/update that at some point.
        
        // To find out if the apply is actually just a number or not, we need to recurs through every sub-node looking for an 'x'.
        //  If the degree ends up positive, then we have a variable inside the apply function it's not a polynomial.
        //  If the degree ends up zero, then it's ultimately some bizarre number formation and we're fine.
        
        var tempDeg = 0;
        
        for (var j = 1; j < tree[position].length; ++j) {
            // Walk the array to find any powers of x.
            if (tree[position][j]=='x') {
                // If the entry is just x, then we have pos deg and we are done.
                debugText('Found an x inside an apply function that suggests the factor is not actually a polynomial!');
                return (-1);
            } else if (Array.isArray(tree[position][j])) {
                // If the term is an array, then we need to recurs to find the degree.
                let tempVal=degreeHunt(tree[position],j,0);
                if (tempVal!==0) {
                    debugText('Found an issue inside an apply function that suggests the factor is not actually a polynomial!');
                    return (-1)
                }
            }// Any other option is degree 0, so no need for an else.
        }// End of for loop and end up 'apply' function
    } else if (tree[position][0]=='^') {
        // Now we process the exponent sign case, but be careful cause students do crazy shit,
        //  So it might be a x^N situation, but it might be some other shenanigans.
        
        debugText('Processing an exponential sign.');
        
        if ((tree[position][1]=='x')&&(isPosInt(tree[position][2]))) {
            // We have something like x^N
            
            debugText('We have x^N');
            
            curDeg = Math.max(curDeg,tree[position][2]);
            
        } else if (tree[position][1]=='x') {
            // If the base is x but it's not being raised to an integer power, then it's not a proper monomial.
            //  Note that we are assuming here that students won't put shit like ``1+1'' as the power, if so, it will be marked wrong.
            
            debugText('We have x^(g(x)) but g(x) is not a positive integer.');
            
            return -1;
            
        } else if ((isNum(tree[position][1]))&&(isNum(tree[position][2]))) {
            // This means we have a^b which is still just a number, so it's fine... but doesn't give a degree.
            
            debugText('We have a^b');
            
        } else if ((Array.isArray(tree[position][1]))&&(isPosInt(tree[position][2]))) {
            // We have something of the form (f(x))^N which might be a part of a factor.
            //  We recurse on the array, and multiply the result by the N.
            
            debugText('We have f(x)^N');
            let tempVal = tree[position][2]*degreeHunt(tree[position],1,curDeg);
            if (tempVal<0) { return (-1)} else {
                curDeg = Math.max(curDeg,tempVal);
            }

            
        } else {
            // All other situations are inevitably not polynomials.
            
            return (-1);
        }
    } else if (tree[position][0]=='/') {
        // This is problematic, because the only way we can allow a division is if the denominator is just a number.
        //  But if the denominator has an 'x' anywhere in it, then we have a problem...
        
        debugText('Processing a division sign.');
        
        if (tree[position][2]=='x') {
            // We're dividing by x, which is bad.
            
            debugText('Dividing by x, naughty naughty!');
            
            return (-1);
        } else if (Array.isArray(tree[position][2])) {
            //  Let's try doing a recurs, and if we get a result that isn't 0, then that means there's an 'x' so we return -1.
            if (degreeHunt(tree[position],2,0)!==0) {
                // If it's not 0, then we found an 'x' or something that invalidates the polynomial.
                return (-1)
            } // otherwise we have some kind of 'dividing by a number' situation, so it's fine, and doesn't impact degree.
            
        }
        
        // Now that we've dealt with the possibilities that cause a non-polynomial function, we can proceed assuming it is just division by a number.
        
        debugText('The denominator is just a number, so we need to check the numerator.');
        
        if (tree[position][1]=='x') {
            // If we've made it past the first two hurtles, then the bottom is just a number of some form.
            //  So if the top is 'x', then we have something like ``x/a'' which is still a degree 1 factor.
            var curDeg = Math.max(curDeg,1);
            debugText('We found a fraction with just x in the top.');
        } else if (Array.isArray(tree[position][1])) {
            // If the top of the fraction is an array - but we've already reduced to case where denominator is a number.
            //  So we need to figure out if there is a degree in the top to count.
            
            debugText('We found a fraction with an array for the numerator.')
            var tempVal = degreeHunt(tree[position],1,0)
            debugText('I think the numerator degree from the array is: '+tempVal);
            if (tempVal<0) { return (-1)} else {
                 var curDeg = Math.max(curDeg,tempVal);
            }
            
        }// Note that if none of the above, then it's just a/b, which is fine and doesn't impact degree.

    } else if (tree[position][0]=='*') {
        // If we are multiplying, we could be multiplying a bunch of terms - maybe a bunch of x terms.
        
        debugText('Processing a product sign.');
        
        var tempDeg = 0;
        for (var j = 0; j < tree[position].length; ++j) {
            // Add the degree of each thing being multiplied, even though most are probably zero.
            if (tree[position][j]=='x') {
                // If the entry is just x, then we have deg 1.
                debugText('Found a solo x term inside the product sign, degree is at least 1.')
                tempDeg = Math.max(tempDeg,1);
            } else if (Array.isArray(tree[position][j])) {
                // If the term is an array, then we need to recurs to find the degree.
                // let tempVal=degreeHunt(tree[position],j,0);
                if (degreeHunt(tree[position],j,0)<0) { 
                    return (-1)
                } else if (degreeHunt(tree[position],j,0)==0){
                    debugText('No degree term found inside product sign, so we stay at degree: '+tempDeg);
                } else {
                    debugText('found a higher degree term inside the product sign, degree is at least: '+tempDeg);
                    tempDeg = Math.max(tempDeg,degreeHunt(tree[position],j,0));
                }
            }// Any other option is degree 0, so no need for an else.
        }
        // Once the for loop finishes, we convert the temp deg to the current deg.
        var curDeg = Math.max(curDeg,tempDeg);
        debugText('Inside the product sign we found the degree is: '+tempDeg);
        debugText('So we set the curDeg to: '+curDeg);
    } else {
        // If it's none of the above, then we should just recurs on any arrays we find.
        
        debugText('Processing an unknown sign... Specifically: '+tree[position][0]);
        
        for (var j = 0; j < tree[position].length; ++j) {
            // Walk the array to find any powers of x.
            if (tree[position][j]=='x') {
                // If the entry is just x, then we have deg 1.
                curDeg = Math.max(curDeg,1);
            } else if (Array.isArray(tree[position][j])) {
                // If the term is an array, then we need to recurs to find the degree.
                let tempVal=degreeHunt(tree[position],j,0);
                if (tempVal<0) { return (-1)} else {
                    curDeg = Math.max(curDeg,tempVal);
                }
            }// Any other option is degree 0, so no need for an else.
        }
        
    }
    debugText('Made it to the end of the degreeHunt function, which means we need to return a curDeg variable, which is: '+curDeg);
    return curDeg
}

// Subfunction just to make sure that the submitted function is in a legitimately factored form.

function JNFisFactored(factorTree) {
   
    // First we check to see if we have a negative factored out, which messes everything up in the tree.
    if ((factorTree[0]=='-')||(factorTree[0]=='*')||((factorTree[0]=='/')&&(isNum(JNFoperation[1])))
    ) {return true} else {return false}
}

// Subfunction to make debug easier.

function debugText(text) {
    if (debugInfo) {
    console.log('DEBUG INFO::' + text)
    }
}

function debugArray(array) {
    if (debugInfo) {
    console.log(array);
    }
}

function factorCheck(f,g) {
    // This validator is designed to check that a student is submitting a factored polynomial. It works by:
    //  Checking that the degree of each factor matches between student submitted and instructor submitted answers (NOT order sensitive),
    //  Checking that the submitted answer and the expected answer are the same via real Ximera evaluation,
    //  Checking that the outer most (last to be computed when following order of operations) operation is multiplication.
    //  It ignores degree 0 terms for degree check, and now can ignore factored out negative signs.
    
    
    debugText(f.tree);
    debugText(g.tree);

    if (JNFisFactored(g.tree)==false) {
        console.log('Answer rejected, instructor answer not in a factored form. Bad instructor, no donut.');
        return false
    }
    
    // First we check to make sure it is in *a* factored form:
    if (JNFisFactored(f.tree)==true) {
        console.log('The student answer is at least in *a* factored form.');
        
        
    } else {
        console.log('Answer rejected, student answer not in a factored form.');
        return false
    }
    
    // Let's duplicate the trees to manipulate, so we keep the original correctly.
    var studentAns=f.tree
    var instructorAns=g.tree
    
    // Also, if there is a factored out negative, let's just kill that, since we aren't doing a funtion comparison on this part:
    while (studentAns[0]=='-'){studentAns = studentAns[1];};
    while (instructorAns[0]=='-'){instructorAns = instructorAns[1];};
    
    // Now we want to fold up any root-level exponents into duplicate children of the master tree,
    //  This lets us assume the top-level node has 1 child per factor.
    debugText('folding up external exponents of studentTree  so factors do not have exponents');
    for (var i = 0; i < studentAns.length; ++i) {
        if ((studentAns[i][0] == '^')&&(isPosInt(studentAns[i][2]))) {
            studentAns=studentAns.concat(Array(studentAns[i][2]).fill(studentAns[i][1]));
            studentAns.splice(i,1);// This should theoretically remove the original term now that we've duplicated it.
            i=i-1;// since we shortened our array by 1, we should move the iteration value down 1 too.
            debugText('Ok, I folded up a term, so hopefully our student vector still makes sense. It is now: '+studentAns);
        } else if (studentAns[i][0] == '^') {
            //if we have a power, but not a positive integer power, then we have a non-polynomial factor, so we're done.
            console.log('I think I found a non-polynomial term, specifically some root term has a non natural number (or zero) power.');
            return false;
        }
    }
    debugText('After all preprocessing my studentAns vector is:');
    debugText(studentAns);

    // Now re repeat with instructor tree:
    debugText('folding up external exponents of instructorTree so factors do not have exponents')
    for (var i = 0; i < instructorAns.length; ++i) {
        if ((instructorAns[i][0] == '^')&&(isPosInt(instructorAns[i][2]))) {
            instructorAns=instructorAns.concat(Array(instructorAns[i][2]).fill(instructorAns[i][1]));
            instructorAns.splice(i,1);// This should theoretically remove the original term now that we've duplicated it.
            i=i-1;// since we shortened our array by 1, we should move the iteration value down 1 too.
            debugText('Ok, I folded up a term, so hopefully our student vector still makes sense. It is now: '+instructorAns);
        } else if (instructorAns[i][0] == '^') {
            //if we have a power, but not a positive integer power, then we have a non-polynomial factor, so we're done.
            return false;
            console.log('Found a non-polynomial term in the instructor answer... huh... Check the code!');
        }
    }
    debugText('After all preprocessing my instructorAns vector is: '+instructorAns);
    
    /*
        ::NOW LETS PROCESS THE STUDENT ANSWER::
    */
    
    var studentDegList=[0]
    for (var i = 0; i < studentAns.length; ++i) {
        if (studentAns[i] == 'x') {
            // If the factor is simply 'x', then it's a degree 1 factor... yay...
            studentDegList.push(1);
            debugText('Found another factors degree, so now studentDegList is: '+studentDegList); 
        } else if (Array.isArray(studentAns[i])) {
            // Otherwise, if it is an array, we have something to go hunting in.
            studentDegList.push(degreeHunt(studentAns,i,0));
            debugText('Found another factors degree, so now studentDegList is: '+studentDegList); 
        }// Note that the only other possibility is it being a number, which we don't care about.
    }
    studentDegList = studentDegList.filter(x => x!==0);// Remove all zeros from the array to avoid stupid padded constant multipliers.
    studentDegList.sort();// Sort the result so that we can later compare it to the instructor version.
    debugText('The final List of Factor Degrees given by the student is: ' + studentDegList);
    
    if (studentDegList.some(elem => elem<0)) {
        console.log('I think one of the student factors is NOT a polynomial. So I am rejecting the answer.');
        return false
        }
    
    
    /*
        ::NOW LETS PROCESS THE INSTRUCTOR ANSWER::
    */
    
    var instructorDegList=[0]
    for (var i = 0; i < instructorAns.length; ++i) {
        if (instructorAns[i] == 'x') {
            // If the factor is simply 'x', then it's a degree 1 factor... yay...
            instructorDegList.push(1);
            debugText('Found another factors degree, so now instructorDegList is: '+instructorDegList); 
        } else if (Array.isArray(instructorAns[i])) {
            // Otherwise, if it is an array, we have something to go hunting in.
            instructorDegList.push(degreeHunt(instructorAns,i,0));
            debugText('Found another factors degree, so now instructorDegList is: '+instructorDegList); 
        }// Note that the only other possibility is it being a number, which we don't care about.
    }
    instructorDegList = instructorDegList.filter(x => x!==0);// Remove all zeros from the array to avoid stupid padded constant multipliers.
    instructorDegList.sort();// Sort the result so that we can later compare it to the instructor version.
    debugText('The final List of Factor Degrees given by the instructor is: ' + instructorDegList);
    
    if (instructorDegList.some(elem => elem<0)) {
        console.log('I think one of the instructor factors is NOT a polynomial. So I am rejecting the answer.');
        return false
        }
    
   /*
        ::NOW WE COMPARE::
   */
    
    if (studentDegList.length!=instructorDegList.length){
        console.log('Ans Rejected: Wrong number of factors.');
        return false;
        }
    
    for (var i = 0; i < studentDegList.length; ++i) {
        if (studentDegList[i] !== instructorDegList[i]) {
        console.log('Ans Rejected: At least one factor is the wrong degree.');
        return false;
        }
    }

    if (f.equals(g)){
        } else {
        console.log('Ans Rejected: Factors do not expand to original Polynomial.');
        }

    return (f.equals(g));
}

function compSubTree(f, treeOne,treeTwo) {
    /* 
        We assume that the input f is the full math expressions object we will need for cloning,
        treeOne is a level of the tree that is used as the subtree to compare
        treeTwo is a level of another tree that is used as the subtree to compare.
        We want to see if f.treeOne = f.treeTwo in the Ximera sense of equality.
    */
    // First we duplicate the object structure so the equals command can be used without pitching a fit.
    let fTemp=Object.create(f);
    let gTemp=Object.create(f);
    
    // Now we assign the subtrees as the full tree of the objects.
    fTemp.tree = treeOne;
    gTemp.tree = treeTwo;
    
    // Now we return the validity of their equality using Ximera equality validation.
    return (fTemp.equals(gTemp))
}

?%
%\makeatother

%\begin{document}
%Some \appendtoverbtoks|This is appended 1.| text. Some text. Some \prependtoverbtoks|This is prepended 1.| text.
%Some \appendtoverbtoks*This is appended 2.* text. Some text. Some \prependtoverbtoks{This is prepended 2.} text.
%\end{document}




%% 
%% Copyright (C) 2024 by Bart Snapp <bart.snapp@gmail.com> and Jim Fowler <kisonecat@gmail.com>
%% 
%% This work may be distributed and/or modified under the
%% conditions of the LaTeX Project Public License (LPPL), either
%% version 1.3c of this license or (at your option) any later
%% version.  The latest version of this license is in the file:
%% 
%% http://www.latex-project.org/lppl.txt
%% 
%% This work is "maintained" (as per LPPL maintenance status) by
%% Bart Snapp. The source code can be found at:
%% 
%% https://github.com/XimeraProject/ximeraLatex
%% 
%% This work consists of the file ximera.dtx, the associated files
%% under src/, and a Makefile.
%% 
%% Running "make" generates the derived files README, ximera.pdf, ximera.cls, xourse.cls.
%% 
%% Running "make inst" installs the files in the user's TeX tree.
%% 
%% Running "make install" installs the files in the local TeX tree.
%% 
%%
%% End of file `ximera.cls'.
