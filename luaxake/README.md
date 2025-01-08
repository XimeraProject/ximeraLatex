# Luaxake

This is a reimplementation of Xake using Lua.

What it should do:

- convert all standalone TeX files in a directory tree to PDF and HTML:
  - we search for all files in subdirectories of the path
  - standalone files are files that contain `\documentclass` command
  - we detect included TeX files and recompile if any dependency is updated

# Usage:

    $ texlua luaxake [options] path/to/directory

# Options

- `-c`,`--config` -- name of TeX4ht config file. It can be full path to the
  config file, or just the name. If you pass just the filename, Luaxake will
  search first in the directory with the current TeX file, to support different
  config files for different projects, then in the current working directory,
  project root and local TEXMF tree.

- `-l`,`--loglevel` -- Set level of messages printed to the terminal. Possible
  values: `debug`, `info`, `status`, `warning`, `error`, `fatal`. Default value is `status`,
  which prints warnings, errors and status messages.

- `-s`,`--script` -- Lua script that can change Luaxake configuration settings.


# Lua configuration 

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

- `documentclass_lines`   -- number of lines in tex files where we should look for `\documentclass`. Luaxake compiles only files which contains the 
  `\documentclass` command on a line in this range.

```Lua
documentclass_lines = 30,
```

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

There are three available compilers, but you can add more:

- `pdf` -- command used for the PDF generation
- `html` -- command used for the HTML generation
- `sagetex.sage` -- command used for the `sagetex.sage` generation

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
