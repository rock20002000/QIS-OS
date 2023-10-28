#include "arch.h"
#include "kernel.h"
#include "mm/buddy.h"
#include <mmu_arm32.h>

/*
 * Fault status register encodings. bit 31 for prefetch abort.
 */
#define FSR_LNX_PF		(1 << 31)
#define FSR_CM			(1 << 13)
#define FSR_WRITE		(1 << 11)
#define FSR_FS4			(1 << 10)
#define FSR_FS3_0		(15)
#define FSR_FS5_0		(0x3f)
#define FSR_FS_AEA		22
#define FS_L1_PERM		0xD
#define FS_L2_PERM		0xF

static inline uint32_t fsr_fs(uint32_t fsr)
{
	return (fsr & FSR_FS3_0) | (fsr & FSR_FS4) >> 6;
}

#ifdef ARM_PERMISSION_FAULT
static inline bool is_permission_fault(uint32_t fsr)
{
	uint32_t fs = fsr_fs(fsr);

	return (fs == FS_L1_PERM || fs == FS_L2_PERM);
}
#endif
#ifdef ARM_PAGE_WRITE_FAULT
static inline bool is_write_fault(uint32_t fsr)
{
	return (fsr & FSR_WRITE) && !(fsr & FSR_CM);
}
#endif

typedef struct {
  paddr_t pte;
  THREAD_t* th;
  iframe_t* iframe;
} fault_context_t;

typedef struct {
	error_t	(*fn)(fault_context_t* ctx, vaddr_t vaddr, uint32_t fsr, const char *name);
	const char *name;
} fsr_info_t;

static inline void dump_frame(iframe_t* iframe)
{
	KDBG("r0=%x, r1=%x, r2=%x, r3=%x, r4=%x, r5=%x, r6=%x, r7=%x, r8=%x, r9=%x, r10=%x, r11=%x, r12=%x\n",
		iframe->r0, iframe->r1, iframe->r2, iframe->r3, iframe->r4, iframe->r5, iframe->r6,
		iframe->r7, iframe->r8, iframe->r9, iframe->r10, iframe->r11, iframe->r12);
}

extern void proc_ms_info(PROCESS_t* proc);
extern mm_block_t* mm_space_find_used(mm_space_t* ms, vaddr_t start, size_t size);
static mm_block_t* get_vmem(fault_context_t* ctx, vaddr_t vaddr, uint32_t fsr, const char *name)
{
  PROCESS_t* proc = ctx->th->process;

  if (!proc)
  {
	dump_frame(ctx->iframe);
    PANIC("%s fsr:%x, fault vaddr:%p, pte:%p, sp:%p lr:%p pc:%p, th:%p(%s), no process",
        name, fsr, vaddr, ctx->pte, ctx->iframe->sp, ctx->iframe->lr, ctx->iframe->pc, ctx->th, ctx->th->name);
  }

  if (ctx->pte != arch_pte_get_entry(proc->ms.mmu))
  {
	dump_frame(ctx->iframe);
	WARN("%s fsr:%x, fault vaddr:%p, pte:%p, sp:%p lr:%p pc:%p, th:%p(%s), proc:%p, pid:%x", name, fsr,
	  vaddr, ctx->pte, ctx->iframe->sp, ctx->iframe->lr, ctx->iframe->pc, ctx->th, ctx->th->name, proc, proc->pid);
	PANIC("PTE: %p != %p", ctx->pte, arch_pte_get_entry(proc->ms.mmu));
  }

  mm_block_t* mb = mm_space_find_used(&proc->ms, vaddr, 1);

  if (!mb)
  {
    proc_ms_info(proc);
	dump_frame(ctx->iframe);
    PANIC("%s fsr:%x, invalid vaddr:%p, pte:%p, sp:%p lr:%p pc:%p, th:%p, proc:%p, pid:%x", name, fsr,
        vaddr, ctx->pte, ctx->iframe->sp, ctx->iframe->lr, ctx->iframe->pc, ctx->th, ctx->th->name, proc, proc->pid);
  }
  return mb;
}

static error_t do_bad(fault_context_t* ctx, vaddr_t vaddr, uint32_t fsr, const char *name)
{
  PROCESS_t* proc = ctx->th->process;

  dump_frame(ctx->iframe);
  PANIC("%s fsr:%x, vaddr:%p, pte:%p, sp:%p lr:%p pc:%p, th:%p(%s), proc:%p, pid:%x", name, fsr,
    vaddr, ctx->pte, ctx->iframe->sp, ctx->iframe->lr, ctx->iframe->pc, ctx->th, ctx->th->name, proc, proc->pid);

  return ERR_ABORTED;
}

extern error_t vm_alloc_map(mm_space_t* ms, vaddr_t vaddr, size_t size, paddr_t paddr, mm_block_t* mb);
static error_t do_page_fault(fault_context_t* ctx, vaddr_t vaddr, uint32_t fsr, const char *name)
{
  PROCESS_t* proc = ctx->th->process;
  mm_block_t* mb = get_vmem(ctx, vaddr, fsr, name);

  if (!mb)
	return ERR_NO_MEMORY;

  error_t err = vm_alloc_map(&proc->ms, PA_DOWN(vaddr), PAGE_SIZE, NULL_PADDR, mb);

  if (EOK != err)
  {
	PANIC("do_page_fault=>vm_alloc_map: %d, %s fsr:%x, vaddr:%p, pte:%p, sp:%p lr:%p pc:%p, th:%p(%s), proc:%p, pid:%x",
		err, name, fsr, vaddr, ctx->pte, ctx->iframe->sp, ctx->iframe->lr, ctx->iframe->pc, ctx->th, ctx->th->name, proc, proc->pid);
  }

  return err;
}

