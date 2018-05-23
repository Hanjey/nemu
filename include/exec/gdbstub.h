#ifndef GDBSTUB_H
#define GDBSTUB_H

#define DEFAULT_GDBSTUB_PORT "1234"

/* GDB breakpoint/watchpoint types */
#define GDB_BREAKPOINT_SW        0
#define GDB_BREAKPOINT_HW        1
#define GDB_WATCHPOINT_WRITE     2
#define GDB_WATCHPOINT_READ      3
#define GDB_WATCHPOINT_ACCESS    4

#ifdef NEED_CPU_H
#include "cpu.h"

typedef void (*gdb_syscall_complete_cb)(CPUState *cpu,
                                        target_ulong ret, target_ulong err);

/**
 * gdb_do_syscallv:
 * @cb: function to call when the system call has completed
 * @fmt: gdb syscall format string
 * @va: arguments to interpolate into @fmt
 *
 * As gdb_do_syscall, but taking a va_list rather than a variable
 * argument list.
 */
void gdb_do_syscallv(gdb_syscall_complete_cb cb, const char *fmt, va_list va);
int use_gdb_syscalls(void);
void gdb_set_stop_cpu(CPUState *cpu);
void gdb_exit(CPUArchState *, int);
/* Get or set a register.  Returns the size of the register.  */
typedef int (*gdb_reg_cb)(CPUArchState *env, uint8_t *buf, int reg);

/* The GDB remote protocol transfers values in target byte order.  This means
 * we can use the raw memory access routines to access the value buffer.
 * Conveniently, these also handle the case where the buffer is mis-aligned.
 */

static inline int gdb_get_reg8(uint8_t *mem_buf, uint8_t val)
{
    stb_p(mem_buf, val);
    return 1;
}

static inline int gdb_get_reg16(uint8_t *mem_buf, uint16_t val)
{
    stw_p(mem_buf, val);
    return 2;
}

static inline int gdb_get_reg32(uint8_t *mem_buf, uint32_t val)
{
    stl_p(mem_buf, val);
    return 4;
}

static inline int gdb_get_reg64(uint8_t *mem_buf, uint64_t val)
{
    stq_p(mem_buf, val);
    return 8;
}

#if TARGET_LONG_BITS == 64
#define gdb_get_regl(buf, val) gdb_get_reg64(buf, val)
#define ldtul_p(addr) ldq_p(addr)
#else
#define gdb_get_regl(buf, val) gdb_get_reg32(buf, val)
#define ldtul_p(addr) ldl_p(addr)
#endif

#endif

int gdbserver_start(const char *port);

void gdbserver_cleanup(void);

/**
 * gdb_has_xml:
 * This is an ugly hack to cope with both new and old gdb.
 * If gdb sends qXfer:features:read then assume we're talking to a newish
 * gdb that understands target descriptions.
 */
extern bool gdb_has_xml;

/* in gdbstub-xml.c, generated by scripts/feature_to_c.sh */
extern const char *const xml_builtin[][2];

#endif
