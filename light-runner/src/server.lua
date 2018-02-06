local server = {
    data = {}
}

function server:init(port, backlog)
    self.server = uv.new_tcp()
    self.server:bind("0.0.0.0", port)
    self.server:listen(backlog, function(status)
        if status < 0 then
            print "Error on server:listen."
        end

        local client = uv.new_tcp()
        self.server:accept(client)
        self.data[client] = ""

        client:read_start(function(error, data)
            if error then
                print "Error reading data from client"
            end

            if data ~= nil then
                self.data[client] = self.data[client] .. data
            else
                runner:add_script(self.data[client])

                self.data[client] = nil

                client:shutdown()
                client:close()
            end
        end)
    end)
end

return server