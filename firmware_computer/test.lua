package.cpath = package.cpath .. ";/usr/local/lib/lua5.4/?.so"
local fdbg = require 'fdbg_client'

local client = fdbg.new()
client:set_debugging_level('trace')
client:connect("/dev/cu.usbserial-1120", 9600)
os.execute("sleep 1")
client:ack(0x6ab9)