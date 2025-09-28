--- DOM module for LuaXML
-- @module luaxml-domobject
-- @author Michal Hoftich <michal.h21@gmail.com
local dom = {}

local xml
local handler
local css_query
local html
if kpse then
  xml = require("luaxml-mod-xml")
  handler = require("luaxml-mod-handler")
  css_query = require("luaxml-cssquery")
  html = require("luaxml-mod-html")
else
  xml = require("luaxml.mod-xml")
  handler = require("luaxml.mod-handler")
  css_query = require("luaxml.cssquery")
  html = require("luaxml.mod-html")
end

local HtmlParser = html.HtmlParser

local void = {area = true, base = true, br = true, col = true, hr = true, img = true, input = true, link = true, meta = true, param = true}

-- support also upper case names
for k,v in pairs(void) do void[string.upper(k)] = true end

local escapes = {
  [">"] = "&gt;",
  ["<"] = "&lt;",
  ["&"] = "&amp;",
  ['"'] = "&quot;",
  ["'"] = "&#39;",
  ["`"] = "&#x60;"
}

-- declarations of local functions
local html_to_dom
local html_parse
local parse



local function escape(search, text)
  return text:gsub(search, function(ch)
    return escapes[ch] or ""
  end)
end

local function escape_element(text)
  return escape("([<>&])", text)
end

local function escape_attr(text)
  return escape("([<>&\"'`])", text)
end

local actions = {
  TEXT = {text = "%s"},
  COMMENT = {start = "<!-- ", text = "%s", stop = " -->"},
  ELEMENT = {start = "<%s%s>", stop = "</%s>", void = "<%s%s />"},
  DECL = {start = "<?%s %s?>"},
  PI = {start = "<?%s %s?>"},
  DTD = {start = "<!DOCTYPE ", text = "%s" , stop=">"},
  CDATA = {start = "<![CDATA[", text = "%s", stop ="]]>"}
  
}

