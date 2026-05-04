/* ------------ License
License:        Blood Rose 4 Clause License v1.0
Licensor:       Blood Rose <https://bloodrose.org>
A full copy of the license terms is available at the end of this source file */

/* ------------	Usage
	Compile with -lcurl */

// ------------	Type Definitions

typedef struct {
	char * stream;
	size_t size;
} net_chunk_t;

typedef struct {
	int status;
	const char * mime;
	const char * body;
	size_t size;
	net_chunk_t * chunk;
} net_t;

// ------------	Function Signatures

bool			net_fetch		(net_t *, const char *);
bool			net_init		(void);
static size_t	__net_write		(void * data, size_t size, size_t nm, void *chunk);

// ------------	Implementation

#ifndef NET_H
#define NET_H

#include <string.h>
#include <curl/curl.h>
// #include <curl.h>

CURL * NET_CURL = NULL;

bool			net_init		(void)
{
	if (NET_CURL) { return true; }
	NET_CURL = curl_easy_init(); if (!NET_CURL) { return false; }
	return true;
}

bool			net_fetch		(net_t * response, const char * url)
{
	if (!net_init()) { return false; }
	net_chunk_t *yield = malloc(sizeof(net_chunk_t));
	curl_easy_setopt(NET_CURL, CURLOPT_URL, url);
	curl_easy_setopt(NET_CURL, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(NET_CURL, CURLOPT_WRITEFUNCTION, __net_write);
    curl_easy_setopt(NET_CURL, CURLOPT_WRITEDATA, (void *)yield);
	CURLcode code = curl_easy_perform(NET_CURL);
	if ( code != CURLE_OK ) {
		free(yield);
		response->status = code;
		response->body = curl_easy_strerror(code);
		response->size = strlen(response->body);
		return false;
	}
	curl_easy_getinfo(NET_CURL, CURLINFO_RESPONSE_CODE, &response->status);
	curl_easy_getinfo(NET_CURL, CURLINFO_CONTENT_TYPE, &response->mime);
	response->chunk = yield;
	response->body = yield->stream;
	response->size = yield->size;
	return true;
}

static size_t	__net_write		(void * data, size_t refsize, size_t nm, void *refchunk)
{
	size_t size = refsize * nm;
	net_chunk_t * chunk = (net_chunk_t *) refchunk;
	void * pointer = realloc(chunk->stream, chunk->size + size + 1);
		if (!pointer) { return 0; }
	chunk->stream = pointer;
	memcpy(&chunk->stream[chunk->size], data, size);
	chunk->size += size;
	chunk->stream[chunk->size] = '\0';
	return size;
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
