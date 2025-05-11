local M = {}
local lfs = require "lfs"
local error_logparser = require("make4ht-errorlogparser")
local pl = require "penlight"
local path = pl.path
local pldir = pl.dir
local plfile = pl.file

local tablex = require("pl.tablex")

local html = require "luaxake-transform-html"
local files = require "luaxake-files"      -- for get_fileinfo
local frost = require "luaxake-frost"      -- for osExecute
local socket = require "socket"
local ffi = require "ffi"

local log = logging.new("bake")

--- remove temporary files
---@param basefile fileinfo 
---@param extensions table    list of extensions of files to be removed
---@return  number nfiles     number of files removed
local function clean(basefile, extensions, infixes, only_check)
  only_check = only_check or false
  local nfiles = 0
  local basename = path.splitext(basefile.absolute_path)
  log:tracef("%s the temp files for %s (%s)", (only_check and "Would remove" or "Removing"), basename, basefile.absolute_path)

  for _, infix in ipairs(infixes) do
    for _, ext in ipairs(extensions) do
      local filename = basename .. infix .. "." .. ext
      if path.exists(filename) then
        log:debugf("%s %-14s file %s", (only_check and "Would remove" or "Removing") ,infix.."."..ext, filename)
        if not only_check then os.remove(filename); nfiles = nfiles + 1 end
      -- else
      --    log:debugf("No file %s present", filename)
      end
    end
  end
  return nfiles
end



local function parse_log_file(filename)
  -- log:errorf("PARSING %s", filename)
  local f = io.open(filename, "r")
  if not f then 
    log:warningf("Cannot open log file %s; SKIPPING parsing logfile for errors ", filename)
    return nil 
  end
  local content = f:read("*a")
  f:close()
  local result =  error_logparser.parse(content)
  log:tracef("PARSING got %s", result)
  return result
end

--
-- These next functions are/can be called by post_command in config.commands
-- HACK: these currently need to be global; TODO: fix!
-- The functions should return an updated 'cmd' structure, with eg
--  cmd.final_output_file = <the post-processed-output file>
--   cmd.status_post_command = "OK"
function post_process_html(cmd)
  -- simple wrapper to make it work in post_command
  --
  return html.post_process_html(cmd)
end

function post_process_pdf(cmd)
  -- move the pdf to a corresponding folder under root_dir (presumably ximera-downloads, with different path/name!)
  --
  -- use absolute paths when running in chdir-context during compilation ....
  local file = cmd.file
  local src_filename = cmd.output_file
  local absfolder = path.join(GLOB_root_dir, cmd.command_metadata.download_folder, file.relative_dir)
  local relfolder = path.join(               cmd.command_metadata.download_folder, file.relative_dir)
  local abstgt    = path.join(absfolder, file.basename ..".pdf")
  local reltgt    = path.join(relfolder, file.basename ..".pdf")
  -- require 'pl.pretty'.dump(src)
  if not path.exists(src_filename) then
    log:warningf("Output file %s does not exists (for %s)",src_filename, file.relative_path)
  else
    log:infof("Moving %s to %s", src_filename, reltgt)
    pldir.makepath(absfolder)
    plfile.copy(src_filename, abstgt)
  end

  if file.relative_path:match("_pdf.tex$" ) then
    log:infof("Convert _pdf.pdf file to svg for  %s",file.relative_path) 
    -- Mmm, osExecute should better not be in module 'frost'
    frost.osExecute("pdf2svg " .. file.absolute_path:gsub(".tex",".pdf") .. " " .. file.absolute_path:gsub(".tex",".svg"))
  end

  cmd.final_output_file = reltgt
  cmd.status_post_command = "OK"
  return cmd
end

