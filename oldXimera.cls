% A LaTeX2e document class for preparing handouts and online material
% via Ximera.

%% ximera.cls
%% Copyright (c) 2013 Bart Snapp, Jim Fowler, and Jason Nowell
%
% This work may be distributed and/or modified under the
% conditions of the LaTeX Project Public License, either version 1.3c
% of this license or (at your option) any later version.
% The latest version of this license is in
%   http://www.latex-project.org/lppl.txt
% and version 1.3c or later is part of all distributions of LaTeX
% version 2013/12/01 or later.
%
% This work has the LPPL maintenance status "author-maintained".
% 
% This work consists of the files ximera.cls

%%% Bart Snapp
%%% Department of Mathematics
%%% The Ohio State University

%%% Jason Nowell
%%% Department of Mathematics
%%% University of Florida

\def\fileversion{1.4}
\def\filedate{2018/03/01}

% ---------------------------------------------------------------------
% ---------------------------------------------------------------------
% If there's some feature that you'd like that this file doesn't
% provide, tell Bart Snapp, Jim Fowler or Jason Nowell about it.  
%
% Thanks to: 
%
% Philip Hirschhorn as this document class is based on his
% ``exam.cls.'' The original work can be found at:
% http://www-math.mit.edu/~psh/
% --------------------------------------------------------------------
% --------------------------------------------------------------------


% --------------------------------------------------------------------
% --------------------------------------------------------------------
% Class Options
% --------------------------------------------------------------------
% --------------------------------------------------------------------

\NeedsTeXFormat{LaTeX2e}

\ProvidesClass{ximera}[\filedate\space Version \fileversion\space by
  Bart Snapp]

\newif\ifhandout
\handoutfalse
\DeclareOption{handout}{\handouttrue}

\newif\ifinstructornotes
\instructornotesfalse
\DeclareOption{instructornotes}{\instructornotestrue}   %% Yes. 
\DeclareOption{noinstructornotes}{\instructornotestrue} %% This is hilarious.

\newif\ifnooutcomes
\nooutcomesfalse
\DeclareOption{nooutcomes}{\nooutcomestrue}

\newif\ifnoauthor
\noauthorfalse
\DeclareOption{noauthor}{\noauthortrue}

\newif\ifhints
\hintsfalse
\DeclareOption{hints}{\hintstrue}


\newif\ifnewpage
\newpagefalse
\DeclareOption{newpage}{\newpagetrue}

\newif\ifnumbers
\numbersfalse
\DeclareOption{numbers}{\numberstrue}

\newif\ifwordchoicegiven%% suppresses choices in handout mode
\wordchoicegivenfalse
\DeclareOption{wordchoicegiven}{\wordchoicegiventrue}

% "space" option depreciated. Keeping code in, in case of conflicts. Should remove this code if no conflicts are detected/reported by 6/28/2018 -- Jason Nowell
%\newif\ifspace 
%\spacefalse
%\DeclareOption{space}{\spacetrue}

%% This basically works for exercises, though page breaks are weird. 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% tikzexport will generate SVGs and PNGs for tikzpictures
%
% This option is automatically chosen when compiling inside the Ximera sandbox
%
\newif\iftikzexport
\tikzexportfalse
\DeclareOption{tikzexport}{%
  \tikzexporttrue%
  \handoutfalse%
  \numbersfalse%
  \newpagefalse%
  \hintsfalse%
  \nooutcomesfalse%
}

\newif\ifxake
\xakefalse
\DeclareOption{xake}{\xaketrue}

\DeclareOption*{%
  \PassOptionsToClass{\CurrentOption}{article}%
}
\ProcessOptions\relax
\LoadClass{article}



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%						Required Packages
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This is a list of required packages. As packages are added to this list, we should include a comment
% as to where they are being utilized. This will help keep this list from being redundant and/or
% outdated. -- Jason Nowell


\RequirePackage[makeroom]{cancel} % for \cancel; loaded here to avoid polluting the .jax output
\RequirePackage[inline]{enumitem}
\RequirePackage[pagestyles]{titlesec} 
\RequirePackage{titletoc} 
\RequirePackage{titling}  
\RequirePackage{url}
\RequirePackage{xcolor}
\RequirePackage{pgfplots}
\usepgfplotslibrary{groupplots}
\usetikzlibrary{calc}
\RequirePackage{fancyvrb}
\RequirePackage{forloop}% Used for (at least) the problem environment dynamic naming and building.

