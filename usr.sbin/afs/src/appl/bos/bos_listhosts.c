/*
 * Copyright (c) 1995 - 2000 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 * 
 * 4. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "appl_locl.h"
#include <sl.h>
#include "bos_local.h"
#include <bos.h>
#include <bos.cs.h>


RCSID("$Id: bos_listhosts.c,v 1.1 2000/09/11 14:40:35 art Exp $");

static int
printhosts(const char *cell, const char *host,
	    int noauth, int localauth, int verbose)
{
  struct rx_connection *connvolser = NULL;
  unsigned int i;
  int error;
  char *server_name;
  char *cell_name;

  connvolser = arlalib_getconnbyname(cell,
				     host,
				     afsbosport,
				     BOS_SERVICE_ID,
				     arlalib_getauthflag(noauth,
							 localauth,0,0));

  if (connvolser == NULL)
    return -1;

  /* which cell is this anyway ? */
  if (( error = BOZO_GetCellName(connvolser, &cell_name)) == 0) {
    printf("Cell name is %s\n",cell_name);
  }
  else {
    printf ("bos %s: %s\n", host, koerr_gettext (error));
  }
  
  /* Who are the VLDB-servers ? */
  i = 0;
  while ((error = BOZO_GetCellHost(connvolser, i,
				   &server_name)) == 0) {
    printf("\t Host %d is %s\n",i+1,server_name);
    i++;
  }

  if (error != BZDOM)
    printf ("bos %s: %s\n", host, koerr_gettext (error));
  arlalib_destroyconn(connvolser);
  return 0;
}


static int helpflag;
static const char *server;
static const char *cell;
static int noauth;
static int localauth;
static int verbose;

static struct getargs args[] = {
  {"server",	0, arg_string,	&server,	"server", NULL, arg_mandatory},
  {"cell",	0, arg_string,	&cell,		"cell",	  NULL},
  {"noauth",	0, arg_flag,	&noauth,	"do not authenticate", NULL},
  {"local",	0, arg_flag,	&localauth,	"localauth"},
  {"verbose",	0, arg_flag,	&verbose,	"be verbose", NULL},
  {"help",	0, arg_flag,	&helpflag,	NULL, NULL},
  {NULL,	0, arg_end,	NULL,		NULL, NULL}
};

static void
usage (void)
{
  arg_printusage (args, "bos listhosts", "", ARG_AFSSTYLE);
}

int
bos_listhosts(int argc, char **argv)
{
  int optind = 0;

  if (getarg (args, argc, argv, &optind, ARG_AFSSTYLE)) {
    usage ();
    return 0;
  }

  if (helpflag) {
    usage ();
    return 0;
  }

  argc -= optind;
  argv += optind;

  if (server == NULL) {
    printf ("bos listhosts: missing -server\n");
    return 0;
  }

  if (cell == NULL)
    cell = cell_getcellbyhost (server);

  printhosts (cell, server, noauth, localauth, verbose);
  return 0;
}
