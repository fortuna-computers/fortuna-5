-- package.cpath = package.cpath .. ";/usr/local/lib/lua5.4/?.so"
package.cpath = package.cpath .. ";../../fortuna-debugger/?.so"
local fdbg = require 'fdbg_client'

local client = fdbg.new()
client:load_user_definition("../fortuna-5.lua")
client:set_debugging_level('trace')
client:connect("/dev/cu.usbserial-1120", 115200)
client:ack(0x6ab9)

print('Ack')

function print_list(v)
    for _,v in ipairs(v) do io.write(string.format("%02X ", v)) end
    io.write("\n")
    io.flush()
end

for _ = 1,10 do
    print_list(client:read_memory(0, 0x3333, 4))
end

--[[
for _,r in ipairs(client:read_memory(0, 0, 8)) do
    io.write(r .. " ")
end
io.write("\n")

-- client:write_memory(0, 0, { 255, 200, 0 })

for _,r in ipairs(client:read_memory(0, 0, 8)) do
    io.write(r .. " ")
end
io.write("\n")

-- client:write_memory(0, 0, { 1, 2, 3 })

for _,r in ipairs(client:read_memory(0, 0, 8)) do
    io.write(r .. " ")
end
io.write("\n")
]]


print("== Done! ==")