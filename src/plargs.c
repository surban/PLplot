/* $Id$
   $Log$
   Revision 1.5  1993/07/01 22:27:59  mjl
   Changed all plplot source files to include plplotP.h (private) rather than
   plplot.h.  Many changes to capabilities of argument parser.  New mode
   flags as well as argument flags.  User-specified variables can now be set
   directly, depending on flags.  Invisible options and ignored options added.
   Many internal options added (most invisible) for support of the TK driver.
   See internal documentation for more details.

 * Revision 1.4  1993/04/26  19:57:56  mjl
 * Fixes to allow (once again) output to stdout and plrender to function as
 * a filter.  A type flag was added to handle file vs stream differences.
 *
 * Revision 1.3  1993/03/17  17:01:37  mjl
 * Eliminated some dead assignments that turned up when running with SAS/C's
 * global optimizer enabled on the Amiga.
 *
 * Revision 1.2  1993/03/03  17:03:20  mjl
 * Changed the -bg flag to accept a full-color argument in the form
 * -bg rrggbb, with rr, gg, bb corresponding to the background RGB
 * values in hex.  Example: -bg FFFF00 to get a yellow background.
 *
 * Revision 1.1  1993/02/23  05:28:26  mjl
 * Added code to parse command line arguments.  Accepts a data structure with
 * argument specification, syntax, description, mode flag, and function handler
 * (called if option is found).  Usable both to parse plplot command flags and
 * user command flags.  The latter is facilitated by input of user routines to
 * handle usage and help messages.  The plplot command line parser removes all
 * arguments it recognizes, allowing the user to detect invalid input.  See
 * plrender.c for a working example of usage of the case of user
 * command flags; the simpler case with no user command flags is illustrated
 * by any of the (C) example programs.
 *
*/

/*
    plargs.c

    Copyright 1993
    Maurice LeBrun

    This software may be freely copied, modified and redistributed without
    fee provided that this copyright notice is preserved intact on all
    copies and modified copies.

    There is no warranty or other guarantee of fitness of this software.
    It is provided solely "as is". The author(s) disclaim(s) all
    responsibility and liability with respect to this software's usage or
    its effect upon hardware or computer systems.

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    This file contains routines to extract & process command flags.  The
    command flags recognized by PLPLOT are stored in the "ploption_table"
    structure, along with strings giving the syntax, long help message,
    and option handler.  The command line parser removes all recognized
    flags (decreasing argc accordingly), so that invalid input may be
    readily detected.  These routines can also be used to process user
    command line flags.

    The command line parsers (one for internal plplot flags, one for
    user-specified flags) accept a variable "mode" which can have
    the following bits set:

    PL_PARSE_FULL -- Full parsing of command line and all error messages
    enabled, including program exit when an error occurs.  This is
    automatically set when calling plParseInternalOpts().  User programs
    that have other command flags should not use this, in which case the
    caller must issue help and syntax messages.  In both cases syntax
    and help messages may still be printed when appropriate.  

    PL_PARSE_QUIET -- Turns off all output except in the case of
    errors.

    PL_PARSE_NODELETE -- Turns off deletion of processed arguments.

    PL_PARSE_SHOWALL -- Show invisible options 

    PL_PARSE_OVERRIDE -- Compares user option table to internal one, and
    disables all internal options that match user options.  Has no effect
    in plParseInternalOpts() or plSetInternalOpt() calls.

    PL_PARSE_NOPROGRAM -- Specified if argv[0] is NOT a pointer to the
    program name.

    Note that the parser for user-defined flags accepts a pointer to a
    function to be called when an error is detected, to allow an
    appropriate usage message to be issued.

    See plrender.c for examples of actual usage.

    A nice enhancement would be support for adding more option tables to
    be checked, where each would presumably originate from additional
    libraries supporting command line options.  When added a flag would
    specify how name clashes are to be handled (possibly supporting all
    three cases: one table dominates, or handle them on an even footing
    if the options are compatible).  Also a scheme would have to be
    worked out to intelligently handle the combined syntax and help
    entries.  Note this may NOT always help in combining options since
    sometimes a certain order must be retained (e.g. plrender follows
    the order: process plrender options, read metafile header, process
    plplot options).
*/