--- run a complete compile-cycle on a given file
--- 
--- SIDE-EFFECT: adds output_files to the file argument !!!
--- 
--- @param file fileinfo file on which the command should be run
--- @param compilers [compiler] list of compilers
--- @param compile_sequence table sequence of keys from the compilers table to be executed
--- @return [compile_info] statuses information from the commands
local function get_commands_to_run(file, compilers, compile_sequence, only_check)
  only_check = only_check or false
  local compile_commands = {}

  -- Collect ALL needed compilations for this file
  -- NOTE: extension is a bad name, it's rather  'compiler'
  for _, extension in ipairs(compile_sequence) do
    log:tracef("Collecting %s compilation of %s (%s)", extension, file.relative_path, file.tex_documentclass)
    local command_metadata = compilers[extension]

    if not command_metadata then
      log:errorf("No compiler defined for %s (%s); SKIPPING",extension,file.relative_path)
      goto uptonextcompilation  -- nice: a goto-statement !!!
    end
    -- This could/should perhaps be handled higher up? Compilation of e.g. preamble.tex does not make sense ...
    if not file.tex_documentclass then
      log:infof("Skipping %s compilation of non-tex-document %s",extension, file.relative_path)
      goto uptonextcompilation 
    end
    if file.tex_documentclass ~= "ximera" and file.tex_documentclass ~= "xourse" and string.match(extension,"html")  then
      log:infof("Compiling a non-ximera %s file %s with 'xhtml' (and thus not ximera.cfg)", file.tex_documentclass,  file.relative_path)
      file.configfile = "xhtml"
    end

    if file.extension ~= "tex" then
      log:errorf("Can't compile non-tex file %s; SKIPPING, SHOULD PROBABLY NOT HAVE HAPPENED",file.relative_path)
      goto uptonextcompilation 
    end


    -- HACK: _pdf.tex and _beamer.tex files should by convention NOT generate HTML (as they typically would contain non-TeX4ht-compatible constructs)
    if extension:match("html$") and ( file.relative_path:match("_pdf.tex$") or file.relative_path:match("_beamer.tex$") ) then
      log:infof("Skipping HTML compilation of pdf-only file %s",file.relative_path) 

      -- create/update a dummy outputfile to mark this file 'uptodate'
      local filename = file.absolute_path:gsub(".tex$",".html")
      local file, err = io.open(filename, "r")
    
      if file then
          -- File exists, update modification time
          file:close()
          lfs.touch(filename)
      else
          -- File doesn't exist, create a new one
          file, err = io.open(filename, "w")
          if file then file:close()
          else         log:infof("Failed to fix dummy htmlfile %s: %s",filename,err)
          end
      end
      goto uptonextcompilation 
    end
  
    -- Construct the expected names of the generated output and logfiles
    local infix = ""    -- used for compilation-variations, eg 'handout' of 'make4ht'/'draft'
    if command_metadata.infix and command_metadata.infix ~= "" then
      infix = command_metadata.infix.."."
    end
    local output_file   = file.absolute_path:gsub("tex$", infix..command_metadata.extension)   -- to be generated by compile
    local log_file      = file.absolute_path:gsub("tex$", infix.."log")                       -- hopefully this is where the logs go
    local rellog_file   = file.relative_path:gsub("tex$", infix.."log")                       -- hopefully this is where the logs go

    -- sometimes compiler wants to check for the output file (like for sagetex.sage),
    if command_metadata.check_file and not path.exists(output_file) then
      log:debugf("Skipping compilation because of 'check_file', and file %s does not exist",output_file)
      goto uptonextcompilation  -- TODO: CHECK (for sagetex.sage ...)
    end
    
    if output_file.exists and not output_file.needs_compilation then
      log:debugf("Mmm, compiling file %s which was registered as not needing compilation.",output_file)
    end

    -- replace placeholders like @{filename} with the corresponding keys (from the metadata table, or config)
    local command = command_metadata.command
    command = command:gsub("@{(.-)}", file)
    command = command:gsub("@{(.-)}", { output_file = output_file })        -- used for sage ...
    command = command:gsub("@{(.-)}", config)

    log:debug("Adding command " .. command )

    local cmd = { 
        id=extension.."|"..file.relative_path, 
        file=file, 
        extension=extension, 
        command=command, 
        command_metadata=command_metadata, 
        output_file = output_file,
        log_file=log_file, 
        rellog_file=rellog_file, -- for logging ...
        only_check=only_check, 
        depends_on_cmds = {}
    }

    if type(file.depends_on_files == "table") then
      for key, _ in pairs(file.depends_on_files) do -- The keys should be same as value.relative_path
        cmd.depends_on_cmds[extension .. "|" .. key] = extension .. "|" .. key -- natee a bit redundant for now
      end
    else
      log:debugf("file.depends_on_files is not a table for cmd.id: %s", tostring(cmd.id))
    end

    table.insert(compile_commands, cmd)

    log:tracef("ADDED %s for %s of %s", cmd.id, cmd.extention, file.relative_path)
      

    ::uptonextcompilation::
  end

  return compile_commands

end


