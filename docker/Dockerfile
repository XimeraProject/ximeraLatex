FROM registry.gitlab.com/islandoftex/images/texlive:TL2024-2024-11-17-medium

RUN apt-get update -y;  \
    apt-get install -y \
    curl \
    git  git-restore-mtime \
    mupdf-tools pdf2svg \
    gnupg \
    jq \
    vim wget strace less 


ADD hashcash_1.22-1_amd64.deb .
RUN dpkg -i hashcash_1.22-1_amd64.deb
    
# Install xake (from repo)
ADD scripts/xake.bin /usr/local/bin/xake


# Extra packages (on top of 'medium')
RUN tlmgr install \
        # afterpage \
        amsmath \
        # amssymb \
        # amsthm \
        # bm \
        booktabs \
        cancel \
        caption \
        ccicons \
        changebar \
        chemfig \
        chemgreek \
        chemmacros \
        circuitikz \
        # color \
        comment \
        currfile \
        datetime \
        enumitem \
        environ \
        epstopdf \
        eurosym \
        # extramarks \
        fancyhdr \
        fancyvrb \
        float \
        # fontenc \
        forloop \
        geometry \
        gettitlestring \
        # graphicx \
        hyperref \
        # ifpdf \
        # ifthen \
        # ifxetex \
        # inputenc \
        listings \
        makecell \
        makecmds \
        marginnote \
        mhchem \
        morewrites \
        # multicol \
        multido \
        multirow \
        # nameref \
        pdflscape \
        pgfplots \
        qrcode \
        relsize \
        # sagetex \
        silence \
        siunitx \
        tabto-ltx \
        # tabularx \
        tcolorbox \
        # textcomp \
        textpos \
        # tikz \
        tikz-3dplot \
        tikzsymbols \
        titlesec \
        # titletoc \
        titling \
        url \
        # verbatim \
        xcolor \
        xifthen \
        xkeyval \
        xstring \
        tikz-cd tkz-euclide \
        ifmtarg \
        etoolbox translations mathtools xfrac filehook \
        doclicense \
        simplekv fmtcount pdfcol \
        tikzfill \
        csquotes \
        babel-dutch \
        microtype \
        tex4ht make4ht \
        luaxml \
        fontawesome stix  \
        latexindent

# make4ht is not in /usr/local/bin if installed with tlmgr!!!
ENV PATH="$PATH:/usr/local/texlive/2024/bin/x86_64-linux"


# Add ximeraLatex repo
RUN mkdir -p /root/texmf/tex/latex
WORKDIR /root/texmf/tex/latex

RUN git clone https://github.com/XimeraProject/ximeraLatex.git 
WORKDIR /root/texmf/tex/latex/ximeraLatex
RUN git pull && git checkout luaxake

ADD scripts/xmlatex-lua /usr/local/bin/xmlatex
ADD scripts/xmlatex     /usr/local/bin/xmlatex.xake

ADD scripts/luaxake /root/texmf/tex/latex/luaxake

RUN ln -s /root/texmf/tex/latex/luaxake/luaxake /usr/local/bin/luaxake

RUN git config --global --add safe.directory /code

WORKDIR /code