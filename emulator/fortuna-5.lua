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

        function os.capture(cmd, raw)
            local f = assert(io.popen(cmd, 'r'))
            local s = assert(f:read('*a'))
            f:close()
            if raw then return s end
            s = string.gsub(s, '^%s+', '')
            s = string.gsub(s, '%s+$', '')
            s = string.gsub(s, '[\n\r]+', ' ')
            return s
        end

        local vasm = "./vasm/vasmz80_oldstyle"
        if os.capture('uname') == "Darwin" then vasm = vasm .. "_macos" end

        local stdout = os.capture(vasm .. " -chklabels -L listing.txt -Llo -ignore-mult-inc -nosym -x -Fbin -o rom.bin " .. source_file .. " 2>stderr.txt")





        -- os.remove("listing.txt")
        os.remove("stderr.txt")
        os.remove("rom.bin")
    end

}