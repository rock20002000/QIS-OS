#include <libc/include/string.h>


void *memmove( void *dest, const void *src, size_t len ) {
	char		*d = dest;
	const char	*s = src;

	if( s < d && (s+len) > d ) {
		/* pointers overlapping, have to copy backwards */
		for( ;; ) {
			--len;
			if( len == (size_t)-1 ) break;
			d[len] = s[len];
		}
	} else {

	/*
     *	use optimized memcpy (duff's device based)
	 */	
     
		memcpy(dest,src,len);
	
	}
	return( dest );
}