local function do_command_handle(cmd)

  -- log:info("START do_command_handle")

  local file=cmd.file
  local extension=cmd.extension

  log:debugf("Handling return of %s for %s: returns %s (expected %d) after %.3f seconds", cmd.extension, file.relative_path, cmd.status_command or 42, cmd.command_metadata.status or 42, cmd.compilation_time or 42)

      if cmd.command_metadata.check_log then

        log:tracef("Checking logfile %s", cmd.log_file)
        local errors = parse_log_file(cmd.log_file)  -- gets errors the make4ht-way !
        cmd.errors = errors             -- keep them around
        
        -- Show nicely formatted errors 
        local err_context = ""
        local err_line = ""
        for i, err in ipairs(errors or {}) do

          -- Format errormessage a bit, and store it in err.constructed_errormessage
          err_context  = "at "..err.context
          err_line = ""
          if err.line then err_line = "[l." .. err.line .. "]" end
          
          -- remove useless context ...
          if err.context:match('See the LaTeX manual or LaTeX Companion for explanation') 
          or err.context:match('^ <-') then
            err_context = ""
          end
          
          err.constructed_errormessage =  string.format("%s %-30s %s", err_line,  err.error, err_context)

          if i<10 then
            log:errorf("Compile error in %-20s:%s", cmd.rellog_file, err.constructed_errormessage)
          elseif i == 10 then
            log:warningf("Compile error in %-20s: skipping further errorlog; %d errors found", cmd.rellog_file, #errors)
          end          
        end
      end

    if cmd.status_command ~= cmd.command_metadata.status then
      -- log:errorf("Compilation of %s for %s failed: returns %d (not %d) after %3f seconds", extension, file.relative_path, cmd.status_command, cmd.status_expected, compilation_time)
      log:errorf("Compilation of %s for %s failed: returns %d (not %d)", extension, file.relative_path, cmd.status_command, cmd.command_metadata.status)
      if path.exists(cmd.output_file) then
        -- prevent trailing non-correct files, as they prevent automatic re-compilation !
        log:infof("Moving output of failed compilation to %s", cmd.output_file..".failed")
        pl.file.move(cmd.output_file, cmd.output_file..".failed")
      end
      cmd.status = "COMMAND_FAILED"   -- adhoc errorstatus
      cmd.status_post_command = nil -- In case this is from a retry
      return cmd  
    end

    -- The 'output_file' might need to be post-processed into a 'final_output_file'
    --  ( eg html manipulation, or moving a pdf to a downloads folder)
    -- in case no postprocessing: 
    local final_output_file = cmd.output_file
    if cmd.command_metadata.post_command then
      local post_command = cmd.command_metadata.post_command
      log:debugf("Start post_processing %s: %s",cmd.id, post_command)
      -- call the post_command
      cmd = _G[post_command](cmd)     -- lua way of calling the function whose name is in 'post_command'
      
      -- The post_command might/should have created a new ('final') output file
      final_output_file = cmd.output_file_postprocessed
    end

    if final_output_file then
      log:debugf("Adding outputfile %s for %s ", final_output_file, file.relative_path)
      if not file.output_files_made then file.output_files_made = {} end
      file.output_files_made[final_output_file] = files.get_fileinfo(final_output_file, true)     -- never used?
    end

    if config.noclean then
      log:debugf("Skipping cleaning temp files")
    else
      local infix = ""
      if cmd.command_metadata.infix and cmd.command_metadata.infix ~= "" then
        infix = "."..cmd.command_metadata.infix
      end
      clean(file, config.clean_extensions, { infix })
    end    
      
    cmd.status = "OK" 
    -- log:trace("DONE do_command_handle")

    -- -- Update 'needs_compilation' ... (BADBAD: should probably be done in a better way ...)
    -- files.update_output_files(file, output_formats)
    -- log:infof("Updated status of %s:%s uptodate", file.relative_path, file.needs_compilation and ' NOT' or '' )
    
    -- files.dump_fileinfo(file)     -- only for debugging
    
    return cmd
end


-- POSIX system calls we need for non-blocking popen
-- Note this makes us POSIX dependent, but we could add Windows system calls to support Windows if we ever needed
ffi.cdef([[
  void* popen(const char* cmd, const char* mode);
  int pclose(void* stream);
  int fileno(void* stream);
  int fcntl(int fd, int cmd, int arg);
  int *__errno_location();
  ssize_t read(int fd, void* buf, size_t count);
]])

-- Lua versions of Linux C macros to check status returned by pclose (Technically system dependent)
function _WSTATUS(x) -- Checks to see if any of the last 7 bits are set to 1
  return x & 127
end

function WIFEXITED(x)
  return _WSTATUS(x) == 0
end

function WEXITSTATUS(x)
  return  (x >> 8) & 255
end
  
local F_SETFL = 4
local O_NONBLOCK = 2048
local EAGAIN = 11

function do_command_start(cmd)
  
  local file=cmd.file
  local command=cmd.command
  local folder=file.absolute_dir
  local _errno = ffi.C.__errno_location()
  
  log:tracef("Starting process in %s with command %s " , folder, command)

  local process = {}
  process.cmd = cmd
  process.file_name = file.absolute_path

  process.start_time =socket.gettime()

  if cmd.only_check then
    log:info("Running in check-modus: SKIPPING " .. command )
    command = "echo SKIPPED " .. command
  end 

  log:tracef("Command %3s started for %s", cmd.job_nr, cmd.id )


  -- Start process with "command"
  process.handle = ffi.C.popen("cd "..folder.."; ".. command, "r")
  if process.handle == nil then
    log:warningf("ffi.popen returns %s", popen)

    local err_code = _errno[0]
    return tonumber(err_code), "Error trying to popen command: " .. tostring(command) .. "Error code: " .. tostring(err_code)
  end

  -- Get file descriptor of pipe
  process.fd = ffi.C.fileno(process.handle)
  log:tracef("ffi.fileno returns %s", process.fd)
  
  if process.fd == -1 then
    local err_code = _errno[0]
    local err_msg = "Failed to get file descriptor for command: " .. tostring(command) .. ". Error code: " .. tostring(err_code)

    local status_code = ffi.C.plcose(process.handle)

    if status_code -1 then
      err_code = _errno[0]
      err_msg = err_msg .. ". Failed to pclose process. Error code: " .. tostring(err_code)
    end

    return tonumber(err_code), err_msg
  end

   -- Set non-blocking mode for pipe
  local status_code = ffi.C.fcntl(process.fd, F_SETFL, O_NONBLOCK)
  if status_code ~= 0 then
    err_code = _errno(0)
    log:info("failed fcntl. status_code: " .. tostring(status_code) .. ". error: " .. tostring(_errno[0]))
    return tonumber(err_code), "Failed to set non-blocking reads for pipe to command: " .. tostring(command) .. ". Error code: " .. tostring(err_code)
  end

  return 0, process
end

function cmd_can_run(cmd, commands_to_run, current_processes, commands_that_ran) 

  for command_id, _ in pairs(cmd.depends_on_cmds) do
    -- Check to see if this dependency still needs to run
    for _, command in ipairs(commands_to_run) do
      if command_id == command.id then
        log:debugf("NEXTCMD: %s WAIT because %s must run first", cmd.id, command_id )
        return "WAIT"
      end
    end

    -- Check to see if the dependency is currently running
    for _, process in ipairs(current_processes) do
      if command_id == process.cmd.id then
        log:debugf("NEXTCMD: %s WAIT because %s currently running", cmd.id, command_id )
        return "WAIT"
      end
    end

    -- Check to see if this dependency ran and failed
    for _, command in ipairs(commands_that_ran) do
      if command_id == command.id and command.status ~= "OK" then
        if tostring(command.status_post_command) == "RETRY_COMPILATION"  then
            log:debugf("NEXTCMD: %s WAIT because %s will be retried", cmd.id, command_id )
            return "WAIT"
        else
          log:debugf("NEXTCMD: %s SKIP because %s failed", cmd.id, command_id )
          cmd.errors = command_id
          return "SKIP"
        end
      end
    end
  end

  log:debugf("NEXTCMD: %s OK", cmd.id)
  return "OK"
end

function bake(to_be_compiled, n_jobs)
  local commands_to_run = {}
  local commands_that_ran = {}

  n_jobs = tonumber(n_jobs)
  if n_jobs == nil or n_jobs < 1 then
    log:warning("n_jobs was invalid. Setting to 2")
    n_jobs = 2
  end


  log:tracef("Collecting all needed compile commands for %s to be compiled files", #to_be_compiled)
  for i, file in ipairs(to_be_compiled) do
    
    local extra_run_commands = get_commands_to_run(file,  config.compilers, config.compile_sequence, config.check)
    tablex.insertvalues(commands_to_run, extra_run_commands)

    log:infof("Added %d compile commands for file %3d/%d: %s", #extra_run_commands, i, #to_be_compiled, file.relative_path)
  end

  local job_total = #commands_to_run
  log:statusf("There are %d commands to run for %d files", job_total, #to_be_compiled)
  log:trace_table(commands_to_run)
    

  local _errno = ffi.C.__errno_location() -- Get pointer to errno location
  local buffer_size = 2025
  local read_buffer = ffi.new("char[?]", buffer_size)

  local current_processes = {}
  local current_process = 1
  local job_nr = 1
  local n_commands_failed = 0

  local total_start_time =  socket.gettime()

  log:debugf("Starting processes (up to %d)", n_jobs)
  
  -- while there is work going on or work to be done 
  while #current_processes > 0 or #commands_to_run > 0 do
    
    -- Start new processes ( if slots are available, and there are more jobs to process)
    local i = 1
    while #current_processes < n_jobs and i <= #commands_to_run do -- Start as many processes as we can
      -- Grab ith command
      local cmd = commands_to_run[i]

      local status = cmd_can_run(cmd, commands_to_run, current_processes, commands_that_ran)
      log:debugf("NXTCMD: Status: %s for command %s", status, cmd.command)

      if status == "OK" then 
        -- Remove command from list
        table.remove(commands_to_run, i)
        -- Give this command a 'job_nr' for logging/follow-up
        if cmd.job_nr then 
           --- presumably a 'retry' !!!pDock
          cmd.job_nr = cmd.job_nr .. "r" 
        else
          cmd.job_nr = job_nr
          job_nr = job_nr + 1
        end  
        cmd.process_nr = #current_processes + 1
        
        log:statusf("Command %3s/%d (%d) starting for %4s of %s (%s)", cmd.job_nr, job_total, cmd.process_nr, cmd.extension, cmd.file.relative_path, cmd.id)
        local ret, process = do_command_start(cmd)

        if ret > 0 then
          return ret,process
        else 
          log:tracef("Added process %d to current_processes",current_process)
          table.insert(current_processes, process)
          current_process = current_process + 1
        end
      elseif status == "WAIT" then
        i = i + 1
      elseif status == "SKIP" then
        log:warningf("Skipping command %s: failed dependency .", tostring(cmd.command))
        
        cmd.status = "SKIP"
        table.remove(commands_to_run, i)
        table.insert(commands_that_ran, cmd)
        
        i = i + 1
      else
        log:errorf("Status %s not implemented. Should not occur.", status)
      end
    end

    -- Check all running processes, and 'handle' the finished ones (check errrors/postporocess/resubmit)
    local j = 1
    while  j <= #current_processes do -- Check for processes that have ended
      local process = current_processes[j]
      log:tracef("Checking process %d of %s (fd=%d)", j , #current_processes, process.fd)
        
      -- log:tracef("Read up to 2024 bytes from fd %d",process.fd)
      local bytes_read = ffi.C.read(process.fd, read_buffer, 2024) -- Read 2024 bytes at a time (We currently don't do anything with what we read)
      -- log:tracef("ffi.read returns %s (%s)", bytes_read, read_buffer)

      if (bytes_read == -1) and (_errno[0] ~= EAGAIN) then -- There was some unexpected error
        log:errorf("Reading 2024 bytes from fd %d returns %s",process.fd, _errno[0])
        return tonumber(_errno[0]), "Failed to read pipe for compilation process for file: " .. tostring(process.file_name) .. ". Error code: " .. tostring(_errno[0])

      elseif bytes_read == -1 then -- _errno[0] == EAGAIN
        j = j+1     -- TODO: check ...!
      elseif bytes_read > 0 then
        local read_string = string.sub(string.gsub(ffi.string(read_buffer),"[\r\n]",""), 1, math.min(bytes_read,30)+1)
        log:tracef("Read from fd %d returns %s: %s...",process.fd, bytes_read, read_string)
        j = j+1
      else  --  bytes_read == 0 then -- End of file
        log:tracef("Zero bytes read from fd %d",process.fd)

        local ret_code = ffi.C.pclose(process.handle)
        table.remove(current_processes, j)

        if ret_code == -1 then
          log:error("Failed to get return code for compilation process for file: " .. tostring(process.file_name) .. ". Error code: " .. tostring(_errno[0]))
        elseif WIFEXITED(ret_code) == false then
          log:warningf("Process did not exit normally. return code: %s. Command: %s.", tostring(ret_code), tostring(process.cmd))
        else
          ret_code = WEXITSTATUS(ret_code)
          log:debugf("Got returncode %s for file %s", ret_code, process.file_name)
        end
          
        process.cmd.status_command = ret_code

        log:trace("Return code for compilation process for file: " .. tostring(process.file_name) .. " is ret_code: " .. tostring(ret_code))

        local compilation_time = socket.gettime() - process.start_time

        process.cmd.compilation_time = compilation_time


        local cmd = do_command_handle(process.cmd)

        log:statusf("Command %3s/%d (%d) returns %s after %.1f seconds (%d failed) for %s of %s", process.cmd.job_nr, job_total, process.cmd.process_nr, process.cmd.status, process.cmd.compilation_time, n_commands_failed, process.cmd.extension, process.cmd.file.relative_path)
    
        table.insert(commands_that_ran, cmd)
        
        -- Put the command back if we need to retry
        if cmd.status_post_command == "RETRY_COMPILATION" then
          cmd.this_is_a_retry = true
          log:infof("Command %s scheduled for RETRY (was job %s (%s))", cmd.id, cmd.job_nr , cmd.process_nr)
          table.insert(commands_to_run, 1, cmd) -- This could mess up the order a bit, but now that we check for dependencies this shouldn't be an issue
        elseif cmd.status ~= "OK" then
          log:debugf("Command %s failed (was job %s (%s))", cmd.id, cmd.job_nr , cmd.process_nr)
          n_commands_failed = n_commands_failed + 1
        end
      end

      local sleep_time = 0.5
      log:tracef("sleep %f", sleep_time)
      socket.sleep(sleep_time)
    end
  end

  local total_end_time =  socket.gettime()
  log:statusf("Finished compiling %d files in %.1f seconds", #to_be_compiled, total_end_time - total_start_time)

  -- collect and print all errors 

  if n_commands_failed > 0 then
    log:errorf("**************************")
    log:errorf("* There were %d errors: *", n_commands_failed)
    log:errorf("**************************")
  end
  local failed_commands = {}
  for _, cmd in ipairs(commands_that_ran) do 
      log:trace("File "..(cmd.output_file or "UNKNOWN??") .." got status " .. (cmd.status or 'NIL??') )
      
      if cmd.status == "SKIP"  then 
        log:errorf("[%-10s] %s: SKIPPED (%s failed)", cmd.extension, cmd.file.relative_path, cmd.errors)
      elseif cmd.status ~= "OK"  then 
        failed_commands[cmd.id] = #(cmd.errors or {})
        log:debugf("Found %d errors: %s", #(cmd.errors or {}), cmd.errors)

          for _, err in ipairs(cmd.errors or {}) do
            -- log:errorf("[%10s] %s:%s %s [%s]", compile_info.compiler, compile_info.source_file, err.line, err.context,err.error)
            log:errorf("[%-10s] %s:%s", cmd.extension, cmd.file.relative_path, err.constructed_errormessage)

          end
        if cmd.post_processing_error then
            log:errorf("[%-10s] %s: %s", "post_command", cmd.file.relative_path, cmd.post_processing_error)
            local _, n_errors = cmd.post_processing_error:gsub("\n", "")   -- HACK: number of lines equals number of errors ...
            failed_commands[cmd.id] = ( failed_commands[cmd.id] or 0 ) + n_errors + 1
        end
      end 
  end
  --
  -- all compilations done; process/summarize errors
  --
  if tablex.size(failed_commands) == 0 then
    -- log:statusf("Baked %d files, no errors found", #to_be_compiled)
    return 0,  string.format("Baked %d files, no errors found", #to_be_compiled)
  else
    log:warningf("Baked %d files, but %d compilation%s failed", #to_be_compiled, tablex.size(failed_commands), tablex.size(failed_commands) == 1 and "" or "s")

    for filename, errs in pairs(failed_commands) do
          log:debugf("Found %2d errors in %s", errs, filename)
    end
    -- log:statusf("Baking resulted in %d errors", tablex.size(failed_files) )
    return 1, string.format("Baking resulted in %d errors:\n%s", tablex.size(failed_commands), table.concat(tablex.keys(failed_commands), "\n" ))
  end
end

M.get_commands_to_run = get_commands_to_run
M.do_command_handle   = do_command_handle
M.bake         = bake
M.clean        = clean

return M