--- It serializes the DOM object back to the XML.
-- This function is mainly used for internal purposes, it is better to
-- use the `DOM_Object:serialize()`.
-- @param parser DOM object
-- @param current Element which should be serialized
-- @param level 
-- @param output
-- @return table Table with XML strings. It can be concenated using table.concat() function to get XML string corresponding to the DOM_Object.
local function serialize_dom(parser, current,level, output)
  local output = output or {}
  local function get_action(typ, action)
    local ac = actions[typ] or {}
    local format = ac[action] or ""
    return format
  end
  local function insert(format, ...)
    table.insert(output, string.format(format, ...))
  end
  local function prepare_attributes(attr)
    local t = {}
    local attr = attr or {}
    for k, v in pairs(attr) do
      t[#t+1] = string.format("%s='%s'", k, escape_attr(v))
    end
    -- sort attributes alphabetically. this will ensure that
    -- their order will not change between several executions of dom:serialize()
    table.sort(t)
    if #t == 0 then return "" end
    -- add space before attributes
    return " " .. table.concat(t, " ")
  end
  local function start(typ, el, attr)
    local format = get_action(typ, "start")
    insert(format, el, prepare_attributes(attr))
  end
  local function text(typ, text, parent)
    local parent = parent or {}
    local format = get_action(typ, "text")
    if parent.verbatim then
      insert(format, text)
    else
      insert(format, escape_element(text))
    end
  end
  local function stop(typ, el)
    local format = get_action(typ, "stop")
    insert(format,el)
  end
  local level = level or 0
  local spaces = string.rep(" ",level)
  local root= current or parser._handler.root
  local name = root._name or "unnamed"
  local xtype = root._type or "untyped"
  local text_content = root._text or ""
  local attributes = root._attr or {}
  -- if xtype == "TEXT" then
  --   print(spaces .."TEXT : " .. root._text)
  -- elseif xtype == "COMMENT" then
  --   print(spaces .. "Comment : ".. root._text)
  -- else
  --   print(spaces .. xtype .. " : " .. name)
  -- end
  -- for k, v in pairs(attributes) do
  --   print(spaces .. " ".. k.."="..v)
  -- end
  if xtype == "DTD" then
    text_content = string.format('%s %s "%s" "%s"', name, attributes["_type"] or "",  attributes._name, attributes._uri )
    -- remove unused fields
    text_content = text_content:gsub('"nil"','')
    text_content = text_content:gsub('%s*$','')
    attributes = {}
  elseif xtype == "ELEMENT" and void[name] and #current._children < 1 then
    local format = get_action(xtype, "void")
    insert(format, name, prepare_attributes(attributes))
    return output
  elseif xtype == "PI" then
    -- it contains spurious _text attribute
    attributes["_text"] = nil
  elseif xtype == "DECL" and name =="xml" then
    -- the xml declaration attributes must be in a correct order
    local encoding = attributes.encoding or "utf-8"
    insert("<?xml version='%s' encoding='%s' ?>", attributes.version, encoding)
    return output
  elseif xtype == "CDATA" then
    -- return content unescaped
    insert("<![CDATA[%s]]>", text_content)
    return output
  end

  start(xtype, name, attributes)
  text(xtype,text_content, (current or {})._parent) 
  local children = root._children or {}
  for _, child in ipairs(children) do
    output = serialize_dom(parser,child, level + 1, output)
  end
  stop(xtype, name)
  return output
end

--- XML parsing function
-- Parse the XML text and create the DOM object.
-- @return DOM_Object
parse = function(
  xmltext --- String to be parsed
  ,voidElements --- hash table with void elements
 )
  local domHandler = handler.domHandler()
  ---  @type DOM_Object
  local DOM_Object = xml.xmlParser(domHandler)
  -- preserve whitespace
  DOM_Object.options.stripWS = nil
  -- don't try to expand entities
  DOM_Object.options.expandEntities = nil
  local voidElements = voidElements or void
  DOM_Object._handler.options.voidElements = voidElements
  DOM_Object:parse(xmltext)
  DOM_Object.current = DOM_Object._handler.root
  DOM_Object.__index = DOM_Object
  DOM_Object.css_query = css_query()

  local function save_methods(element)
    setmetatable(element,DOM_Object)
    local children = element._children or {}
    for _, x in ipairs(children) do
      save_methods(x)
    end
  end
  local parser = setmetatable({}, DOM_Object)

  --- Returns root element of the DOM_Object 
  -- @return DOM_Object 
  function DOM_Object:root_node()
    return self._handler.root
  end


  --- Get current node type
  -- @param  el [optional] node to get the type of
  function DOM_Object:get_node_type( 
    el --- [optional] element to test
    )
    local el = el or self
    return el._type
  end

  --- Test if the current node is an element.
  -- You can pass different element as parameter
  -- @return boolean
  function DOM_Object:is_element(
    el --- [optional] element to test
    )
    local el = el or self
    return self:get_node_type(el) == "ELEMENT" -- @bool
  end

  
  --- Test if current node is text
  -- @return boolean
  function DOM_Object:is_text(
    el --- [optional] element to test
    )
    local el = el or self
    return self:get_node_type(el) == "TEXT"
  end

  local lower = string.lower

  --- Return name of the current element
  -- @return string
  function DOM_Object:get_element_name(
    el --- [optional] element to test
    )
    local el = el or self
    return el._name or "unnamed"
  end

  --- Get value of an attribute
  -- @return string
  function DOM_Object:get_attribute(
    name --- Attribute name
    )
    local el = self
    if self:is_element(el) then
      local attr = el._attr or {}
      return attr[name]
    end
  end

  --- Set value of an attribute
  -- @return boolean
  function DOM_Object:set_attribute( 
    name --- Attribute name
    , value --- Value to be set
    )
    local el = self
    if self:is_element(el) then
      el._attr = el._attr or {}
      el._attr[name] = value
      return true
    end
  end
  

  --- Serialize the current node back to XML
  -- @return string
  function DOM_Object:serialize(
    current --- [optional] element to be serialized
    )
    local current = current
    -- if no current element is added and self is not plain parser object
    -- (_type is then nil), use the current object as serialized root
    if not current and self._type then
      current = self
    end
    return table.concat(serialize_dom(self, current))
  end

  --- Get text content from the node and all of it's children
  -- @return string
  function DOM_Object:get_text(
    current --- [optional] element which should be converted to text
    )
    local current = current or self
    local text = {}
    if current:is_text() then return current._text or "" end
    for _, el in ipairs(current:get_children()) do
      if el:is_text() then
        text[#text+1] = el._text or ""
      elseif  el._type == "CDATA" then
        text[#text+1] = el._text or ""
      elseif el:is_element() then
        text[#text+1] = el:get_text()
      end
    end
    return table.concat(text)
  end



  --- Retrieve elements from the given path.
  -- The path is list of elements separated by space,
  -- starting from the top element of the current element
  -- @return table of elements which match the path
  function DOM_Object:get_path(
    path --- path to be traversed
    , current --- [optional] element which should be traversed. Default element is the root element of the DOM_Object
    )
    local function traverse_path(path_elements, current, t)
      local t = t or {}
      if #path_elements == 0 then 
        -- for _, x in ipairs(current._children or {}) do
          -- table.insert(t,x)
        -- end
        table.insert(t,current)
        return t
      end
      local current_path = table.remove(path_elements, 1)
      for _, x in ipairs(self:get_children(current)) do
        if self:is_element(x) then
          local name = string.lower(self:get_element_name(x))
          if name == current_path then
            t = traverse_path(path_elements, x, t)
          end
        end
      end
      return t
    end
    local current = current or self:root_node() -- self._handler.root
    local path_elements = {}
    local path = string.lower(path)
    for el in path:gmatch("([^%s]+)") do table.insert(path_elements, el) end
    return traverse_path(path_elements, current)
  end

  --- Select elements chidlren using CSS selector syntax
  -- @return table with elements matching the selector.
  function DOM_Object:query_selector(
    selector --- String using the CSS selector syntax
    )
    local css_query = self.css_query
    local css_parts = css_query:prepare_selector(selector)
    return css_query:get_selector_path(self, css_parts)
  end

  --- Get table with children of the current element
  -- @return table with children of the selected element
  function DOM_Object:get_children(
    el --- [optional] element to be selected
    )
    local el  = el or self
    local children = el._children or {}
    return children
  end

  --- Get the parent element
  -- @return DOM_Object parent element
  function DOM_Object:get_parent( 
    el --- [optional] element to be selected
    )
    local el = el or self
    return el._parent
  end

  --- Execute function on the current element and all it's children nodes.
  -- The differenct to DOM_Object:traverse_elements() is that it executes the function 
  -- also on text nodes and all other kinds of XML nodes.
  -- The traversing of child elements of a given node can be disabled when the executed
  -- function returns false.
  function DOM_Object:traverse(
    fn, --- function which will be executed on the current element and all it's children
    current --- [optional] element to be selected
    )
    local current = current or self --
    -- Following situation may happen when this method is called directly on the parsed object
    if not current:get_node_type() then
      current = self:root_node() 
    end
    local status = true
    local status = fn(current)
    if current:is_element() or current:get_node_type() == "ROOT" then
      -- don't traverse child nodes when the user function return false
      if status ~= false then
        for _, child in ipairs(current:get_children()) do
          self:traverse(fn, child)
        end
      end
    end
  end

  --- Execute function on the current element and all it's children elements.
  --- The traversing of child elements of a given node can be disabled when the executed
  -- function returns false.
  -- @return nothing
  function DOM_Object:traverse_elements(
    fn, --- function which will be executed on the current element and all it's children
    current --- [optional] element to be selected
    )
    local current = current or self --
    current:traverse(function(node)
      if node:is_element() or node:get_node_type() == "ROOT" then
        fn(node)
      end
    end)
  end

  --- Get table with the inner text of an element, every text node is a separate table item. 
  --- @return table
  function DOM_Object:strings(
    current --- [optional] element to be selected
    )
    local strings = {}
    local current = current or self
    current:traverse(function(node)
      if node:get_node_type() == "TEXT" then
        table.insert(strings, node._text or "")
      end
    end)
    return strings
  end

  --- Get table with the inner text of an element - leading and trailing spaces are removed and elements that contain only white space are ignored.
  --  @return table 
  function DOM_Object:stripped_strings(
    current --- [optional] element to be selected
    )
    local current = current or self
    local strings = current:strings()
    local cleaned = {}
    for k,v in ipairs(strings) do
      v = v:gsub("^%s*", ""):gsub("%s*$", "")
      if v ~= "" then
        table.insert(cleaned, v)
      end
    end
    return cleaned
  end




  --- Execute function on list of elements returned by DOM_Object:get_path()
  function DOM_Object:traverse_node_list( 
    nodelist --- table with nodes selected by DOM_Object:get_path()
    , fn --- function to be executed
    )
    local nodelist = nodelist or {}
    for _, node in ipairs(nodelist) do
      for _, element in ipairs(node._children) do
        fn(element)
      end
    end
  end

  --- Replace the current node with new one
  -- @return boolean, message
  function DOM_Object:replace_node(
     new --- element which should replace the current element
    )
    local old = self
    local parent = self:get_parent(old)
    local id,msg = self:find_element_pos( old)
    if id then
      parent._children[id] = new
      return true
    end
    return false, msg
  end

  -- restore correct links to parent elements
  local function fix_parents(el)
    for k,v in ipairs(el._children or {}) do
      if v:is_element() then
        v._parent = el
        fix_parents(v)
      end
    end
  end


  --- Add child node to the current node
  function DOM_Object:add_child_node( 
    child, --- element to be inserted as a current node child
    position --- [optional] position at which should the node be inserted
    )
    local parent = self
    child._parent = parent
    fix_parents(child)
    if position then
      table.insert(parent._children, position, child)
    else
      table.insert(parent._children, child)
    end
  end


  --- Create copy of the current node
  -- @return DOM_Object element
  function DOM_Object:copy_node( 
    element --- [optional] element to be copied
    )
    local element = element or self
    local t = {}
    for k, v in pairs(element) do
      if type(v) == "table" and k~="_parent" then
        t[k] = self:copy_node(v)
      else
        t[k] = v
      end
    end
    save_methods(t)
    return t
  end


  --- Create a new element
  -- @return DOM_Object element
  function DOM_Object:create_element(
    name, -- New tag name
    attributes, -- Table with attributes
    parent -- [optional] element which should be saved as the element's parent
    )
    local parent = parent or self
    local new = {}
    new._type = "ELEMENT"
    new._name = name
    new._attr = attributes or {}
    new._children = {}
    new._parent = parent
    save_methods(new)
    return new
  end

  --- Create new text node
  -- @return DOM_Object text object
  function DOM_Object:create_text_node( 
    text, -- string
    parent -- [optional] element which should be saved as the element's parent
    )
    local parent = parent or self
    local new = {}
    new._type = "TEXT"
    new._parent = parent
    new._text = text
    save_methods(new)
    return new
  end

  --- Delete current node
  function DOM_Object:remove_node(
    element -- [optional] element to be removed
    )
    local element = element or self
    local parent = self:get_parent(element)
    local pos = self:find_element_pos(element)
    -- if pos then table.remove(parent._children, pos) end
    if pos then 
      -- table.remove(parent._children, pos) 
      parent._children[pos] = setmetatable({_type = "removed"}, DOM_Object)
    end
  end

  --- Find the element position in the current node list
  -- @return integer position of the current element in the element table
  function DOM_Object:find_element_pos(
    el -- [optional] element which should be looked up
    )
    local el = el or self
    local parent = self:get_parent(el)
    if not self:is_element(parent) and self:get_node_type(parent) ~= "ROOT" then return nil, "The parent isn't element" end
    for i, x in ipairs(parent._children) do
      if x == el then return i end
    end
    return false, "Cannot find element"
  end

  --- Get node list which current node is part of
  -- @return table with elements
  function DOM_Object:get_siblings(
    el -- [optional] element for which the sibling element list should be retrieved
    )
    local el = el or self
    local parent = el:get_parent()
    if parent:is_element() then
      return parent:get_children()
    end
  end

  --- Get sibling node of the current node
  -- @param change Distance from the current node
  -- @return DOM_Object node
  function DOM_Object:get_sibling_node( change)
    local el = self
    local pos = el:find_element_pos()
    local siblings = el:get_siblings()
    if pos and siblings then
      return siblings[pos + change]
    end
  end

  --- Get next node
  -- @return DOM_Object node
  function DOM_Object:get_next_node(
    el --- [optional] node to be used
    )
    local el = el or self
    return el:get_sibling_node(1)
  end

  --- Get previous node
  -- @return DOM_Object node
  function DOM_Object:get_prev_node(
    el -- [optional] node to be used
    )
    local el = el or self
    return el:get_sibling_node(-1)
  end

  --- parse string as HTML or XML and return created elements
  --- @return table elements
  function DOM_Object:create_template(
    str,
    is_xml
    )
    -- <> is a dummy element, we just need to wrap everything in some element 
    str = "<>" .. (str or "") .. "</>"
    local template = is_xml and parse(str) or parse(str)
    local root = template:root_node()._children[1]
    return root
  end

  --- parse string as HTML or XML and insert it as a child of the current node
  function DOM_Object:inner_html(
    str, --- HTML or XML to be inserted
    is_xml --- [optional] Pass true to parse as XML, otherwise parse as HTML
  )
    local el = self
    local root = self:create_template(str, is_xml)
    -- replace original children of the current element with children of the dummy element created by parsing
    el._children = root._children
    return el
  end


  ---  parse string as HTML or XML and insert it before current the element
  function DOM_Object:insert_before_begin(
    str, --- HTML or XML to be inserted
    is_xml --- [optional] Pass true to parse as XML, otherwise parse as HTML
  )
    local el = self 
    local root = self:create_template(str, is_xml)
    local parent = el:get_parent()
    local current_pos = el:find_element_pos()
    local children = root:get_children()
    for i = 1,  #children do
      parent:add_child_node(children[i], current_pos + i - 1)
    end
  end

  ---  parse string as HTML or XML and insert it at the beginning of the current the element
  function DOM_Object:insert_after_begin(
    str, --- HTML or XML to be inserted
    is_xml --- [optional] Pass true to parse as XML, otherwise parse as HTML
  )
    local el = self 
    local root = self:create_template(str, is_xml)
    local children = root:get_children()
    for i = 1,  #children do
      el:add_child_node(children[i], i)
    end
  end

  ---  parse string as HTML or XML and insert it at the end of the current the element
  function DOM_Object:insert_before_end(
    str, --- HTML or XML to be inserted
    is_xml --- [optional] Pass true to parse as XML, otherwise parse as HTML
  )
    local el = self 
    local root = self:create_template(str, is_xml)
    local children = root:get_children()
    for i = 1,  #children do
      el:add_child_node(children[i])
    end
  end

  ---  parse string as HTML or XML and insert it after current the element
  function DOM_Object:insert_after_end(
    str, --- HTML or XML to be inserted
    is_xml --- [optional] Pass true to parse as XML, otherwise parse as HTML
    )
    local el = self
    local root = self:create_template(str, is_xml)
    local parent = el:get_parent()
    local current_pos = el:find_element_pos()
    local children = root:get_children()
    for i = 1,  #children do
      parent:add_child_node(children[i], current_pos + i)
    end
  end


  -- include the methods to all xml nodes
  save_methods(parser._handler.root)
  -- parser:
  return parser, DOM_Object
end

-- table of elements that should be kept without XML escaping in the DOM serialization
local verbatim_elements = {script=true, style=true}

function html_to_dom(html_object)
  -- convert parsed HTML DOM to the XML DOM
  local dom, DOM_Object = parse("") -- use empty text to just initialize the DOM object
  -- use root of the DOM object as the original parent
  local current_parent = dom._handler.root

  local function create_node(tbl)
    -- create node suitable for LuaXML DOM object
    tbl._children = {}
    -- this should copy methods from the DOM object to the newly created object
    tbl.__index = DOM_Object
    return setmetatable(tbl, DOM_Object)
  end

  local function build_tree(object)
    -- convert tree produced by the HTML parser to LuaXML DOM 
    local typ = object._type 
    -- process particular node types from the HTML parser
    if typ == "doctype" then
      current_parent:add_child_node(create_node {_name=object.name, _type="DTD"})
    elseif typ == "comment" then
      current_parent:add_child_node(create_node {_text=object.text, _type="COMMENT"})
    elseif typ == "element" then
      local attributes = {}
      -- convert attributes to the form expected by the DOM object
      for _, attr in ipairs(object.attr) do 
        attributes[attr.name] = attr.value
      end
      local element = current_parent:create_element(object.tag, attributes)
      -- disable escaping of text in dom:serialize() for <script> or <style> elements
      if verbatim_elements[string.lower(object.tag)] then element.verbatim = true end
      current_parent:add_child_node(element)
      -- set the current element as parent for the processing of children
      local old_parent = current_parent
      current_parent = element
      -- process children
      for k,v in ipairs(object.children) do
        build_tree(v)
      end
      -- restore original parent
      current_parent = old_parent
    elseif typ == "text" then
      local text = current_parent:create_text_node(object.text)
      current_parent:add_child_node(text)
    else
      -- for other node types, just process the children
      for k,v in ipairs(object.children) do
        build_tree(v)
      end
    end

  end
  build_tree(html_object)
  return dom
end

--- Parse HTML text as a DOM object.
--  It supports all methods as the object returned by the parse() function.
-- @param html_str  string with the HTML code to be parsed
-- @return DOM_Object
function html_parse(html_str)
  local html_obj = HtmlParser:init(html_str)
  local html_dom = html_obj:parse()
  return html_to_dom(html_dom)
end



--- @export
return {
  parse = parse, 
  serialize_dom= serialize_dom,
  html_parse = html_parse
}
