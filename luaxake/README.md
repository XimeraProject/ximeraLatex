# Luaxake

 `luaxake` is a reimplementation in LUA of Ximera's original `xake` program, that was written in GO.

 The main part of the reimplementation was done by Michal Hoftich (summer 2024), and it was further completed by Wim Obbels (dec 2024).

 The `luaxake` program is (or should) most often be called via the wrapper script `xmlatex`, that automatically starts a docker container if needed, and provides some extra defaults and options. 

The `luaxake` program should provide

- '`bake`: convert to PDF and/or HTML all standalone TeX files in directories given as argument
  - find (interesting....) files (.tex, .pdf, .html...)
  - detect dependencies  (mainly relevant in the HTML case to get proper titles/abstracts for xourses out of .html files)
  - (re-)compile (only) if the files themselves or their dependencies have changed
  - (standalone files are files that contain a `\documentclass` command)

- `name`: setup a destination ximera-server. This currently needs a GPG key, and is done in `xmlatex`. This could/should change in/with a new ximera server setup....

- `frost`: create a git tag for publishing, containing the necessary output files and some metadata
 in the generated file `metadata.json`

- `serve`: publish the (previuosly `frosted`) content to a (previously `name`d) ximera server


# Usage:

```
    luaxake [options] command <path/to/directory...>
```

# Options   (TO BE UPADTED)

The command is `bake`, `frost`, `serve` or a number of variants/alternatives/extensions.

One or more directories and/or TeX files can be given as argument.

For the options, see `luaxake -h`, or the `luaxake`source code. Main options are

- `-c`,`--config` -- name of TeX4ht config file. It can be full path to the
  config file, or just the name. If you pass just the filename, Luaxake will
  search first in the directory with the current TeX file, to support different
  config files for different projects, then in the current working directory,
  project root and local TEXMF tree.

- `-l`,`--loglevel` -- Set level of messages printed to the terminal. Possible
  values: `debug`, `info`, `status`, `warning`, `error`, `fatal`. Default value is `status`,
  which prints warnings, errors and status messages.

- `-s`,`--settings` -- Lua script that can change Luaxake configuration settings.


# Lua settings  (implementation to be documented/improved)

You can set settings using a Lua script with the `-s` option. The script should 
only set the configuration values. For example, to change the command for HTML 
conversion, you can use the following script:

```Lua 
compilers.html.command = "make4ht -c @{config_file} @{filename} 'options'"
```

## Available configuration settings

- `output_formats` -- list of extensions of output formats

```Lua
output_formats = {"html", "pdf", "sagetex.sage"},
```

OBSOLETE: this is calculated by `luaxake` now, based on the `compile_sequence`

- `compile_sequence` -- sequence  of compilers to be called on each TeX file

```Lua
compile_sequence = {"pdf", "sagetex.sage", "pdf", "html"},
```

- `clean` -- list of extensions to be removed after compilation

```Lua
clean = { "aux", "4ct", "4tc", "oc", "md5", "dpth", "out", "jax", "idv", "lg", "tmp", "xref", "log", "auxlock", "dvi", "scmd", "sout" }
```

## Compilers

- `compilers` -- settings for compiler commands. Each compiler contains table with additional settings.

There are several available compilers, and more can be added in the settings:

- `pdf` -- command used for the PDF generation
- `html` -- command used for the HTML generation
- `sagetex.sage` -- command used for the `sagetex.sage` generation  (NOT YET IMPLEMENTED)

```Lua
compilers = {
  html = {
    command = "make4ht -f html5+dvisvgm_hashes -c @{config_file} -sm draft @{filename}",
    check_log = true, -- check log
    status = 0 -- check that the latex command return 0
  },
}
```

### Settings available in the `compiler` table:

- `check_log` -- should we check the log file for errors?
- `check_file` -- check if the file exists before compilation. It is used by `sage`, which must be executed only if `filename.sagetex.sage` exists.
- `status` -- expected status code from the command.
- `process_html` -- run HTML post-processing.
- `command` -- template for the command to be executed. `@{variable}` tag will be replaced with the content of variable. 

### Variables available in command templates:

  - `dir` -- relative directory path of the file 
  - `absolute_dir` -- absolute directory path of the file
  - `filename` -- filename of the file
  - `basename` -- filename without extension
  - `extension` -- file extension
  - `relative_path` -- relative path of the file 
  - `absolute_path` -- absolute path of the file
  - `exists` -- boolean, true if file exists
  - `config_file` -- TeX4ht config file
