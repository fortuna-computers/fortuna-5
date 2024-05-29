return {

    --
    -- Machine definitions
    --
    machine = {

        -- Machine ID. Can by any 16-bit value, it's used to check if the debugger is
        -- connected to the right emulator. Needs to match the emulator id.
        id = 0x6ab9,

        -- Machine name
        name = "Fortuna-5"

    },

    --
    -- Debugger definitions
    --

    debugger = {

        -- Source code comment separators. Used for syntax highlighting.
        comment_separators = ";#",

        -- List of memories. Usually just the RAM, but it can be used for things like
        -- a SDCard or a hard drive. The size is the full addressable memory space, and not
        -- just the physical size.
        memories = {
            { name = "RAM", size = 64 * 1024 },
        },

        -- List of registers. The register count and order must match the list of register
        -- values that is returned by the emulator/computer.
        registers = {
            { name = "AF", size = 16 },
            { name = "BC", size = 16 },
            { name = "DE", size = 16 },
            { name = "HL", size = 16 },
            { name = "AF'", size = 16 },
            { name = "BC'", size = 16 },
            { name = "DE'", size = 16 },
            { name = "HL'", size = 16 },
            { name = "IX", size = 16 },
            { name = "IY", size = 16 },
            { name = "SP", size = 16 },
            { name = "I", size = 8 },
        },

        -- List of boolean flags. The flag count and order must match the list of flags
        -- that is returned by the emulator/computer.
        flags = { "S", "Z", "H", "P/V", "N", "C" },

        -- The terminal size
        terminal = { lines = 25, columns = 80 },

        -- These fields are used to setup the cycle interface - when we want to debug
        -- CPU cycles.
        --[[
        cycle_bytes = {
            { name = "ADDR", size = 16 },
            { name = "DATA", size = 8 },
        },
        cycle_bits = { "R", "XY" },
        ]]
    },

    --
    -- Options used to setup default microcontroller settings.
    --
    --[[
    microcontroller = {
        uart_baud_rate = 115200,   -- default UART baud rate
        vendor_id = "2e8a",        -- used to autodetect the USB serial port
        product_id = "000a",
    },
    ]]

    --
    -- Compiler. In this case we're compiling from Lua, but it can also be used to call
    -- an external compiler and then parse the result. The result needs to be in the
    -- following format:
    --
    -- {
    --     success = true/false,
    --     result_info = "Any relevant compiler output (not errors)",
    --     error = "Error - if the compilation was not successful",
    --     files = { "List", "of", "source", "files", "parsed" },
    --     source_lines = {
    --         { line = "Each source line", file_idx=1, line_number=1, address=0x0 },
    --         ...
    --     },
    --     symbols = {
    --         { name = "symbol", address = 0x40 }
    --         ...
    --     },
    --     binaries = {
    --        { rom = { 61, 62, 63 }, load_pos = 30 },
    --     },
    --     files_to_watch = { "List of files that the debugger will watch for changes" }
    -- }
    --
    compile = function(source_file)

        local path = source_file:match("(.*/)")

        function os.capture(cmd, raw)
            local f = assert(io.popen(cmd, 'r'))
            local s = assert(f:read('*a'))
            local status = f:close()
            if raw then return s end
            s = string.gsub(s, '^%s+', '')
            s = string.gsub(s, '%s+$', '')
            s = string.gsub(s, '[\n\r]+', ' ')
            return s, status
        end

        -- run compiler

        local vasm = "./vasm/vasmz80_oldstyle"
        if os.capture('uname') == "Darwin" then vasm = vasm .. "_macos" end

        local stdout, status = os.capture(vasm .. " -chklabels -L /tmp/listing.txt -Llo -ignore-mult-inc -nosym -x -Fbin -o /tmp/rom.bin " .. source_file .. " 2>/tmp/stderr.txt")

        -- get compiler output

        local ret = {
            source_lines = {},
            symbols = {},
        }
        ret.success = status
        ret.result_info = stdout

        local f = assert(io.open("/tmp/stderr.txt", "r"))
        ret.error = f:read("*all")
        f:close()

        if status then

            -- read generated binary

            f = assert(io.open("/tmp/rom.bin", "rb"))
            local rom = f:read("*all")
            local bytes = {}
            for c in (rom or ''):gmatch('.') do
                bytes[#bytes+1] = c:byte()
            end
            ret.binaries = { { rom = bytes, load_pos = 0 } }

            -- read listing file

            local files = {}
            local current_file = ''
            local current_file_idx = 0

            f = assert(io.open("/tmp/listing.txt", "r"));
            for line in f:lines() do

                -- new file?
                local source = line:match('^Source: "(.+)"')
                if source then
                    current_file = source
                    -- find file index
                    current_file_idx = 0
                    for i,v in ipairs(files) do
                        if v == source then current_file_idx = i end
                    end
                    if current_file_idx == 0 then
                        table.insert(files, source)
                        current_file_idx = #files
                    end
                end

                -- source line
                local addr, line_nr, text = line:match("^%x+:(%x+)%s+%x*%s+(%d+):(.+)")
                if not addr then
                    line_nr, text = line:match("^%s+(%d+):(.+)")
                end
                if line_nr then
                    local s = { line = text, line_number = tonumber(line_nr), file_idx = current_file_idx }
                    if addr then s.address = tonumber(addr, 16) end
                    table.insert(ret.source_lines, s)
                end

                -- symbols
                local addr, symbol = line:match("^(%x+)%s+(.*)")
                if addr then
                    table.insert(ret.symbols, { name = symbol, address = tonumber(addr, 16) })
                end

            end
            f:close()

            -- files
            ret.files_to_watch = {}
            for _,v in ipairs(files) do
                if string.find(v, "/") then
                    table.insert(ret.files_to_watch, v)
                else
                    table.insert(ret.files_to_watch, path .. v:match("([^/]+)$"))
                end
            end
            ret.files = {}
            for _,v in ipairs(files) do
                table.insert(ret.files, v:match("([^/]+)$"))
            end

        end

        -- remove generated files

        os.remove("listing.txt")
        os.remove("stderr.txt")
        os.remove("rom.bin")

        --[[
        local function dump(o)
            if type(o) == 'table' then
                local s = '{ '
                for k,v in pairs(o) do
                    if type(k) ~= 'number' then k = '"'..k..'"' end
                    s = s .. '['..k..'] = ' .. dump(v) .. ','
                end
                return s .. '} '
            else
                return tostring(o)
            end
        end
        print(dump(ret))
        ]]

        return ret
    end

}