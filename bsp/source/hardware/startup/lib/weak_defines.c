#include "startup.h"


__weak void board_init(syspage_t* p_syspage)
{

}

__weak void crash_done(void)
{
	/* hang forever */
	for( ;; ) {}
}