#include "plplotP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "plplotio.h"
#include "plstream.h"

/* Support functions */
/* INDENT OFF */

static void Usage	(char *);
static int  ParseOpt	(int *, char ***, int *, char ***, PLOptionTable *);
static int  ProcessOpt	(char *, PLOptionTable *, int *, char ***, int *);
static int  GetOptarg	(char **, int *, char ***, int *);

static void (*UsageH) (char *) = Usage;

/* Option handlers */

static int opt_h		(char *, char *);
static int opt_v		(char *, char *);
static int opt_dev		(char *, char *);
static int opt_o		(char *, char *);
static int opt_geo		(char *, char *);
static int opt_a		(char *, char *);
static int opt_ori		(char *, char *);
static int opt_width		(char *, char *);
static int opt_color		(char *, char *);
static int opt_bg		(char *, char *);
static int opt_fam		(char *, char *);
static int opt_fsiz		(char *, char *);
static int opt_bufmax		(char *, char *);
static int opt_np		(char *, char *);
static int opt_px		(char *, char *);
static int opt_py		(char *, char *);
static int opt_geo		(char *, char *);
static int opt_plserver		(char *, char *);
static int opt_plwindow		(char *, char *);
static int opt_tcl_cmd		(char *, char *);
static int opt_auto_path		(char *, char *);
static int opt_bufmax		(char *, char *);

/* Global variables */

static char	*program_name = "<user program>";

static int	mode_full;
static int	mode_quiet;
static int	mode_nodelete;
static int	mode_showall;
static int	mode_noprogram;
static int	mode_override;

/*----------------------------------------------------------------------*\
* PLPLOT options data structure definition.
*
* The table is defined as follows
*
* typedef struct {
*     char *opt;
*     int  (*handler)	(char *, char *);
*     void *var;
*     long mode;
*     char *syntax;
*     char *desc;
* } PLOptionTable;
*
* where each entry has the following meaning:
*
* opt		option string
* handler	pointer to function for processing the option and
*		 (optionally) its argument
* var		address of variable to set based on "mode"
* mode		governs handling of option (see below)
* syntax	short syntax description
* desc		long syntax description
*
* The mode bits are:
*
* PL_OPT_ENABLED	Processing for option is enabled
* PL_OPT_ARG		Option has an argment 
* PL_OPT_NODELETE	Don't delete after processing 
* PL_OPT_INVISIBLE	Make invisible (usually for debugging)
*
* The following mode bits cause the option to be processed as specified:
*
* PL_OPT_FUNC		Call function handler (opt, optarg)
* PL_OPT_BOOL		Set *var=1
* PL_OPT_INT		Set *var=atoi(optarg)
* PL_OPT_FLOAT		Set *var=atof(optarg)
* PL_OPT_STRING		Set var=optarg
*
* where opt points to the option string and optarg points to the
* argument string.
*
\*----------------------------------------------------------------------*/

