/* ------------ License
License:        Blood Rose 4 Clause License v1.0
Licensor:       Blood Rose <https://bloodrose.org>
A full copy of the license terms is available at the end of this source file */

// ------------	Usage
/*
	At the beginning of each function,
	- debug_fn(FUNCTION_NAME);
	Replace normal returns with
	- debug_return(VALUE);
	Insert try/catch-like control flow with
	- debug_catch(fail_condition,
		ok_message, fail_message, { failure });
*/
// ------------	Function Signatures

void            debug_fn        (char *);
void            debug_ok        (const char *);
void            debug_error     (const char *);
void            debug_panic     (const char *);
bool            debug_try       (bool, const char *, const char *);
void			debug_pop		(void);
void			debug_pad		(void);

// ------------	Implementation

#ifndef DEBUG_H
#define DEBUG_H

#define debug_catch(CND, OKMSG, FAILMSG, RET) { \
	if (!(CND)) { debug_ok(OKMSG); } \
	else 		{ debug_error(FAILMSG); RET; }}

#define debug_return(RET) { \
	debug_pop(); \
	debug_pad(); \
	printf(DEBUG_DENOUNCE, DEBUG_NAME[DEBUG_INDEX+1], STR(RET)); \
	return RET; }

#define DEBUG_MAX_DEPTH			512
#define DEBUG_RED               "\x1B\x5B\x30\x3b\x33\x31\x6D"
#define DEBUG_GREEN             "\x1B\x5B\x30\x3b\x33\x32\x6D"
#define DEBUG_PURPLE            "\x1B\x5B\x30\x3b\x33\x35\x6D"
#define DEBUG_WHITE             "\x1B\x5B\x30\x6D"
#define DEBUG_ANNOUNCE			DEBUG_PURPLE "enter fn " DEBUG_GREEN "%s" DEBUG_WHITE "\n"
#define DEBUG_DENOUNCE			DEBUG_PURPLE "%s " DEBUG_WHITE "returned " DEBUG_GREEN "(%s)" DEBUG_WHITE "\n"
#define DEBUG_ERR               DEBUG_RED "[ERR] " DEBUG_WHITE "%s\n"
#define DEBUG_OK                DEBUG_GREEN "[OK]  " DEBUG_WHITE "%s\n"
#define DEBUG_NULL				"\0"
#define DEBUG_PADDING			"    "

char * 			DEBUG_NAME [DEBUG_MAX_DEPTH];
int				DEBUG_INDEX = -1;

void            debug_ok        (const char * str) { debug_pad(); printf(DEBUG_OK, str); }
void            debug_error     (const char * str) { debug_pad(); printf(DEBUG_ERR, str); }
void            debug_panic     (const char * str) { debug_error(str); exit(1); }

void            debug_fn        (char * str)
{
	DEBUG_INDEX++;
	if ( DEBUG_INDEX >= DEBUG_MAX_DEPTH ) { debug_panic("Exceeded maximum debug stack depth"); }
	DEBUG_NAME[DEBUG_INDEX] = str;
	debug_pad(); printf(DEBUG_ANNOUNCE, DEBUG_NAME[DEBUG_INDEX]);
}

void			debug_pop		(void)
{
	if (DEBUG_INDEX < 0) { debug_panic("Returning past start of debug stack"); }
	DEBUG_INDEX--;
}

bool            debug_try       (bool state, const char *good, const char *evil)
{
	if (state) { debug_ok(good); }
	else { debug_error(evil); }
	return state;
}

void			debug_pad		(void)
{
	printf("\r");
	int i; for(i=0;i<DEBUG_INDEX;i++) { printf(DEBUG_PADDING); }
}


#endif

/*
(C) 2026, Blood Rose Records

This software is made available under the
v1.0 Blood Rose 4-Clause License

Redistribution and use in source and binary forms,
with or without modification,
are permitted provided that the following conditions are met:

1. Redistribution of source code must retain
the above copyright notice, this list of conditions, and the following disclaimer.

2. Redistribution in binary form must reproduce
the above copyright notice, this list of conditions, and the following disclaimer
in the documentation and/or other materials provided with the distribution.

3. This license does not grant the right to sell the software.
"Sell" means practicing any or all of the rights granted by this license
to provide a product or service to third parties for a fee or other consideration
(including, without limitation, fees for hosting, consulting, or support services),
where the product or service derives substantially or wholly from the software.

4. Neither the name of the licensor nor the names of
the software's contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO event_t SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*/