%% mdframed is used to make pretty foldable, but the amsthm/mdframed
%% conflict also messes up the .jax file so don't load mdframed when
%% performing the xake step
\iftikzexport\else\RequirePackage[framemethod=TikZ]{mdframed}\fi

\RequirePackage{environ}% Included to allow saving of environment contents. This does *not* play well with various verbatem environments however and would be great to find a way around utilizing this package. One option is the "Suppress" command included below.
\RequirePackage{amssymb}% Included to have access to math typeset.
\RequirePackage{amsmath}% Included to have access to math typeset.
\RequirePackage{amsthm}%  Included to have access to math typeset.
\RequirePackage{xifthen}% http://ctan.org/pkg/xifthen
\RequirePackage{multido}% http://ctan.org/pkg/multido
\RequirePackage{listings} %% is this required???

\RequirePackage{xkeyval}

\RequirePackage{comment}

\oddsidemargin 62pt %% so we don't get weird margins in 2-sided mode
\evensidemargin 62pt %% so we don't get weird margins in 2-sided mode
\textwidth 345pt %% so we don't get weird margins in 2-sided mode
\headheight 14pt %% so we don't get weird margins in 2-sided mode



\makeatletter
% Don't use hyperref when using Tex4ht
\ifdefined\HCode
\RequirePackage{hyperref}
%\newcommand{\href}[1]{}
\else
\RequirePackage[pdfpagelabels,colorlinks=true,allcolors=blue!30!black]{hyperref}
\pdfstringdefDisableCommands{\def\hskip{}}%% quiets warning
\fi
% We put hyperref after all other packages becuase that is better.

% the article document class is made to have lines of text that are
% not too long. 

% http://tex.stackexchange.com/questions/71172/why-are-default-latex-margins-so-big


% ---------------------------------------------------------------------
% ---------------------------------------------------------------------
% 				Custom Intrapackage Commands
% ---------------------------------------------------------------------
% ---------------------------------------------------------------------

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%  		Suppress Command  		%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  
% The suppress command is a good way to suppress output without commenting it.
% This way we can avoid many of the places we use environ package and this should also
% avoid most of the verbatim conflicts.
%
% Jason Nowell -- Code adapted from syntonly.sty


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


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%  	Make Counter Command  		%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Improved version of \newcounter to check for existance before creating a counter to minimize conflicts with packages.
%
% Added by Jason Nowell