static PLOptionTable ploption_table[] = {
{
    "h",			/* Help */
    opt_h,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED,
    "-h",
    "Print out this message" },
{
    "v",			/* Version */
    opt_v,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED,
    "-v",
    "Print out the plplot library version number" },
{
    "dev",			/* Output device */
    opt_dev,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-dev name",
    "Output device name" },
{
    "showall",			/* Turns on invisible options */
    NULL,
    &mode_showall,
    PL_OPT_BOOL | PL_OPT_ENABLED | PL_OPT_INVISIBLE,
    "-showall",
    "Turns on invisible options" },
{
    "o",			/* Output filename */
    opt_o,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-o name",
    "Output filename" },
{
    "display",			/* X server */
    opt_o,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-display name",
    "X server to contact" },
{
    "px",			/* Plots per page in x */
    opt_px,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-px number",
    "Plots per page in x" },
{
    "py",			/* Plots per page in y */
    opt_py,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-py number",
    "Plots per page in y" },
{
    "geometry",			/* Geometry */
    opt_geo,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-geometry geom",
    "Window size, in pixels (e.g. -geometry 400x300)" },
{
    "geo",			/* Geometry (alias) */
    opt_geo,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG | PL_OPT_INVISIBLE,
    "-geo geom",
    "Window size, in pixels (e.g. -geo 400x300)" },
{
    "a",			/* Aspect ratio */
    opt_a,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-a aspect",
    "Plot aspect ratio" },
{
    "ori",			/* Orientation */
    opt_ori,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-ori orient",
    "Plot orientation (0,2=landscape, 1,3=portrait)" },
{
    "width",			/* Pen width */
    opt_width,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-width width",
    "Default pen width (1 <= width <= 10)" },
{
    "color",			/* Color on switch */
    opt_color,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED,
    "-color",
    "Enables color output (e.g. for PS driver)" },
{
    "bg",			/* Background color */
    opt_bg,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-bg color",
    "Background color (0=black, FFFFFF=white)" },
{
    "fam",			/* Familying on switch */
    opt_fam,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED,
    "-fam",
    "Create a family of output files" },
{
    "fsiz",			/* Family file size */
    opt_fsiz,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG,
    "-fsiz size",
    "Output family file size in MB (e.g. -fsiz 1.0)" },
{
    "np",			/* Page pause off switch */
    opt_np,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED,
    "-np",
    "No pause between pages" },
{
    "bufmax",			/* # bytes sent before flushing output */
    opt_bufmax,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG | PL_OPT_INVISIBLE,
    "-bufmax",
    "bytes sent before flushing output" },
{
    "plserver",			/* plplot server name */
    opt_plserver,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG | PL_OPT_INVISIBLE,
    "-plserver name",
    "Name of plplot server" },
{
    "plwindow",			/* plplot container window name */
    opt_plwindow,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG | PL_OPT_INVISIBLE,
    "-plwindow name",
    "Name of plplot container window" },
{
    "tcl_cmd",			/* TCL initialization command */
    opt_tcl_cmd,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG | PL_OPT_INVISIBLE,
    "-tcl_cmd command",
    "TCL command string run at startup" },
{
    "auto_path",		/* Additional directory(s) to autoload */
    opt_auto_path,
    NULL,
    PL_OPT_FUNC | PL_OPT_ENABLED | PL_OPT_ARG | PL_OPT_INVISIBLE,
    "-auto_path dir",
    "Additional directory(s) to autoload" },
{
    NULL,
    NULL,
    NULL,
    0,
    NULL,
    NULL }
};

static char *notes[] = {
"All parameters must be white-space delimited.  Some options are driver",
"dependent.  Please see the plplot reference document for more detail.",
NULL};

/* INDENT ON */
/*----------------------------------------------------------------------*\
* The following routines contain some code derived from "xterm.c" and
* "ParseCmd.c" of the X-windows Version 11 distribution.  The copyright
* notice is reproduced here:

Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

* The full permission notice is given in the plplot documentation.
\*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*\
* plSyntax()
*
* Print syntax message appropriate for plplot.
\*----------------------------------------------------------------------*/

void
plSyntax()
{
    PLOptionTable *tab;
    int col, len;

    fprintf(stderr, "\nplplot options:");

    col = 80;
    for (tab = ploption_table; tab->syntax; tab++) {
	if ( ! (tab->mode & PL_OPT_ENABLED))
	    continue;

	if ( ! mode_showall && (tab->mode & PL_OPT_INVISIBLE))
	    continue;

	len = 3 + strlen(tab->syntax);		/* space [ string ] */
	if (col + len > 79) {
	    fprintf(stderr, "\r\n   ");		/* 3 spaces */
	    col = 3;
	}
	fprintf(stderr, " [%s]", tab->syntax);
	col += len;
    }
    fprintf(stderr, "\r\n");
}

/*----------------------------------------------------------------------*\
* plHelp()
*
* Print long help message appropriate for plplot.
\*----------------------------------------------------------------------*/

