local runner = {
    scripts = {},
    env = {
        string = string,
        table = table,
        type = type,
        tostring = tostring,
        setmetatable = setmetatable,
        ipairs = ipairs,
        next = next,
        pairs = pairs,
        print = print,
        select = select,
        tonumber = tonumber,
        unpack = unpack,
        coroutine = coroutine, 
        math = math,
    }
}

function runner:add_script(data)
    local script_data, err, chunk

    err, chunk = pcall(loadstring, data)

    if err then
        print("Error running given script. \n" .. chunk)
        return
    end

    setfenv(chunk, self.env)
    err, script_data = pcall(chunk)

    if err then
        print("Error reading script data.\n")
        return
    end

    -- TODO: check required table fields
    table.insert(self.scripts, script_data)
end

return runner