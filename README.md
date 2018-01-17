Greetings!
==========

In this repository, we hope to supply potential authors of Ximera
activities with all the resources they need to get started. Note,
there will be many changes until May 1st.


Philosophy
----------

Since Ximera is built on LaTeX source, we want to use LaTeX as a
method of validating the code authors write. Hence, if you want to
write a Ximera online activity, the first step is constructing LaTeX
documents.

Once you have the LaTeX documents, and you have checked them for
typos, accuracy, etc, the fact that they compile should be reasonable
evidence that they will display correctly in Ximera.

Now it is time to deploy to the Ximera website. This feature is not
yet available, but it will be available in the coming months.


Contents of the repository
---------------------------

* This README.md file. 

* The GNU license.

* Documentation directory containing the documentation on how to write
  a Ximera activity. You will need to typeset the file
  ximeraInPractice.tex

* The directory inTheClassRoom, containing ideas on how to use Ximera
  activities in the classroom. This is surely incomplete at the
  moment, but it is our hope that over time these materials will grow.

* The Ximera document class, ximera.cls

* A file named exampleActivityCollection.tex, a file for testing
  purposes.

* Two test directories firstExample and secondExample that contain
  firstExample.tex, secondExample.tex. These are example files.

* A generic preamble file for each of the .tex files containing
  user-defined macros. Warning: Environments cannot be defined here.


Directions for download
-----------------------

### Register for a GitHub account

To start, you need a GitHub account. If you already have a GitHub
account, you can go on to the next step. Otherwise go to:

`https://github.com/`

and create an account. Choose the free plan, and no organization is
needed.


### Obtain a git client

#### For Mac or Windows

Go to: 

`https://desktop.github.com/`

and download GitHub.

#### For Linux

This will depend on your flavor of Linux. However, it will be
something like:

`$ sudo apt-get install git-core`

or

`$ pacman -S git`

Regardless, a search for `git <my Linux variety>` should lead you in
the right direction.


### Clone the ximeraLatex repository

Depending on your operating system, these command may be different. 

#### For Mac or Windows

When you start your client, it may ask if you want to use SSH, and you
do. When it asks which local repositories you want to use, just click
"Done."

Visit the repo on Github.com and click the "Clone in Desktop" button.


#### For Linux

Use SSH (requires setting up an ssh key, instructions [here](https://help.github.com/articles/generating-an-ssh-key/):
`$ git clone git@github.com:XimeraProject/ximeraLatex.git`

or

Use HTTPS (no key needed):
`$ git clone https://github.com/XimeraProject/ximeraLatex.git`


Setting up your local directories
---------------------------------

#### For Linux

Create the directory structure:

`~/texmf/tex/latex/`

and move ximeraLatex to `~/texmf/tex/latex/`. This will allow all of
your documents to find ximera.cls

#### For Mac

Create the directory structure:

`~/Library/texmf/tex/latex/`

and move ximeraLatex to `~/Library/texmf/tex/latex/`. This will allow
all of your documents to find ximera.cls. Once you move ximeraLatex,
you will need to tell your GitHub client where to look for it, you can
do this using "find files."

#### For Windows

Create the directory structure:

`C:\localtexmf\tex\latex\`

and move ximeraLatex to `C:\localtexmf\tex\latex\`. For MiKteX to notice this
directory, go to:

1. Start -> All programs -> MiKTeX Folder -> Maintenance (Admin) Folder -> Settings (Admin).
2. Now select the tab "Roots."
3. Click "Add" because you are going to add a path.
4. Find `C:\localtexmf\` and click "OK."
5. Click "apply" then "OK."
6. Reopen Miktex Settings (Admin). Click Refresh FNDB.

This will allow all of your documents to find ximera.cls. Note, it is
important that none of the directories containing ximeraLatex have
spaces in their names. Once you move ximerLatex, you will need to tell
your GitHub client where to look for it, you can do this using "find
files."

#### ximeraLatex vs. ximera.cls

While one could simply move ximera.cls to the directory on the path,
we suggest that the entire ximeraLatex directory should be moved as
described above. This will allow users to update to the most recent
version of the ximera.cls and discourage users from working directly
in ximeraLatex. When building activities, each activity should be in
its own directory.


### Check the ximeraLatex directory

Now you should have a directory called ximeraLatex. If you are running
windows (MiKTeX), you will need to copy (or move) the files listed
below to a directory that MiKTeX expects to find *.tex files in, for
example, a subdirectory of "Documents."

* Compile ximeraLatex/documentation/ximeraInPractice.tex

* Compile ximeraLatex/exampleActivityCollection.tex

* Compile ximeraLatex/firstExample/firstExample.tex

* Compile ximeraLatex/secondActivity/secondExample.tex

All of these documents should compile at this point. If they do not,
something is probably wrong.


Creating your GitHub repository
-------------------------------

Now that you have the ximera.cls file and can compile
exampleActivityCollection.tex, /firstExample/firstExample.tex, and
/firstExample/firstExample.tex it is time to get started on your own
GitHub repo.

You should not build your Ximera activities in ximeraLatex. The
directory ximeraLatex's sole purpose is to give you the most recent
version of ximera.cls and some examples. Hence you must build your
activities in a new directory. In fact, you should build them in your
own GitHub repo.

#### Create your working directory

To start, make a directory where your activities will live. Give this
directory a descriptive name, ximeraActivities, or something
similar. For example purposes, we will use this name, but you should
use your own. Inside of this directory, you should have

* A README.md file giving a brief explanation of what these files are.

* A LICENSE, you can simply copy the one from ximeraLatex

* A directory for each individual activity, named the same as the
  activity (without the .tex suffix) with a directory structure
  similar to the one found in ximeraLatex.

* A master activity file, one that is similar to
  exampleActivityCollection.tex


#### Creating the repository in Linux

Go to GitHub, click ``+'' and Create New Repository Give it a title,
and a short description.

Ignore the ``initialize this repo with a README''

Click ``Create repository''

Now we will push your current directory to this repo. Once you are within your repo:

`git init`

`git add .`

`git commit -m 'first push'`

`git remote add origin git@github.com:YOUR-GIT-USER/REPO-NAME.git`

`git push -u origin master`

where YOUR-GIT-USER is your GitHub username and REPO-NAME is the name
of the repository you created.


#### Creating the repository via a client

Drag and drop you new folder on to the (probably empty) repository
window. Then push to GitHub with the button in the upper right hand
corner. You want to always allow GitHub to use your SSH KeyChain.

You can confirm a repository was made by going to GitHub and viewing
your user profile. To add new files to the repository, go to the
"Repository" heading in the Git client and click "open in finder."
From there one can add files by dragging and dropping, and then
syncing.


Writing an activity
-------------------

Now it is time to write your first activity. All activities should be
in their own directory, with all supporting documents also in this
directory. This will provide maximum flexibility and modularity of
your activity.

If you follow the directory structure suggested in ximeraLatex and in
ximeraLatex/documentation/ximeraInPractice.pdf (assuming you've
compiled it!) you will be able to compile each activity on its own, or
as a collection.


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


