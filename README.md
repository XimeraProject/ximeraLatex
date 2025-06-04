
This repository contains the source for the `ximera` LaTeX package, and some supporting scripts. 

This repo is *not* directly relevant for (prospective) Ximera authors or endusers.
They should consult 
* an [Example Ximera Xourse](https://go.osu.edu/ximera-examples) to see what Ximera is and can do.
* the [https://github.com/XimeraProject/ximeraFirstSteps] repo to get a hands-on introduction, or
* the [Ximera Manual](https://ximera.osu.edu/xman) the get a more detailed introduction and overview.


The Ximera document class is available in [CTAN](https://ctan.org/pkg/ximera?lang=en), but it is strongly advised to use Ximera with [Codespaces or docker](https://github.com/XimeraProject/ximeraFirstSteps). 


This repo also contains the (bash-) script `xmlatex` that compiles PDF and HTML versions, and publishes to a Ximera Server, and transparently starts a docker container if needed and possible. Most of the actual work is done by the (lua-) buildscript `luaxake`, for which `xmlatex`is essentially just a wrapper. To compile .tex files, `luaxake` essentially just runs `pdflatex` and `make4ht`, but it has dependency checking, does some HTML cleanup, and has adhoc code to publish .html (and .pdf) to a Ximera Server. 
This repo also contains the `Dockerfile(s)` to build docker images which currently provide the most typical way to use Ximera.

Manual local installation of the Ximera LaTeX package is normally never needed, but (somewhat old) documentation remains [available](./installingLocally.md).


# Contents of the repository

* This README.md file. 

* The LPPL 1.3c license.

* The Ximera documented LaTeX file type, ximera.dtx. This file
  generates ximera.cls, xourse.cls, and ximeraLaTeX.pdf, as well as a
  few other files, from the source .dtx files is `src`.

* In the `src` folder the ximeraLatex source files (as used by ximera.dtx)

* In the `luaxake` folder the LUA code of the `luaxake` build script. This has its own [README](luaxake/README.md).

* In the `xmScripts` folder, the (wrapper-) script `xmlatex`. One version goes into the docker image, a simplified 'header' part should go in each ximera-repo (or just once somewhere in the PATH) of your PC.

* In the `docker` folder build files for docker images. Images are automatically build for each tag of this repo, and released versions are available from [github](https://github.com/orgs/XimeraProject/packages)


# A Non-Official Changelog
* 5/2025: Merge the 'luaxake' branch into master. This obsoleted the older 'xake' program (written in GO), and was already used for quite some time by the dockerimages v2.6.x.

# A Non-Official List Of Possible Future Features

- Ability to include \activities and \practice within a ximera document
  - when adding Xourses, the path to the xourse/file needs to be given
in some way. We have an example of this in the preamble of
examples/exerciseCollection/exerciseCollection.tex This enables an
author to print the file and know where to find the parts.
  - Perhaps by default, all Xourse files appear on the top page, but if modified with `\documentclass[hidden]{xourse}, they would no longer appear
  - A separate, perhaps password protected page with ALL content on it.

# A Non-Official ToDo List
 * verify and remove all OLD_xxx branches
 * Cleanup/solve the Issues
 * Merge (at least part of?) luaxakeuf branch
 * Move the above Possible Furure features elsewhere
 * Do all this ToDo's
 * Do the above ToDo


# Advanced: building (local) docker images

Check out this repo and run (from the root folder)
```
docker buildx build --tag ghcr.io/ximeraproject/ximeralatex:latest --file docker/Dockerfile.full .
```

and test or use the newly build image eg with 
```
XAKE_VERSION=latest  xmlatex bake mytestfile.tex
```
To further develop, test or manipulate Ximera, you can work inside the container with
```
XAKE_VERSION=latest  xmlatex bash
```
It is possible to extract this package from the container into a .ximera_local folder, and develop from there with 
```
XAKE_VERSION=latest  xmlatex copySettingsLocal
```

The default XAKE_VERSION (and thus the container to be used) can also be set in xmScripts/config.txt.



 # Advanced: compiling the `ximera` LaTeX package

Running `make` generates the derived files README, ximera.pdf, ximera.cls, xourse.cls, ximera.cfg, ximera.4ht, xourse.4ht.

Running `make ctan` generates a submission suitable for CTAN

(OBSOLETE) Running `make inst` installs the files in the user's TeX tree.

(OBSOLETE) Running `make install` installs the files in the local TeX tree.

All this can (optionally) done INSIDE a ximeralatex docker container, ie after running
```
xmlatex bash
```
in the root folder of this repo.