void
plHelp(void)
{
    PLOptionTable *tab;

    fprintf(stderr, "\nplplot options:\n");
    for (tab = ploption_table; tab->syntax; tab++) {
	if ( ! (tab->mode & PL_OPT_ENABLED))
	    continue;

	if ( ! mode_showall && (tab->mode & PL_OPT_INVISIBLE))
	    continue;

	if (tab->desc == NULL)	/* Should never happen but let's */
	    break;		/* be safe */

	if (tab->mode & PL_OPT_INVISIBLE) 
	    fprintf(stderr, " *  %-20s %s\n", tab->syntax, tab->desc);
	else 
	    fprintf(stderr, "    %-20s %s\n", tab->syntax, tab->desc);
    }
}

/*----------------------------------------------------------------------*\
* plNotes()
*
* Print usage notes.
\*----------------------------------------------------------------------*/

void
plNotes(void)
{
    char **cpp;

    putc('\n', stderr);
    for (cpp = notes; *cpp; cpp++) {
	fputs(*cpp, stderr);
	putc('\n', stderr);
    }
    putc('\n', stderr);
}

/*----------------------------------------------------------------------*\
* plParseInternalOpts()
*
* Process plplot internal options list
* If mode is PL_PARSE_FULL, exit on an error.
\*----------------------------------------------------------------------*/

int
plParseInternalOpts(int *p_argc, char **argv, PLINT mode)
{
    int status;

    mode &= ~PL_PARSE_OVERRIDE;
    status = plParseOpts(p_argc, argv, mode, ploption_table, NULL);

    return(status);
}

/*----------------------------------------------------------------------*\
* plSetInternalOpt()
*
* Process input strings, treating them as an option and argument pair.
* Returns 1 on an error.
\*----------------------------------------------------------------------*/

int
plSetInternalOpt(char *opt, char *optarg)
{
    int mode = 0, argc, status;
    char *argv[3];

    argv[0] = opt;
    argv[1] = optarg;
    argv[2] = NULL;

    mode = PL_PARSE_QUIET | PL_PARSE_NODELETE | PL_PARSE_NOPROGRAM;
    argc = 2;

    status = plParseOpts(&argc, argv, mode, ploption_table, NULL);
    if (status) {
	fprintf( stderr, "plSetInternalOpt: Unrecognized option %s\n", opt);
    }
    return(status);
}

/*----------------------------------------------------------------------*\
* plParseOpts()
*
* Process options list
* An error in parsing the argument list causes a program exit if
* mode_full is set, otherwise the function returns with an error.
\*----------------------------------------------------------------------*/

int
plParseOpts(int *p_argc, char **argv, PLINT mode, PLOptionTable *option_table,
	    void (*usage_handler) (char *))
{
    char **argsave, **argend;
    int	myargc, status = 0;
    PLStream *pls;
    PLOptionTable *tab, *pltab;

/* Initialize */

    mode_full      = mode & PL_PARSE_FULL;
    mode_quiet     = mode & PL_PARSE_QUIET;
    mode_nodelete  = mode & PL_PARSE_NODELETE;
    mode_showall   = mode & PL_PARSE_SHOWALL;
    mode_noprogram = mode & PL_PARSE_NOPROGRAM;
    mode_override  = mode & PL_PARSE_OVERRIDE;

    myargc = (*p_argc); 
    argend = argv + myargc;
    if (usage_handler != NULL)
	UsageH = usage_handler;

/* Disable internal options that match user options if mode_override is set */

    if (mode_override) {
	for (tab = option_table; tab->opt; tab++) {
	    for (pltab = ploption_table; pltab->opt; pltab++) {
		if ( ! (pltab->mode & PL_OPT_ENABLED)) 
		    continue;

		if (*tab->opt == *pltab->opt &&
		    ! strcmp(tab->opt, pltab->opt)) {
		    pltab->mode &= ! PL_OPT_ENABLED;
		}
	    }
	}
    }

/* If program name is first argument, save and advance */

    if ( ! mode_noprogram) {
	program_name = argv[0];
	plgpls(&pls);
	pls->program = argv[0];

	--myargc; ++argv;
	argsave = argv;
    }

    if (myargc == 0)
	return(0);

/* Process the command line */

    for (; myargc > 0; --myargc, ++argv) {
	status = ParseOpt(&myargc, &argv, p_argc, &argsave, option_table);
	if (status) {
	    if (mode_full)
		exit(1);
	    else
		break;
	}
    }

/* NULL-terminate compressed argv */

    if ( ! mode_nodelete && (argsave < argend))
	(*argsave) = NULL;

    return(status);
}

