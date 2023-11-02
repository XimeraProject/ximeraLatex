# Installing Locally

To get started as an author in Ximera, all you need is the XimeraLaTeX
LaTeX Package. Unfortunately, at this point, this package is not part
of CTAN. However, you can install it manually with the instructions
below. Please feel free to contact Bart Snapp with questions
(snapp.14@osu.edu).

1. Get a GitHub account
2. Go to: [https://github.com/XimeraProject/ximeraLatex](XimeraLaTeX on GitHub)
3. Click on the green code button: ![Click on the green code button.](https://github.com/XimeraProject/.github/blob/main/profile/codeButton.png "Click on the green code button.") and copy the code, (or just copy: `git@github.com:XimeraProject/ximeraLatex.git`)
4. Clone the repository. 

At this point the installation becomes operating-system specific.

# Linux

If you are running linux, create the local directory structure `~/texmf/tex/latex`

├── texmf

├───└── tex

└───────└── latex

Now move the `ximeraLatex` folder (cloned in step 4. above) into the latex folder. At this point you have installed XimeraLaTeX! You can test your installation by compiling:
```
\documentclass{ximera}
\begin{document}
\begin{problem}
Hello $\answer[format=string]{World}$
\end{problem}
\end{document}
```

# MacOS

If you are running MacOS, create the local directory structure `~/Library/texmf/tex/latex`. To do this, you'll need to make your Library folder visible, see [http://kb.mit.edu/confluence/display/istcontrib/How+to+make+your+Library+folder+visible+in+the+Finder+in+OS+X+10.9+%28Mavericks%29+or+later](How to make your Library folder visible in the Finder in OS X 10.9 (Mavericks) or later).

├── Library

├───└── texmf

├───────└── tex

└───────────└── latex

Now move the `ximeraLatex` folder (cloned in step 4. above) into the latex folder. At this point you have installed XimeraLaTeX! You can test your installation by compiling:
```
\documentclass{ximera}
\begin{document}
\begin{problem}
Hello $\answer[format=string]{World}$
\end{problem}
\end{document}
```


# Windows