static error_t do_sect_fault(fault_context_t* ctx, vaddr_t vaddr, uint32_t fsr, const char *name)
{
  return do_page_fault(ctx, vaddr, fsr, name);
}

static error_t do_translation_fault(fault_context_t* ctx, vaddr_t vaddr, uint32_t fsr, const char *name)
{
  return do_page_fault(ctx, vaddr, fsr, name);
}

static fsr_info_t dfsr_info[] =
{
	{ do_bad,	"vector exception"},
	{ do_bad,	"alignment exception 1"},
	{ do_bad,	"terminal exception"},
	{ do_bad,	"alignment exception 3"},
	{ do_bad,	"external abort on linefetch 4"},
	{ do_translation_fault,	"section translation fault"},
	{ do_bad,	"external abort on linefetch 6"},
	{ do_page_fault,	"page translation fault"},
	{ do_bad,	"external abort on non-linefetch 8" },
	{ do_bad,	"section domain fault"},
	{ do_bad,	"external abort on non-linefetch 10" },
	{ do_bad,	"page domain fault"},
	{ do_bad,	"external abort on translation 12"},
	{ do_sect_fault,	"section permission fault"},
	{ do_bad,	"external abort on translation 14"},
	{ do_page_fault,	"page permission fault"},
	{ do_bad,	"unknown data fault 16"},
	{ do_bad,	"unknown data fault 17"},
	{ do_bad,	"unknown data fault 18"},
	{ do_bad,	"unknown data fault 19"},
	{ do_bad,	"lock abort"},
	{ do_bad,	"unknown data fault 21"},
	{ do_bad,	"imprecise external abort"},
	{ do_bad,	"unknown data fault 23"},
	{ do_bad,	"dcache parity error"},
	{ do_bad,	"unknown data fault 25"},
	{ do_bad,	"unknown data fault 26"},
	{ do_bad,	"unknown data fault 27"},
	{ do_bad,	"unknown data fault 28"},
	{ do_bad,	"unknown data fault 29"},
	{ do_bad,	"unknown data fault 30"},
	{ do_bad,	"unknown data fault 31"}
};

static fsr_info_t ifsr_info[] =
{
	{ do_bad,	"unknown prefetch fault 0"},
	{ do_bad,	"unknown prefetch fault 1"},
	{ do_bad,	"debug event"},
	{ do_bad,	"section access flag fault"},
	{ do_bad,	"unknown prefetch fault 4"},
	{ do_translation_fault,	"section translation prefetch fault"},
	{ do_bad,	"page access flag fault"},
	{ do_page_fault,	"page translation prefetch fault"},
	{ do_bad,	"external prefetch abort on non-linefetch" },
	{ do_bad,	"section domain fault"},
	{ do_bad,	"unknown prefetch fault 10"},
	{ do_bad,	"page domain prefetch fault"},
	{ do_bad,	"external prefetch abort on translation 12"},
	{ do_sect_fault,	"section permission prefetch fault"},
	{ do_bad,	"external prefetch abort on translation 14"},
	{ do_page_fault,	"page permission prefetch fault"},
	{ do_bad,	"unknown prefetch fault 16"},
	{ do_bad,	"unknown prefetch fault 17"},
	{ do_bad,	"unknown prefetch fault 18"},
	{ do_bad,	"unknown prefetch fault 19"},
	{ do_bad,	"unknown prefetch fault 20"},
	{ do_bad,	"unknown prefetch fault 21"},
	{ do_bad,	"unknown prefetch fault 22"},
	{ do_bad,	"unknown prefetch fault 23"},
	{ do_bad,	"unknown prefetch fault 24"},
	{ do_bad,	"unknown prefetch fault 25"},
	{ do_bad,	"unknown prefetch fault 26"},
	{ do_bad,	"unknown prefetch fault 27"},
	{ do_bad,	"unknown prefetch fault 28"},
	{ do_bad,	"unknown prefetch fault 29"},
	{ do_bad,	"unknown prefetch fault 30"},
	{ do_bad,	"unknown prefetch fault 31"},
};

static void handle_mmu_fault(iframe_t* iframe, uint32_t status, uint32_t vaddr, const fsr_info_t *info)
{
  fault_context_t ctx;
  
  ctx.pte = arch_pte_backup();
  ctx.th = CURRENT_THREAD;
  ctx.iframe = iframe;
  info += fsr_fs(status);
  info->fn(&ctx, vaddr, status, info->name);
  
  arch_pte_restore(ctx.pte);
}

void handle_prefetch_abort(iframe_t* iframe)
{
  uint32_t status = arm_ifsr_get();
  uint32_t vaddr = arm_ifar_get();

  handle_mmu_fault(iframe, status | FSR_LNX_PF, vaddr, ifsr_info);
}

void handle_data_abort(iframe_t* iframe)
{
  uint32_t status = arm_dfsr_get();
  uint32_t vaddr = arm_dfar_get();

  handle_mmu_fault(iframe, status & ~FSR_LNX_PF, vaddr, dfsr_info);
}

void handle_undefined_exception(iframe_t* iframe)
{
	THREAD_t* th = CURRENT_THREAD;
	PROCESS_t *proc = th->process;
	uint32_t code = *(uint32_t*)iframe->pc;

	KWARN("handle_undefined_exception: sp:%p, lr:%p, pc:%p, code:%x, thread:%p, proc:%s\n",
		iframe->sp, iframe->lr, iframe->pc, code, th, proc->name);
}