/*----------------------------------------------------------------------*\
* ParseOpt()
*
* Parses & determines appropriate action for input flag.
\*----------------------------------------------------------------------*/

static int
ParseOpt(int *p_myargc, char ***p_argv, int *p_argc, char ***p_argsave,
	 PLOptionTable *option_table)
{
    PLOptionTable *tab;
    char *opt;

/* Only handle actual flags and their arguments */

    if ((*p_argv)[0][0] == '-') {

	opt = (*p_argv)[0] + 1;

	for (tab = option_table; tab->opt; tab++) {

/* Skip if option not enabled */

	    if ( ! (tab->mode & PL_OPT_ENABLED)) 
		continue;

	    if (*opt == *tab->opt && ! strcmp(opt, tab->opt)) {

/* Option matched, so remove from argv list if applicable. */

		if (mode_nodelete || (tab->mode & PL_OPT_NODELETE))
		    (*(*p_argsave)++) = (**p_argv);
		else
		    --(*p_argc);

/* Process option (and argument if applicable) */

		return(ProcessOpt(opt, tab, p_myargc, p_argv, p_argc));
	    }
	}
    }

/* If control reaches here the argument is unrecognized */

    if ( ! mode_nodelete)
	(*(*p_argsave)++) = (**p_argv);  /* compress arglist */ 

    if (mode_full) {
	if ( ! mode_quiet)
	    (*UsageH) (**p_argv);
	return(1);
    }
    else
	return(0);
}

/*----------------------------------------------------------------------*\
* ProcessOpt()
*
* Process option (and argument if applicable).
\*----------------------------------------------------------------------*/

static int
ProcessOpt(char *opt, PLOptionTable *tab, int *p_myargc, char ***p_argv,
	   int *p_argc)
{
    int need_arg;
    char *optarg = NULL;

/* Get option argument if necessary */

    need_arg = PL_OPT_ARG | PL_OPT_INT | PL_OPT_FLOAT | PL_OPT_STRING;

    if (tab->mode & need_arg) {
	if (GetOptarg(&optarg, p_myargc, p_argv, p_argc))
	    return(1);
    }

/* Process argument */

    switch (tab->mode & 0xFF00) {

/* Call function handler to do the job */

      case PL_OPT_FUNC:

	if (tab->handler == NULL) {
	    fprintf(stderr,
		    "ProcessOpt: no handler specified for option %s\n",
		    tab->opt);
	    return(1);
	}
	return( (*tab->handler) (opt, optarg) );

/* Set *var as a boolean */

      case PL_OPT_BOOL:
	if (tab->var == NULL) {
	    fprintf(stderr,
		    "ProcessOpt: no variable specified for option %s\n",
		    tab->opt);
	    return(1);
	}
	*(int *)tab->var = 1;
	break;

/* Set *var as an int */

      case PL_OPT_INT:
	if (tab->var == NULL) {
	    fprintf(stderr,
		    "ProcessOpt: no variable specified for option %s\n",
		    tab->opt);
	    return(1);
	}
	*(int *)tab->var = atoi(optarg);
	break;

/* Set *var as a float */

      case PL_OPT_FLOAT:
	if (tab->var == NULL) {
	    fprintf(stderr,
		    "ProcessOpt: no variable specified for option %s\n",
		    tab->opt);
	    return(1);
	}
	*(float *)tab->var = atof(optarg);
	break;

/* Set var (can be NULL initially) to point to optarg string */

      case PL_OPT_STRING:
	tab->var = optarg;
	break;

/* Somebody messed up.. */

      default:
	fprintf(stderr,
		"ProcessOpt: invalid processing mode for option %s\n",
		tab->opt);
	return(1);
    }
    return(0);
}

/*----------------------------------------------------------------------*\
* GetOptarg()
*
* Retrieves an option argument.
* If an error occurs here it is a true syntax error.
\*----------------------------------------------------------------------*/