\newcommand{\Make@Counter}[1]{
	\@ifundefined{c@#1}	% Check to see if counter exists
		{									%If not, create it and set it to 0.
		\newcounter{#1}
		\setcounter{#1}{0}
		}
		{									%If so, reset to 0.
		\setcounter{#1}{0}
		}
}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%						Tikz Fix.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Currently Tikz doesn't compile natively into the website because of how the xake bake compilation works.
% ::IMPORTANT NOTE FOR TIKZ::
% In order to make Tikz work, you need to get the tool "mutool" on the machine that is performing xake bake.

\PassOptionsToPackage{table}{xcolor}
\RequirePackage{tikz}

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
        pdflatex \tikzexternalcheckshellescape -halt-on-error -interaction=batchmode -jobname "\image" "\\PassOptionsToClass{tikzexport}{ximera}\texsource";
        mutool draw -o \image.svg \image.pdf ;
        mutool draw -r 150 -c rgbalpha -o \image.png \image.pdf ;
      }
    }
    \tikzexternalize[optimize=false,prefix=./]
  \fi

  \fi

% Load sagetex if it exists
\IfFileExists{sagetex.sty}{\RequirePackage{sagetex}}{}


% ---------------------------------------------------------------------
% ---------------------------------------------------------------------
% Exercise Headers
% ---------------------------------------------------------------------
% ---------------------------------------------------------------------

\newcommand{\license}{\excludecomment}
\newcommand{\acknowledgement}{\excludecomment}
\renewcommand{\tag}{\excludecomment}
             
% ---------------------------------------------------------------------
% ---------------------------------------------------------------------
% Print Formatting
% ---------------------------------------------------------------------
% ---------------------------------------------------------------------

\setlength{\parindent}{0pt}
\setlength{\parskip}{5pt}


\let\title\relax
\newcommand{\title}[1][]{{\protected@xdef\@pretitle{#1}}\protected@xdef\@title}


\title{}


\let\abstract\relax
\let\endabstract\relax
\NewEnviron{abstract}{\protected@xdef\theabstract{\BODY}}% Use of environ package, may want to find a better way.


\newcounter{titlenumber}
\renewcommand{\thetitlenumber}{\arabic{titlenumber}}
%\renewcommand{\thesection}{\arabic{titlenumber}} %% Makes section numbers work
\setcounter{titlenumber}{0}



\newpagestyle{main}{
\sethead[\textsl{\ifnumbers\thetitlenumber\quad\fi\@title}][][] % even
{}{}{\textsl{\ifnumbers\thetitlenumber\quad\fi\@title}} % odd
\setfoot[\thepage][][] % even
{}{}{\thepage} % odd
}
\pagestyle{main}


% Redefine maketitle and put them in a table of contents.  The \phantomsection is to fix the hrefs.

\let\@emptyauthor\@author
\def\author#1{\gdef\@author{#1}}
\def\@author{\@latex@warning@no@line{No \noexpand\author given}}
\def\and{ and } 

\renewcommand\maketitle{%
  \addtocounter{titlenumber}{1}\phantomsection \ifnumbers\addcontentsline{toc}{section}{\thetitlenumber~\@title}\else\addcontentsline{toc}{section}{\@title}\fi %% puts titles in the toc, with or without numbers.
  {\flushleft\large\bfseries \@pretitle\par\vspace{-1em}}
  {\flushleft\LARGE\bfseries {\ifnumbers\thetitlenumber\fi}{\ifnumbers\hspace{1em}\else\hspace{0em}\fi}\@title \par }
  \vskip .6em\noindent\textit\theabstract\setcounter{problem}{0}\setcounter{section}{0}\setcounter{subsection}{0}\par\vspace{2em}
  \ifnooutcomes\else\let\thefootnote\relax\footnote{Learning outcomes: \theoutcomes}\fi
  \ifnoauthor\else\let\thefootnote\relax\footnote{Author(s):~\@author}\fi
  \aftergroup\@afterindentfalse
  \aftergroup\@afterheading}

\relax

\ifnumbers
\setcounter{secnumdepth}{2}
\renewcommand{\thesection}{\arabic{titlenumber}.\arabic{section}~} 
\else
\setcounter{secnumdepth}{-2}

\fi
\relax


\def\activitystyle{}
\newcounter{sectiontitlenumber}
\setcounter{secnumdepth}{0}
\newcommand\chapterstyle{%
  \def\activitystyle{activity-chapter}
  \def\maketitle{%
    \addtocounter{titlenumber}{1}%
    \addcontentsline{toc}{section}{\textbf{\thetitlenumber\quad\@title}}%
                    {\flushleft\small\sffamily\bfseries\@pretitle\par\vspace{-1.5em}}%
                    {\flushleft\LARGE\sffamily\bfseries\thetitlenumber\quad\@title \par }%
                    {\vskip .6em\noindent\textit\theabstract\setcounter{problem}{0}\setcounter{sectiontitlenumber}{0}}%
                    \par\vspace{2em}}}

\newcommand\sectionstyle{%
  \def\activitystyle{activity-section}
  \def\maketitle{%
    \addtocounter{sectiontitlenumber}{1}
    \addcontentsline{toc}{subsection}{\thetitlenumber.\thesectiontitlenumber\quad\@title}%
    {\flushleft\small\sffamily\bfseries\@pretitle\par\vspace{-1.5em}}%
    {\flushleft\Large\sffamily\bfseries\thetitlenumber.\thesectiontitlenumber\quad\@title \par}%
    {\vskip .6em\noindent\textit\theabstract}%
    \par\vspace{2em}}}


\iftikzexport%% allows xake to handle \chapterstyle and \sectionstye
\renewcommand\chapterstyle{\def\activitystyle{chapter}}
\renewcommand\sectionstyle{\def\activitystyle{section}}
\else
\fi


%% makes everymath display style even when inline, could be optional
\everymath{\displaystyle}
%% Ok not everything, we also need this
\let\prelim\lim 
\renewcommand{\lim}{\displaystyle\prelim} 
%% for display style limits




% --------------------------------------------------------------------
% --------------------------------------------------------------------
% Content
% --------------------------------------------------------------------
% --------------------------------------------------------------------

\def\theoutcomes{}

\ifdefined\HCode%
  \newcommand{\outcome}[1]{}
\else%
  \newwrite\outcomefile
  \immediate\openout\outcomefile=\jobname.oc

  \newcommand{\outcome}[1]{\edef\theoutcomes{\theoutcomes #1~}%
  \immediate\write\outcomefile{\unexpanded{\outcome}{#1}}}
\fi%


% Youtube link
\newcommand{\youtube}[1]{YouTube link: \url{https://www.youtube.com/watch?v=#1}}

% --------------------------------------------------------------------
% Interactives

\DefineVerbatimEnvironment{javascript}{Verbatim}{numbers=left,frame=lines,label=JavaScript,labelposition=topline}
\def\js#1{\mbox{\texttt{\detokenize{#1}}}}

%Geogebra link
\newcommand{\geogebra}[3]{Geogebra link: \url{https://tube.geogebra.org/m/#1}}

%Desmos link
\newcommand{\desmos}[3]{Desmos link: \url{https://www.desmos.com/calculator/#1}}

% Google Spreadsheet link (read only)
\newcommand{\googleSheet}[5]{%
  Google Spreadsheet link: \url{https://docs.google.com/spreadsheets/d/#1}%
}

% Embedded graph (in math mode)
\newcommand{\graph}[2][]{\text{Graph of $#2$}}

% Interactive element

\define@key{interactive}{id}{\def\interactive@id{#1}}
\setkeys{interactive}{id=}
\newcommand{\includeinteractive}[2][]{
\setkeys*{interactive}{#1}%
\ifthenelse{\equal{\interactive@id}{}}{}{\recordvariable{\interactive@id}}
Interactive
}


%HTML 

%\newenvironment{html}{\textbf{INTERACTIVE NOT AVAILABLE}\suppress}{\endsuppress}% New version using suppress method

% Old code. Delete by 6/28/2018 if no errors reported.
\NewEnviron{html}{\textbf{INTERACTIVE NOT AVAILABLE.}} 

	
% --------------------------------------------------------------------
% Hyperlinks to outside documents

\newcommand*{\link}[2][]{#1%
	\ifthenelse{\equal{#1}{}}%
		{\footnote{Link: \url{#2}}}%
		{\footnote{See #1 at \url{#2}}}
	}
                              

%% image environment
%\newenvironment{image}[1][]{\begin{center}}{\end{center}}
\NewEnviron{image}[1][3in]{% 
  \begin{center}\resizebox{#1}{!}{\BODY}\end{center}% resize and center
}

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
\newtheorem{formula}{Formula}
\newtheorem{idea}{Idea}
\newtheorem{lemma}{Lemma}
\newtheorem{model}{Model}
\newtheorem{notation}{Notation}
\newtheorem{observation}{Observation}
\newtheorem{paradox}{Paradox}
\newtheorem{procedure}{Procedure}
\newtheorem{proposition}{Proposition}
\newtheorem{remark}{Remark}
\newtheorem{summary}{Summary}
\newtheorem{template}{Template}
\newtheorem{warning}{Warning}



\renewenvironment{proof}[1][\proofname]{\begin{trivlist}\item[\hskip \labelsep \itshape \bfseries #1{}\hspace{2ex}]}
{\qed\end{trivlist}}

\renewcommand{\qedsymbol}{$\blacksquare$}

% ---------------------------------------------------------------------
% ---------------------------------------------------------------------
% Problems
% ---------------------------------------------------------------------
% ---------------------------------------------------------------------

%% Prompt environment
\ifhandout
	\NewEnviron{prompt}{}
%	\newenvironment{prompt}{\suppress}{\endsuppress} -- Currently breaks when put in mathemode
\else
	\newenvironment{prompt}
    	{\bgroup\color{gray!50!black}}
        {\egroup}
\fi

%% onlineOnly environment
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
 

% ---------------------------------------------------------------------
% ---------------------------------------------------------------------
% Answers
% ---------------------------------------------------------------------
% ---------------------------------------------------------------------


\ifdefined\HCode
	\newcommand{\recordvariable}[1]{}
\else
	\newwrite\idfile
	\immediate\openout\idfile=\jobname.ids
	\newcommand{\recordvariable}[1]{\ifthenelse{\equal{#1}{}}{}{\immediate\write\idfile{var #1;}}}
\fi

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 	Defining keys for answer command key=value pairs
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\define@key{answer}{given}[true]{\def\ans@given{#1}}% Determines if answer is shown in handout mode.
	%% when given=true, show answer in handout mode, show answer in "given box" outside handout mode
	%% when given=false, do not show answer in handout mode, show answer outside handout mode

\define@key{answer}{tolerance}{\def\ans@tol{#1}}% Used for setting numeric answer tolerance for online student input. 
												 %  Note: currently no affect on pdf output.

\define@key{answer}{validator}{}%			Used to run dynamic js code on student provided answers.
												 %  Note: currently pdf outputs the validator code itself.

\define@key{answer}{id}{\def\ans@id{#1}} %	Used for assigning a js ID to answer for dynamic code (eg validators).
												 %  Note: currently no affect on pdf output.

\define@key{answer}{format}{}%				Used to set anticipated input format; eg "string".
												 %  Note: currently no affect on pdf output.



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%			Set default values for answer command key=value pairs
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Default values are:
%
% given = false

\setkeys{answer}{id=,given=false}




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%						Answer command base code.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


\newcommand{\answer}[2][]{%
	\ifmmode%
		\setkeys{answer}{#1}%
		\recordvariable{\ans@id}
		\ifthenelse{\boolean{\ans@given}}
			{% Start then statement
			\ifhandout 
				#2
			\else
				\underset{\scriptstyle\mathrm{given}}{\fbox{\ensuremath{#2}}}
			\fi
			}% End then statement
			{% Start else statement
			\ifhandout
				\fbox{\rm{?}}
			\else% show answer in box outside handout mode
				\fbox{\ensuremath{#2}}
			\fi
			}% End else statement
	\else%
		\GenericError{\space\space\space\space}% Throw an error based on... something? -- Jason
		{Attempt to use \@backslashchar answer outside of math mode}
		{See https://github.com/ximeraProject/ximeraLatex for explanation.}
		{Need to use either inline or display math.}%
	\fi
}
             


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%				Answer: Multiple Choice Environment.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% Jim: Originally this was \renewcommand{\theenumi}{$(\mathrm{\alph{enumi}})$}
% but that breaks tex4ht because mathmode can only be processed by mathjax.
% so now I made this just italicized.

%% Not sure what role these renewed commands make? -- Jason
\renewcommand{\theenumi}{\textup{(\alph{enumi})}}
\renewcommand{\labelenumi}{\theenumi}
\renewcommand{\theenumii}{\textup{(\roman{enumii})}}
\renewcommand{\labelenumii}{\theenumii}

%% correct answers for multiple choice


%%%%  key=value pairs for multiple choice options.

\define@key{choice}{value}[]{\def\choice@value{#1}}
\define@boolkey{choice}{correct}[true]{\def\choice@correct{#1}}% This flags the answer as the correct answer

\define@key{multipleChoice}{id}{\def\mc@id{#1}}

%% \otherchoice outputs the item if correct and nothing if incorrect.
\define@key{otherchoice}{value}[]{\def\otherchoice@value{#1}}
\define@boolkey{otherchoice}{correct}[true]{\def\otherchoice@correct{#1}}

%%%%  Default key choices for multiple choice options.
%
% Default for choice pairs
\setkeys{choice}{correct=false,value=}% Default: answers without the option "correct=true" is "incorrect".

% Defaults for multipleChoice pairs.
\setkeys{multipleChoice}{id=}% Default to no id? -- Jason

% Defaults for otherchoice pairs.
\setkeys{otherchoice}{correct=false,value=}% Default "otherchoice" to behave like "choice" for error checking.

%%%%  Default choice command.
%
% choice command denotes a possible answer choice for the multiple choice question.

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

%Define an expandable version of choice Not really meant to be used outside this package (used in wordChoice)
% Is there a reason we can't just always use this as default? -- Jason
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
	\ignorespaces
	\setkeys{otherchoice}{#1}
	\ifthenelse{\boolean{\otherchoice@correct}}
		{% Start then result
		#2\setkeys{otherchoice}{correct=false}
		}% End then result
		{}% Start/End else result
	\ignorespaces
	}

%%%%  multipleChoice Environment(s).
%
% multipleChoice* and multipleChoice** are for internal use only. 
%% Given normal LaTeX syntax, we may want to change this to something more like 
%% multipleChoice@ and multipleChoice@@. -- Jason


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

%multipleChoice* is for internal use only! (used in wordChoice)
%It displays all choices in a list separated by /
\newenvironment{multipleChoice*}[1][]
	{% Environment Start Code
	\setkeys{multipleChoice}{#1}%
	\ifthenelse{\equal{\mc@id}{}}% Test if \mc@ID is empty. This is not a robust check, is this dangerous? -- Jason
		{}% Begin/End then result.
		{% Begin else result
		\immediate\write\idfile{var \mc@id;}
		}% End else result
	\begin{enumerate*}[before={\upshape{(\hspace{-.25em}}}, itemjoin={/\hspace{-.25em}}, after={\upshape{)}}, label={}]% Weird formatting choices, as stated displays "/" between items.
	}
	{% Environment End Code
	\end{enumerate*}
	}


%multipleChoice** is for internal use only! (used in wordChoice)
%this is simply a wrapper for the sole showing (other)choice.
\newenvironment{multipleChoice**}[1][]{}{}



%wordChoice is an in-line version of multipleChoice:  uses enumitem package
%note, it is coded as a single line to avoid unwanted spaces in "given" mode
\newcommand{\wordChoice}[1]{%
	\let\choicetemp\choice% Assign a "choicetemp" command to duplicate choice.
	\ifwordchoicegiven
		\def\choice{\otherchoice}
		\begin{multipleChoice**}
			#1
		\end{multipleChoice**}
	\else
		\begin{multipleChoice*}
			#1
		\end{multipleChoice*}
	\fi
	\let\choice\choicetemp% Now that choicetmp has been manipulated to what we want, replace choice with it.
	}


\newenvironment{selectAll}[1][]
{\begin{trivlist}\item[\hskip \labelsep\small\bfseries Select All Correct Answers:]\hfil\begin{enumerate}}
{\end{enumerate}\end{trivlist}}

%% In the future we need this to (optionally) be displayed in the
%% problem, while the actual code lives in the solution. Here is how
%% this could be implemented: Like the title/maketitle commands, the
%% multiple-choice could be stored in \themultiplechoice, flip a
%% boolean, and execute \makemultiplechoice at the \end of the
%% problem.  We should also make a command called "\showchoices" that
%% will show choices in the handout.

\ifhandout
	\excludecomment{matrixAnswer}
%% Note:  This workaround will throw an error "File ended while scanning use of \next" if "\end{matrixAnswer}" does not appear on its own line with no spaces preceding it.  This is a documented issue with the \excludecomment command.

\else
	\newenvironment{matrixAnswer}
	{
    % First, we tell fancyvrb that we're inside a verbatim environment.
    \VerbatimEnvironment
    % Now we start the actual environment:
    \begin{Verbatim}[
        frame=lines,
        label=Matrix Answer,
        labelposition=topline,
      ]%
	}
	{\end{Verbatim}}
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
%		{}% Don't think this is doing anything? -- Jason
		}
		{%
		\ifthenelse{\givenatend}
			{% Begin then result
			\end{trivlist}
			}% End then result
			{% Begin else result
			\egroup
			}% End else result
%			{}% Don't think this is doing anything? -- Jason
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
			{%
			\egroup\ignorespacesafterend %% If this mysteriously starts breaking remove \ignorespacesafterend
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
			% %% line at the bottom} 
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






%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%		Verbatim Environments for displaying programming code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% code answer environment
%% You cannot use Environ with the fancyvrb/listings package if you want nested environments

\DefineVerbatimEnvironment{code}{Verbatim}{numbers=left,frame=lines,label=Code,labelposition=topline}



%% python answer environment
%% You cannot use Environ with the fancyvrb/listings package if you want nested environments

\DefineVerbatimEnvironment{python}{Verbatim}{numbers=left,frame=lines,label=Python,labelposition=topline}

%% \lstnewenvironment{python}
%%                   {\lstset{language=python,numbers=left}}
%%                   {}



% ---------------------------------------------------------------------
% Problem environment decorations (these should be user invoked, not default)

% The decoration for these environments were inspired by
% http://tex.stackexchange.com/questions/11098/nice-formatting-for-theorems?lq=1

%%
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


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%  		Problem Environments		%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Most of the below added by Jason Nowell

\newcommand{\latexProblemContent}[1]{#1}%% Added for those that want to use UF problems without using the problem filter code. This command is renewed into something meaningful in the 'ProblemSelector.sty'.
\Make@Counter{Iteration@probCnt}% Iterate count for problem counts.



%%%%%% Configure environment configuration commands

% The command \problemNumber contains all the format code to determine the number (and the format of the number) for any of the problem environments.
\newcommand{\problemNumber}{

% First we determine if we have a counter for this question depth level.
% -- Can probably replace this with Make@Counter at some point -- Jason

\ifcsname c@depth\Roman{problem@Depth}Count\endcsname	% Check to see if counter exists
	%If so, do nothing.
	\else
	%If not, create it.
	\expandafter\newcounter{depth\Roman{problem@Depth}Count}
	\expandafter\setcounter{depth\Roman{problem@Depth}Count}{0}
	\fi

\expandafter\stepcounter{depth\Roman{problem@Depth}Count}
\arabic{depthICount}% The first problem depth, what use to be "\theproblem".
\forloop{Iteration@probCnt}{2}{\arabic{Iteration@probCnt} < \numexpr \value{problem@Depth} + 1 \relax}{%
.\expandafter\arabic{depth\Roman{Iteration@probCnt}Count}% Get the problem number of the next depth level and append it with a ".".
}
%\ifhandout % Currently handout mode doesn't allow hints. Putting this code in place in case that changes.
%	\theproblem 
%\else
%	\theproblem 
%\fi
}



%%%%%%%%%%%%%%%%%%%%%%%

%%%%%% Configure various problem environment commands
\Make@Counter{problem@Depth}



%%%% Configure environments start content

\newcommand{\problemEnvironmentStart}[2]{%
% This takes in 2 arguments. 
% The first is optional and is the old optional argument from existing environments. 
%	This is passed down to the associated problem environment name in case you want a global value.
% The second argument is mandatory and is the name of the 'problem' environment, 
%	such as problem, question, exercise, etc. 
% It then configures everything needed at the start of that environment.

\stepcounter{problem@Depth}% Started a problem, so we've sunk another problem layer.
\def\spaceatend{#1}%
\begin{trivlist}%
\item%
  [%
	\hskip\labelsep\sffamily\bfseries% 
	#2%
	\problemNumber% Determine the correct number of the problem, and the format of that number.
	]
\slshape
}



%%%%% Configure environments end content

\newcommand{\problemEnvironmentEnd}{%This configures all the end content for a problem.
%
% First we need to see if we've dropped fully out of a depth level, 
% so we can reset that counter back to zero for the next time we enter that depth level.
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
%
% Old environments were "problem", "exercise", "exploration", and "question".
% Note that you can add content to the start/end code on top of these base code pieces if you want.


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



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%  				Validator			%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% New Validator rewrite code added by Jason Nowell. Original code provided by Jim Fowler
%
% Validator is an environment designed to run a custom check on answers (usually) using javascript code. 


% Define a placeholder command for validator and feedback.

\newcommand{\PH@Command}{}

%% Validator should take an argument and detokenize it and display it at the start of the environment. The original Validator environment had everything framed in an mbox; presumably to make the text look a bit nicer, although this seems redundant with texttt. It shouldn't cause any harm so I have left it in for now.

\newenvironment{validator}[1][]{
	\def\PH@Command{#1}% Use PH@Command to hold the content and be a target for "\expandafter" to expand once.
	\mbox{\texttt{\detokenize\expandafter{\PH@Command}}}% Now expand PH@Command once and then detokenize. 
	}{}

%%% Old validator environment definition has been depreciated. -- remove by 5/28/2018 if no issues reported.
%\def\validator[#1]{\mbox{\texttt{\detokenize{#1}}}}
%\def\endvalidator{}



%%%% Feedback

\ifhandout%	First, if it's a handout, we want feedback to eat everything and then disappear entirely. So we use the environ package to eat the content by not using \BODY command anywhere.
\NewEnviron{feedback}[1][]{}
% No! We are going to replace with the suppress command method. Delete the above old code by 6/28/2018 if no errors
%\newenvironment{feedback}{\expandafter\phantom\expandafter\begingroup\expandafter\noexpand\begingroup}{\endgroup\endgroup}

\else% If this isn't a handout, then we want to display the Feedback by using a label, 
%	positioned and formated as a \item in a trivlist. It is important that we also detokenize the content 
%	of the optional argument, as it is likely to contain javascript or other code that latex won't be 
%	able to make sense of.

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


%Old feedback code is retained below during depreciation period. -- delete by 5/28/2018 if no errors reported.

%\ifhandout
%
%\def\feedback{\@ifnextchar[{\@feedbackcode}{\@feedbackattempt}}
%\def\@feedbackcode[#1]{\setbox0\vbox\bgroup}
%\def\@feedbackattempt{\setbox0\vbox\bgroup}
%\def\endfeedback{\egroup}
%
%\else
%
%\def\feedback{\@ifnextchar[{\@feedbackcode}{\@feedbackattempt}}
%\def\@feedbackcode[#1]{\begin{trivlist}\item[\hskip \labelsep\small\slshape\bfseries Feedback (\texttt{\detokenize{#1}}):\hspace{2ex}]\small\slshape}
%
%\def\@feedbackattempt{\begin{trivlist}\item[\hskip \labelsep\small\slshape\bfseries Feedback (attempt):\hspace{2ex}]\small\slshape}
%
%\def\endfeedback{\end{trivlist}}
               
%\fi

               


%% Hints
%Create a counter that will track how deeply nested the current hint is
\newcounter{hintLevel}
\setcounter{hintLevel}{0}
%Create an empty shell to renew
\newenvironment{hint}{}{}
%Now we renew the environment as needed, this should allow support for any transition code that treats some parts as a "handout" and some parts as non-handout. 
% renewing the environment on the fly is a bit hacky. Maybe we should have a hintHandout env or option.
\renewenvironment{hint}
	{
	\ifhandout
		\setbox0\vbox\bgroup
	\else
		\begin{trivlist}\item[\hskip \labelsep\small\slshape\bfseries Hint:\hspace{2ex}]
		\small\slshape
	\fi
	%Step up hint level to track the nested level of the hint. This will be used for problem numbering
	\stepcounter{hintLevel}
	}
	{
	\ifhandout
		\egroup\ignorespacesafterend
	\else
		\end{trivlist}
	\fi
	%detract from hint level counter to track hint nested level
	\addtocounter{hintLevel}{-1}
	}

\ifhints
	\renewenvironment{hint}{
	\begin{trivlist}\item[\hskip \labelsep\small\slshape\bfseries Hint:\hspace{2ex}]
	\small\slshape}
	{\end{trivlist}}
\fi


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Dialogue

\newenvironment{dialogue}{%
   \renewcommand\descriptionlabel[1]{\hspace{\labelsep}\textbf{##1:}}
   \begin{description}%
}{%
   \end{description}%
}

\newcounter{problem} %% for the main problem

\colorlet{textColor}{black} % since textColor is referenced below
\colorlet{background}{white} % since background is referenced below

% The core environments. Find results in 4ht file.
%% pretty-foldable
\iftikzexport
\newenvironment{foldable}{%
}{%
}
\else
\renewmdenv[
  font=\upshape,
  outerlinewidth=3,
  topline=false,
  bottomline=false,
  leftline=true,
  rightline=false, 
  leftmargin=0,
  innertopmargin=0pt,
  innerbottommargin=0pt,
  skipbelow=\baselineskip,
  linecolor=textColor!20!white,
  fontcolor=textColor,
  backgroundcolor=background
]{foldable}%
\fi

%% pretty-expandable
\iftikzexport\newenvironment{expandable}{%
}{%
}
\else
\newmdenv[
  font=\upshape,
  outerlinewidth=3,
  topline=false,
  bottomline=false,
  leftline=true,
  rightline=false, 
  leftmargin=0,
  innertopmargin=0pt,
  innerbottommargin=0pt,
  skipbelow=\baselineskip,
  linecolor=black,
]{expandable}%
\fi
%% \newenvironment{expandable}{%
%% }{%
%% }

\newcommand{\unfoldable}[1]{#1}



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\DefineVerbatimEnvironment{javascriptCode}{Verbatim}{numbers=left,frame=lines,label=JavaScript,labelposition=topline}

\DefineVerbatimEnvironment{sageCell}{Verbatim}{numbers=left,frame=lines,label=SAGE,labelposition=topline}
\DefineVerbatimEnvironment{sageOutput}{Verbatim}{numbers=left,frame=lines,label=SAGE-Output,labelposition=topline}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\newcommand{\xkcd}[1]{#1}



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Leash

\newenvironment{leash}[1]{%
}{%
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% When designing new elements, it is nice to be able to wrap things in a class

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% When using mathjax, it is useful to dump all the newcommands to a "jax" file

\ifdefined\HCode
  \else
    \newwrite\myfile
    \immediate\openout\myfile=\jobname.jax
    \immediate\write\myfile{\unexpanded{\newenvironment}{prompt}{}{}}

    

\let\@oldargdef\@argdef
\long\def\@argdef#1[#2]#3{%
\immediate\write\myfile{\unexpanded{\newcommand}{\unexpanded{#1}}[\unexpanded{#2}]{\unexpanded{#3}}}%
\@oldargdef#1[#2]{#3}%
}

\let\@OldDeclareMathOperator\DeclareMathOperator
\renewcommand{\DeclareMathOperator}[2]{\@OldDeclareMathOperator{#1}{#2}\immediate\write\myfile{\unexpanded{\DeclareMathOperator}{\unexpanded{#1}}{\unexpanded{#2}}}}

    
\fi



\makeatother
