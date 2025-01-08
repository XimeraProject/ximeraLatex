local M = {}
-- load and run a script in the provided environment
-- returns the modified environment table


-- https://stackoverflow.com/a/69910551
--- run Lua file and return it's environment table
--- @param scriptfile string script file name
--- @param config config configuration table
--- @return table env script environment table
local function run_test_script(scriptfile, config)
  local env = setmetatable({}, {__index=config})
  assert(pcall(loadfile(scriptfile,"run_test_script",env)))
  setmetatable(env, nil)
  return env
end


-- https://stackoverflow.com/a/7470789
--- merge tables
---@param t1 table
---@param t2 table
---@return table merged
local function merge(t1, t2)
  for k, v in pairs(t2) do
    if (type(v) == "table") and (type(t1[k] or false) == "table") then
      merge(t1[k], t2[k])
    else
      t1[k] = v
    end
  end
  return t1
end



--- update config table from script
--- @param scriptfile string filename 
--- @param config config table
--- @return config configuration table
local function update_config(scriptfile, config)
  local env =  run_test_script(scriptfile, config)
  return merge(config, env)
end


M.update_config = update_config

return M