static int
GetOptarg(char **poptarg, int *p_myargc, char ***p_argv, int *p_argc)
{
    int error = 0;

    --(*p_myargc);

    if ((*p_myargc) <= 0)		/* oops, no more arguments */
	error = 1;

    if ( ! error) {
	(*p_argv)++;
	if ((*p_argv)[0][0] == '-' && isalpha((*p_argv)[0][1])) {

	    (*p_argv)--;		/* oops, next arg is a flag */
	    error = 1;
	}
    }

    if ( ! error) {			/* yeah, the user got it right */
	(*p_argc)--;
	*poptarg = (*p_argv)[0];
	return(0);
    }
    else {
	if ( ! mode_quiet) {
	    fprintf(stderr, "Argument missing for %s option.\n",
		    (*p_argv)[0]); 
	    (*UsageH) ("");
	    return(1);
	}
    }
}

/*----------------------------------------------------------------------*\
* Usage()
*
* Print usage & syntax message.
\*----------------------------------------------------------------------*/

static void
Usage(char *badOption)
{
    if (*badOption != '\0')
	fprintf(stderr, "\n%s:  bad command line option \"%s\"\r\n",
		program_name, badOption);

    fprintf(stderr, "\nUsage:\n        %s [plplot options]\n",
	    program_name);

    plSyntax();

    fprintf(stderr, "\r\n\nType %s -h for a full description.\r\n\n",
	    program_name);
}

/*----------------------------------------------------------------------*\
* Option handlers
\*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*\
* opt_h()
*
* Performs appropriate action for option "h".
\*----------------------------------------------------------------------*/

static int
opt_h(char *opt, char *optarg)
{

/* Help */

    if ( ! mode_quiet) {
	fprintf(stderr, "\nUsage:\n        %s [plplot options]\n",
		program_name);

	plHelp();
	plNotes();
    }
    return(1);
}

/*----------------------------------------------------------------------*\
* opt_v()
*
* Performs appropriate action for option "v".
\*----------------------------------------------------------------------*/

static int
opt_v(char *opt, char *optarg)
{

/* Version */

    if ( ! mode_quiet) {
	fprintf(stderr, "\nplplot library version: %s\n", PLPLOT_VERSION);
    }
    return(1);
}

/*----------------------------------------------------------------------*\
* opt_dev()
*
* Performs appropriate action for option "dev".
\*----------------------------------------------------------------------*/

