#include <libc/include/stdio.h>

__weak void printchar(__unused struct vprintf_context *ctx, char ch) 
{
}

__weak void screen_printchar(struct vprintf_context *ctx, char ch) 
{
}

__weak int printf(const char *fmt, ...)
{
	struct vprintf_context ctx = {printchar, NULL, 0, 0, NULL};

	va_list vargs;
	va_start(vargs, fmt);
	vprintf_with_context(&ctx, fmt, vargs);
	va_end(vargs);
	return 0;
}

__weak void screen_printf(char* pszText)
{
	if (!pszText)
		return;
	while (*pszText)
		screen_printchar(NULL, *pszText++);
}
#if 0
__weak void printf_s(const char *fmt, ...)
{
#ifdef SPRINTF_RAW
	char szBuff[1024] = {0};
	va_list args;
	va_start(args, fmt);
	sprintf_raw(szBuff, fmt, args);
	va_end(args);

	screen_printf(szBuff);
#else
    struct vprintf_context ctx = {.printchar = printchar};

    va_list vargs;
    va_start(vargs, fmt);
    vprintf_with_context(&ctx, fmt, vargs);
    va_end(vargs);
#endif
}
#endif
/// Prints error message
__weak void perror(const char *pszOut)		//	(const char *fmt, ...)
{
	screen_printf(pszOut);
}
