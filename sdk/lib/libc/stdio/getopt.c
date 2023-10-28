
#include <libc/include/stdio.h>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

int getopt(int argc, char * const argv[], const char *optstring) {
	const char			*p, *opt;
	static char			const *saved_arg;
	static int			optpos;

	/* if argv[optind] is null, or *argv[optind] is not '-' or argv[optind] is "-", return -1 */
	if(optind >= argc || !(p = argv[optind]) || *p != '-' || !*++p) {
		optpos = 0;
		return -1;
	}

	/* if argv[optind] is "--", return -1 after incrementing optind */
	if(*p == '-' && !p[1]) {
		optind++;
		optpos = 0;
		return -1;
	}

	/* If different arg list, reset option position */
	if(p != saved_arg) {
		saved_arg = p;
		optpos = 0;
	}

	/* point to next option in a combined argument */
	p += optpos;

	/* Skip leading colon */
	if(*(opt = optstring) != ':') {
		opt--;
	}

	optopt = *p;
	while(*++opt) {
		if(*opt != ':' && *p == *opt) {
			/* Found a matching option */
			p++;
			if(*++opt == ':') {
				/* Option requires a operand */
				optpos = 0;
				optind++;
				if(*p) {
					/* operand is tail of current argument */
					optarg = (char *)p;
				} else {
					/* operand is the next argument */
					optarg = argv[optind++];
					if(optind > argc) {
						if(*optstring == ':') {
							/* Caller wants to know operand was missing */
							return ':';
						}
						break;
					}
				}
			} else {
				/* No operand required, so argument may hold multiple options */
				if(*p) {
					/* multiple options, remember next one */
					optpos++;
				} else {
					/* no more options, go to next argument */
					optpos = 0;
					optind++;
				}
			}
			/* Return the matching option */
			return optopt;
		}
	}

	/* There was an error, skip to next option */
	if(!*opt) {
		if(*++p) {
			optpos++;
		} else {
			optpos = 0;
			optind++;
		}
	}

	if(*optstring != ':' && opterr) {
		/* Called wants this function to display the errors as per 1003.2 4.27.6.2 */
		char				*str;
		const char			*err;
		int					errlen;

		/* Could eventually get strings from a locale */
		if(*opt) {
			err = ": " "option requires an argument" " -- ";
		} else {
			err = ": " "illegal option" " -- ";
		}
#if 0
		/* Try to allocate stack for error message */
		errlen = strlen(err);
		if((str = alloca(strlen(argv[0]) + errlen + 5))) {
			int					len;

			/* basename() can damage string, so copy it */
			strcpy(str, argv[0]);
			len = strlen(p = basename(str));
			memmove(str, p, len);

			/* Add the error message */
			strcpy(str + len, err);

			/* And the option that failed */
			str[errlen += len] = optopt;
			str[++errlen] = '\n';
			errlen++;
			err = str;
		}

		/* Do one atomic write so it looks good and doesn't pull in stdio */
		write(STDERR_FILENO, err, errlen);
#else
        printf("%s%s\n", argv[0], err);
#endif
	}

	/* Always return a question mark */
	return '?';
}
