/*! \file
    \brief Header of the neoip_fnmatch
 
\par Brief Description
this is just a workaround because fnmatch is not provided by mingw32.
So a netbsd implementation takes over IIF _WIN32 is defined.
- just a few modification to make it easy to compile in c++/mingw32
    
*/


#include "neoip_fnmatch.hpp"

NEOIP_CPP_EXTERN_C_BEGIN
#ifdef _WIN32

/*	$NetBSD: fnmatch.c,v 1.21 2005/12/24 21:11:16 perry Exp $	*/

/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Guido van Rossum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

//#include <sys/cdefs.h>

/*
 * Function fnmatch() as specified in POSIX 1003.2-1992, section B.6.
 * Compares a filename or pathname to a pattern.
 */

//#include "namespace.h"

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define FNM_PATHNAME    (1 << 0) /* No wildcard can ever match `/'.  */
#define FNM_NOESCAPE    (1 << 1) /* Backslashes don't quote special chars.  */
#define FNM_PERIOD      (1 << 2) /* Leading `.' is matched only explicitly.  */

# define FNM_FILE_NAME   FNM_PATHNAME   /* Preferred GNU name.  */
# define FNM_LEADING_DIR (1 << 3)       /* Ignore `/...' after a match.  */
# define FNM_CASEFOLD    (1 << 4)       /* Compare without regard to case.  */
# define FNM_EXTMATCH    (1 << 5)       /* Use ksh-like extended matching. */

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define FNM_NOMATCH     1

#include <assert.h>
#include <ctype.h>
//#include <fnmatch.h>
#include <string.h>

#ifdef __weak_alias
__weak_alias(fnmatch,_fnmatch)
#endif

#define	EOS	'\0'

static const char *rangematch (const char *, int, int);

static inline int foldcase(int ch, int flags)
{

	if ((flags & FNM_CASEFOLD) != 0 && isupper(ch))
		return (tolower(ch));
	return (ch);
}

#define	FOLDCASE(ch, flags)	foldcase((unsigned char)(ch), (flags))

int fnmatch(const char *pattern, const char *string, int flags)
{
	const char *stringstart;
	char c, test;

//	_DIAGASSERT(pattern != NULL);
//	_DIAGASSERT(string != NULL);

	for (stringstart = string;;)
		switch (c = FOLDCASE(*pattern++, flags)) {
		case EOS:
			if ((flags & FNM_LEADING_DIR) && *string == '/')
				return (0);
			return (*string == EOS ? 0 : FNM_NOMATCH);
		case '?':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && (flags & FNM_PATHNAME))
				return (FNM_NOMATCH);
			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);
			++string;
			break;
		case '*':
			c = FOLDCASE(*pattern, flags);
			/* Collapse multiple stars. */
			while (c == '*')
				c = FOLDCASE(*++pattern, flags);

			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);

			/* Optimize for pattern with * at end or before /. */
			if (c == EOS) {
				if (flags & FNM_PATHNAME)
					return ((flags & FNM_LEADING_DIR) ||
					    strchr(string, '/') == NULL ?
					    0 : FNM_NOMATCH);
				else
					return (0);
			} else if (c == '/' && flags & FNM_PATHNAME) {
				if ((string = strchr(string, '/')) == NULL)
					return (FNM_NOMATCH);
				break;
			}

			/* General case, use recursion. */
			while ((test = FOLDCASE(*string, flags)) != EOS) {
				if (!fnmatch(pattern, string,
					     flags & ~FNM_PERIOD))
					return (0);
				if (test == '/' && flags & FNM_PATHNAME)
					break;
				++string;
			}
			return (FNM_NOMATCH);
		case '[':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && flags & FNM_PATHNAME)
				return (FNM_NOMATCH);
			if ((pattern =
			    rangematch(pattern, FOLDCASE(*string, flags),
				       flags)) == NULL)
				return (FNM_NOMATCH);
			++string;
			break;
		case '\\':
			if (!(flags & FNM_NOESCAPE)) {
				if ((c = FOLDCASE(*pattern++, flags)) == EOS) {
					c = '\\';
					--pattern;
				}
			}
			/* FALLTHROUGH */
		default:
			if (c != FOLDCASE(*string++, flags))
				return (FNM_NOMATCH);
			break;
		}
	/* NOTREACHED */
}

static const char * rangematch(const char *pattern, int test, int flags)
{
	int negate, ok;
	char c, c2;

//	_DIAGASSERT(pattern != NULL);

	/*
	 * A bracket expression starting with an unquoted circumflex
	 * character produces unspecified results (IEEE 1003.2-1992,
	 * 3.13.2).  This implementation treats it like '!', for
	 * consistency with the regular expression syntax.
	 * J.T. Conklin (conklin@ngai.kaleida.com)
	 */
	if ((negate = (*pattern == '!' || *pattern == '^')) != 0)
		++pattern;
	
	for (ok = 0; (c = FOLDCASE(*pattern++, flags)) != ']';) {
		if (c == '\\' && !(flags & FNM_NOESCAPE))
			c = FOLDCASE(*pattern++, flags);
		if (c == EOS)
			return (NULL);
		if (*pattern == '-' 
		    && (c2 = FOLDCASE(*(pattern+1), flags)) != EOS &&
		        c2 != ']') {
			pattern += 2;
			if (c2 == '\\' && !(flags & FNM_NOESCAPE))
				c2 = FOLDCASE(*pattern++, flags);
			if (c2 == EOS)
				return (NULL);
			if (c <= test && test <= c2)
				ok = 1;
		} else if (c == test)
			ok = 1;
	}
	return (ok == negate ? NULL : pattern);
}

#endif // _WIN32
NEOIP_CPP_EXTERN_C_END