static int
opt_dev(char *opt, char *optarg)
{
/* Output device */

    plsdev(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_o()
*
* Performs appropriate action for option "o".
\*----------------------------------------------------------------------*/

static int
opt_o(char *opt, char *optarg)
{
/* Output file */

    plsfnam(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_a()
*
* Performs appropriate action for option "a".
\*----------------------------------------------------------------------*/

static int
opt_a(char *opt, char *optarg)
{
    PLFLT aspect;

/* Override aspect ratio */

    aspect = atof(optarg);
    plsasp(aspect);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_ori()
*
* Performs appropriate action for option "ori".
\*----------------------------------------------------------------------*/

static int
opt_ori(char *opt, char *optarg)
{
    int orient;

/* Orientation */

    orient = atoi(optarg);
    plsori(orient);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_width()
*
* Performs appropriate action for option "width".
\*----------------------------------------------------------------------*/

static int
opt_width(char *opt, char *optarg)
{
    int width;

/* Width */

    width = atoi(optarg);
    if (width == 0) {
	fprintf(stderr, "?invalid width\n");
	return(1);
    }
    else
	plwid(width);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_bg()
*
* Performs appropriate action for option "bg".
\*----------------------------------------------------------------------*/

static int
opt_bg(char *opt, char *optarg)
{
    long bgcolor, r, g, b;

/* Background */

    bgcolor = strtol(optarg, NULL, 16);
    r = (bgcolor & 0xFF0000) >> 16;
    g = (bgcolor & 0x00FF00) >> 8;
    b = (bgcolor & 0x0000FF);
    plscolbg(r, g, b);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_color()
*
* Performs appropriate action for option "color".
\*----------------------------------------------------------------------*/

static int
opt_color(char *opt, char *optarg)
{

/* Color */

    plscolor(1);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_fam()
*
* Performs appropriate action for option "fam".
\*----------------------------------------------------------------------*/

static int
opt_fam(char *opt, char *optarg)
{

/* Family output files */

    plsfam(1, -1, -1);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_fsiz()
*
* Performs appropriate action for option "fsiz".
\*----------------------------------------------------------------------*/

static int
opt_fsiz(char *opt, char *optarg)
{
    PLINT bytemax;

/* Size of a member file (may be larger since eof must occur at page break) */

    bytemax = 1.0e6 * atof(optarg);
    if (bytemax == 0) {
	fprintf(stderr, "?invalid bytemax\n");
	return(1);
    }
    plsfam(-1, -1, bytemax);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_np()
*
* Performs appropriate action for option "np".
\*----------------------------------------------------------------------*/

static int
opt_np(char *opt, char *optarg)
{

/* No pause between pages */

    plspause(0);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_bufmax()
*
* Performs appropriate action for option "bufmax".
\*----------------------------------------------------------------------*/

static int
opt_bufmax(char *opt, char *optarg)
{
    PLStream *pls;

/* Bytes sent before output is flushed */

    plgpls(&pls);
    pls->bufmax = atoi(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_plserver()
*
* Performs appropriate action for option "plserver".
\*----------------------------------------------------------------------*/

static int
opt_plserver(char *opt, char *optarg)
{
    PLStream *pls;

/* plplot server name */

    plgpls(&pls);
    pls->plserver = optarg;

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_plwindow()
*
* Performs appropriate action for option "plwindow".
\*----------------------------------------------------------------------*/

static int
opt_plwindow(char *opt, char *optarg)
{
    PLStream *pls;

/* plplot window name */

    plgpls(&pls);
    pls->plwindow = optarg;

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_tcl_cmd()
*
* Performs appropriate action for option "tcl_cmd".
\*----------------------------------------------------------------------*/

static int
opt_tcl_cmd(char *opt, char *optarg)
{
    PLStream *pls;

/* TCL command(s) to eval on startup */

    plgpls(&pls);
    pls->tcl_cmd = optarg;

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_auto_path()
*
* Performs appropriate action for option "auto_path".
\*----------------------------------------------------------------------*/

static int
opt_auto_path(char *opt, char *optarg)
{
    PLStream *pls;

/* Additional directories to autoload */

    plgpls(&pls);
    pls->auto_path = optarg;

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_px()
*
* Performs appropriate action for option "px".
\*----------------------------------------------------------------------*/

static int
opt_px(char *opt, char *optarg)
{

/* Pack in x */

    plssub(atoi(optarg), -1);

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_py()
*
* Performs appropriate action for option "py".
\*----------------------------------------------------------------------*/

static int
opt_py(char *opt, char *optarg)
{

/* Pack in y */

    plssub(-1, atoi(optarg));

    return(0);
}

/*----------------------------------------------------------------------*\
* opt_geo()
*
* Performs appropriate action for option "geo".
\*----------------------------------------------------------------------*/

static int
opt_geo(char *opt, char *optarg)
{
    char *field;
    PLFLT xdpi = 0., ydpi = 0.;
    PLINT xwid, ywid, xoff = 0, yoff = 0;
    PLStream *pls;

/* The TK driver uses the geometry string directly */

    plgpls(&pls);
    pls->geometry = malloc((size_t) (1+strlen(optarg)) * sizeof(char));
    strcpy(pls->geometry, optarg);

/* Geometry for output window (e.g. 400x400+100+0) */

    if ((field = strtok(optarg, "x")) == NULL)
	return(1);

    xwid = atoi(field);
    if (xwid == 0) {
	fprintf(stderr, "?invalid xwid\n");
	return(1);
    }

    if ((field = strtok(NULL, "+")) == NULL)
	return(1);

    ywid = atoi(field);
    if (ywid == 0) {
	fprintf(stderr, "?invalid ywid\n");
	return(1);
    }

    if ((field = strtok(NULL, "+")) != NULL) {
	xoff = atoi(field);

	if ((field = strtok(NULL, "+")) != NULL)
	    yoff = atoi(field);
    }

    plspage(xdpi, ydpi, xwid, ywid, xoff, yoff);

    return(0);
}
