#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "libfdbg-server.h"
#include "z80/Z80.h"

static FdbgServer server_;
static uint8_t bank = 0;
static uint8_t ram[8][64 * 1024];
static uint8_t next_op = 0;
static Z80 z80;
static uint8_t next_char = 0;

fdbg_ComputerStatus get_computer_status(FdbgServer* server)
{
    (void) server;

    fdbg_ComputerStatus status = fdbg_ComputerStatus_init_zero;

    status.pc = z80.PC.W;

    status.registers_count = 12;
    status.registers[0] = z80.AF.W;
    status.registers[1] = z80.BC.W;
    status.registers[2] = z80.DE.W;
    status.registers[3] = z80.HL.W;
    status.registers[4] = z80.AF1.W;
    status.registers[5] = z80.BC1.W;
    status.registers[6] = z80.DE1.W;
    status.registers[7] = z80.HL1.W;
    status.registers[8] = z80.IX.W;
    status.registers[9] = z80.IY.W;
    status.registers[10] = z80.SP.W;
    status.registers[11] = z80.I;

    status.flags_count = 6;
    status.flags[0] = z80.AF.W & (1 >> 7);
    status.flags[1] = z80.AF.W & (1 >> 6);
    status.flags[2] = z80.AF.W & (1 >> 4);
    status.flags[3] = z80.AF.W & (1 >> 2);
    status.flags[4] = z80.AF.W & (1 >> 1);
    status.flags[5] = z80.AF.W & (1 >> 0);

    status.stack.size = STACK_SZ;
    for (size_t i = 0; i < STACK_SZ; ++i)
        status.stack.bytes[i] = ram[bank][(uint16_t) (z80.SP.W - i)];

    return status;
}

void reset(FdbgServer* server)
{
    (void) server;

    ResetZ80(&z80);
}

void on_keypress(FdbgServer* server, const char* key)
{
    (void) server; (void) key;

    next_char = key[0];
}

uint64_t step(FdbgServer* server, bool full, fdbg_Status* status)
{
    (void) server; (void) full; (void) status;

    RunZ80(&z80);
    next_op = ram[bank][z80.PC.W];

    return z80.PC.W;
}

bool next_instruction(FdbgServer* server, ADDR_TYPE* addr)
{
    (void) server;

    static const uint8_t CALL_OPS[] = { 0xC4, 0xCC, 0xCD, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC };
    static const uint8_t RST_OPS[] = { 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF };

    for (size_t i = 0; i < sizeof(CALL_OPS); ++i) {
        if (next_op == CALL_OPS[i]) {
            *addr = z80.PC.W + 3;
            return true;
        }
    }
    for (size_t i = 0; i < sizeof(RST_OPS); ++i) {
        if (next_op == RST_OPS[i]) {
            *addr = z80.PC.W + 1;
            return true;
        }
    }

    return false;
}

bool write_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed)
{
    (void) server; (void) nr; (void) first_failed;

    for (size_t i = 0; i < sz; ++i)
        ram[bank][pos + i] = data[i];

    return true;
}

void read_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t sz, uint8_t* out_data)
{
    (void) server; (void) nr;

    for (size_t i = 0; i < sz; ++i)
        out_data[i] = ram[bank][pos + i];
}

void WrZ80(word Addr,byte Value)
{
    ram[bank][Addr] = Value;
}

byte RdZ80(word Addr)
{
    return ram[bank][Addr];
}

void OutZ80(word Port,byte Value)
{
    if (Port == 0) {
        char text[2] = { (char) Value, 0 };
        fdbg_server_terminal_print(&server_, text);
    }
}

byte InZ80(word Port)
{
    if (Port == 0) {
        uint8_t r = next_char;
        next_char = 0;
        return r;
    }
    return 0;
}

word LoopZ80(Z80 *R)
{
    (void) R;
    return INT_QUIT;
}

void PatchZ80(Z80 *R)
{
    (void) R;
}

int main()
{
    srand(time(NULL));
    for (size_t i = 0; i < 64 * 1024; ++i)
        ram[0][i] = rand();

    fdbg_server_init_pc(&server_, 0x6ab9, EMULATOR_BAUD_RATE);

    FdbgServerEvents events = {
            .get_computer_status = get_computer_status,
            .reset = reset,
            .step = step,
            .write_memory = write_memory,
            .read_memory = read_memory,
            .next_instruction = next_instruction,
            .on_keypress = on_keypress,
    };

    for (;;) {
        fdbg_server_next(&server_, &events);
        fdbg_die_if_parent_dies();
    }
}
