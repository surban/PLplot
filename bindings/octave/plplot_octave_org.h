/*
 * jc: this is a massaged plplot.h 
 */

/*
    Copyright (C) 1992 by 
    Maurice J. LeBrun, Geoff Furnish, Tony Richardson.

    Macros and prototypes for the PLplot package.  This header file must
    be included by all user codes.

    This software may be freely copied, modified and redistributed
    without fee provided that this copyright notice is preserved intact
    on all copies and modified copies.

    There is no warranty or other guarantee of fitness of this software.
    It is provided solely "as is". The author(s) disclaim(s) all
    responsibility and liability with respect to this software's usage
    or its effect upon hardware or computer systems.

    Note: some systems allow the Fortran & C namespaces to clobber each
    other.  So for PLplot to work from Fortran, we do some rather nasty
    things to the externally callable C function names.  This shouldn't
    affect any user programs in C as long as this file is included. 
*/

#ifndef __PLPLOT_H__
#define __PLPLOT_H__

/*--------------------------------------------------------------------------*\
 *    USING PLplot
 * 
 * To use PLplot from C or C++, it is only necessary to 
 * 
 *      #include "plplot/plplot.h"
 * 
 * This file does all the necessary setup to make PLplot accessible to
 * your program as documented in the manual.  Additionally, this file
 * allows you to request certain behavior by defining certain symbols
 * before inclusion.  At the moment the only one is:
 *
 * #define DOUBLE	or..
 * #define PL_DOUBLE
 *
 * This causes PLplot to use doubles instead of floats.  Use the type
 * PLFLT everywhere in your code, and it will always be the right thing.
 *
 * Note: most of the functions visible here begin with "pl", while all
 * of the data types and switches begin with "PL".  Eventually everything
 * will conform to this rule in order to keep namespace pollution of the
 * user code to a minimum.  All the PLplot source files actually include
 * "plplotP.h", which includes this file as well as all the internally-
 * visible declarations, etc.  
\*--------------------------------------------------------------------------*/

/* The majority of PLplot source files require these, so.. */
/* Under ANSI C, they can be included any number of times. */

#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------------------*\
 *        SYSTEM IDENTIFICATION
 *
 * Several systems are supported directly by PLplot.  In order to avoid
 * confusion, one id macro per system is used.  Since different compilers
 * may predefine different system id macros, we need to check all the
 * possibilities, and then set the one we will be referencing.  These are:
 *
 * __cplusplus                Any C++ compiler
 * __unix                     Any Unix-like system
 * __hpux                     Any HP/UX system
 * __aix                      Any AIX system
 * __linux                    Linux for i386
 * (others...)
 *
\*--------------------------------------------------------------------------*/

#ifdef unix			/* the old way */
#ifndef __unix
#define __unix
#endif
#endif

/* Make sure Unix systems define "__unix" */

#if defined(SX)	||				/* NEC Super-UX */      \
    (defined(_IBMR2) && defined(_AIX)) ||	/* AIX */               \
    defined(__hpux) ||				/* HP/UX */             \
    defined(sun) ||				/* SUN */               \
    defined(CRAY) ||				/* Cray */              \
    defined(__convexc__) ||			/* CONVEX */            \
    (defined(__alpha) && defined(__osf__))	/* DEC Alpha AXP/OSF */

#ifndef __unix
#define __unix
#endif
#endif

/* A wrapper used in some header files so they can be compiled with cc */

#define PLARGS(a)	a

/*--------------------------------------------------------------------------*\
 * Base types for PLplot
 *
 * Only those that are necessary for function prototypes are defined here.
 * Notes:
 *
 * PLINT is typedef'd to an int by default.  This is a change from some
 * previous versions, where a long was used.  Under MSDOS, a PLINT is
 * typedef'd to a long, since 16 bits is too inaccurate for some PLplot
 * functions.  So under MSDOS you must use type PLINT for integer array
 * arguments to PLplot functions, but on other systems you can just use
 * an int.
 *
 * short is currently used for device page coordinates, so they are
 * bounded by (-32767, 32767).  This gives a max resolution of about 3000
 * dpi, and improves performance in some areas over using a PLINT.
\*--------------------------------------------------------------------------*/

/* I haven't figure out how matwarp -cpp option works, so:
 * WARNING: matwrap don't process conditionals, so they must be hard coded!
 */

/*
 * #if defined(PL_DOUBLE) || defined(DOUBLE)
 * typedef double PLFLT;
 * #else
 * typedef float PLFLT;
 * #endif
 */

REPLACE_ME
 
#if defined(MSDOS)
typedef long PLINT;
#else
typedef int PLINT;
#endif

/* For passing user data, as with X's XtPointer */

typedef void* PLPointer;

/*--------------------------------------------------------------------------*\
 * Complex data types and other good stuff
\*--------------------------------------------------------------------------*/

/* Switches for escape function call. */
/* Some of these are obsolete but are retained in order to process
   old metafiles */

#define PLESC_SET_RGB		1	/* obsolete */
#define PLESC_ALLOC_NCOL	2	/* obsolete */
#define PLESC_SET_LPB		3	/* obsolete */
#define PLESC_EXPOSE		4	/* handle window expose */
#define PLESC_RESIZE		5	/* handle window resize */
#define PLESC_REDRAW		6	/* handle window redraw */
#define PLESC_TEXT		7	/* switch to text screen */
#define PLESC_GRAPH		8	/* switch to graphics screen */
#define PLESC_FILL		9	/* fill polygon */
#define PLESC_DI		10	/* handle DI command */
#define PLESC_FLUSH		11	/* flush output */
#define PLESC_EH		12      /* handle Window events */
#define PLESC_GETC		13	/* get cursor position */
#define PLESC_SWIN		14	/* set window parameters */
#define PLESC_DOUBLEBUFFERING	15	/* configure double buffering */
#define PLESC_XORMOD		16	/* jc: set xor mode */
#define PLESC_SET_COMPRESSION	17	/* AFR: set compression */
#define PLESC_CLEAR		18      /* RL: clear graphics region */
#define PLESC_DASH		19	/* RL: draw dashed line */

/* Window parameter tags */

#define PLSWIN_DEVICE		1	/* device coordinates */
#define PLSWIN_WORLD		2	/* world coordinates */

/* PLplot Option table & support constants */

/* Option-specific settings */

#define PL_OPT_ENABLED		0x0001	/* Obsolete */
#define PL_OPT_ARG		0x0002	/* Option has an argment */
#define PL_OPT_NODELETE		0x0004	/* Don't delete after processing */
#define PL_OPT_INVISIBLE	0x0008	/* Make invisible */
#define PL_OPT_DISABLED		0x0010	/* Processing is disabled */

/* Option-processing settings -- mutually exclusive */

#define PL_OPT_FUNC		0x0100	/* Call handler function */
#define PL_OPT_BOOL		0x0200	/* Set *var = 1 */
#define PL_OPT_INT		0x0400	/* Set *var = atoi(optarg) */
#define PL_OPT_FLOAT		0x0800	/* Set *var = atof(optarg) */
#define PL_OPT_STRING		0x1000	/* Set var = optarg */

/* Global mode settings */
/* These override per-option settings */

#define PL_PARSE_PARTIAL	0x0000	/* For backward compatibility */
#define PL_PARSE_FULL		0x0001	/* Process fully & exit if error */
#define PL_PARSE_QUIET		0x0002	/* Don't issue messages */
#define PL_PARSE_NODELETE	0x0004	/* Don't delete options after */
					/* processing */
#define PL_PARSE_SHOWALL	0x0008	/* Show invisible options */
#define PL_PARSE_OVERRIDE	0x0010	/* Obsolete */
#define PL_PARSE_NOPROGRAM	0x0020	/* Program name NOT in *argv[0].. */
#define PL_PARSE_NODASH		0x0040	/* Set if leading dash NOT required */
#define PL_PARSE_SKIP		0x0080	/* Skip over unrecognized args */

/* Obsolete names */

#define plParseInternalOpts(a, b, c)	plParseOpts(a, b, c)
#define plSetInternalOpt(a, b)		plSetOpt(a, b)

/* Option table definition */

typedef struct {
    char *opt;
    int  (*handler)	(char *, char *, void *);
    void *client_data;
    void *var;
    long mode;
    char *syntax;
    char *desc;
} PLOptionTable;

/* PLplot Graphics Input structure */

#define PL_MAXKEY 16

typedef struct {
    int type;			/* of event (CURRENTLY UNUSED) */
    unsigned int state;		/* key or button mask */
    unsigned int keysym;	/* key selected */
    unsigned int button;	/* mouse button selected */
    char string[PL_MAXKEY];	/* translated string */
    int pX, pY;			/* absolute device coordinates of pointer */
    PLFLT dX, dY;		/* relative device coordinates of pointer */
    PLFLT wX, wY;		/* world coordinates of pointer */
} PLGraphicsIn;

/* Structure for describing the plot window */

#define PL_MAXWINDOWS	64	/* Max number of windows/page tracked */

typedef struct {
    PLFLT dxmi, dxma, dymi, dyma;	/* min, max window rel dev coords */
    PLFLT wxmi, wxma, wymi, wyma;	/* min, max window world coords */
} PLWindow;

/* Structure for doing display-oriented operations via escape commands */
/* May add other attributes in time */

typedef struct {
    unsigned int x, y;			/* upper left hand corner */
    unsigned int width, height;		/* window dimensions */
} PLDisplay;

/* Macro used (in some cases) to ignore value of argument */
/* I don't plan on changing the value so you can hard-code it */

#define PL_NOTSET (-42)

/* See plcont.c for examples of the following */

/*
 * PLfGrid is for passing (as a pointer to the first element) an arbitrarily
 * dimensioned array.  The grid dimensions MUST be stored, with a maximum of 3
 * dimensions assumed for now.
 */

typedef struct {
    PLFLT *f;
    PLINT nx, ny, nz;
} PLfGrid;

/*
 * PLfGrid2 is for passing (as an array of pointers) a 2d function array.  The
 * grid dimensions are passed for possible bounds checking.
 */

typedef struct {
    PLFLT **f;
    PLINT nx, ny;
} PLfGrid2;

/*
 * NOTE: a PLfGrid3 is a good idea here but there is no way to exploit it yet
 * so I'll leave it out for now.
 */

/*
 * PLcGrid is for passing (as a pointer to the first element) arbitrarily
 * dimensioned coordinate transformation arrays.  The grid dimensions MUST be
 * stored, with a maximum of 3 dimensions assumed for now.
 */

typedef struct {
    PLFLT *xg, *yg, *zg;
    PLINT nx, ny, nz;
} PLcGrid;

/*
 * PLcGrid2 is for passing (as arrays of pointers) 2d coordinate
 * transformation arrays.  The grid dimensions are passed for possible bounds
 * checking.
 */

typedef struct {
    PLFLT **xg, **yg, **zg;
    PLINT nx, ny;
} PLcGrid2;

/*
 * NOTE: a PLcGrid3 is a good idea here but there is no way to exploit it yet
 * so I'll leave it out for now.
 */

/* PLColor is the usual way to pass an rgb color value. */

typedef struct {
    unsigned char r;		/* red */
    unsigned char g;		/* green */
    unsigned char b;		/* blue */
    char *name;
} PLColor;

/* PLControlPt is how cmap1 control points are represented. */

typedef struct {
    PLFLT h;			/* hue */
    PLFLT l;			/* lightness */
    PLFLT s;			/* saturation */
    PLFLT p;			/* position */
    int rev;			/* if set, interpolate through h=0 */
} PLControlPt;

/* A PLBufferingCB is a control block for interacting with devices
   that support double buffering. */

typedef struct {
    PLINT cmd;
    PLINT result;
} PLBufferingCB;

#define PLESC_DOUBLEBUFFERING_ENABLE     1
#define PLESC_DOUBLEBUFFERING_DISABLE    2
#define PLESC_DOUBLEBUFFERING_QUERY      3


/*--------------------------------------------------------------------------*\
 *		BRAINDEAD-ness
 *
 * Some systems allow the Fortran & C namespaces to clobber each other.
 * For PLplot to work from Fortran on these systems, we must name the the
 * externally callable C functions something other than their Fortran entry
 * names.  In order to make this as easy as possible for the casual user,
 * yet reversible to those who abhor my solution, I have done the
 * following:
 *
 *	The C-language bindings are actually different from those
 *	described in the manual.  Macros are used to convert the
 *	documented names to the names used in this package.  The
 *	user MUST include plplot.h in order to get the name
 *	redefinition correct.
 *
 * Sorry to have to resort to such an ugly kludge, but it is really the
 * best way to handle the situation at present.  If all available
 * compilers offer a way to correct this stupidity, then perhaps we can
 * eventually reverse it (there is a way now, by defining NOBRAINDEAD, but
 * be careful because this will totally hose the Fortran interface on some
 * systems).  If you feel like screaming at someone (I sure do), please
 * direct it at your nearest system vendor who has a braindead shared
 * C/Fortran namespace.  Some vendors do offer compiler switches that
 * change the object names, but then everybody who wants to use the
 * package must throw these same switches, leading to no end of trouble.
 *
 * Note that this definition should not cause any noticable effects except
 * when debugging PLplot calls, in which case you will need to remember
 * the real function names (same as before but with a 'c_' prepended).
 *
 * Also, to avoid macro conflicts, the BRAINDEAD part must not be expanded
 * in the stub routines.
 *
 * Aside: the reason why a shared Fortran/C namespace is deserving of the
 * BRAINDEAD characterization is that it completely precludes the the kind
 * of universal API that is attempted (more or less) with PLplot, without
 * Herculean efforts (e.g. remapping all of the C bindings by macros as
 * done here).  The vendors of such a scheme, in order to allow a SINGLE
 * type of argument to be passed transparently between C and Fortran,
 * namely, a pointer to a conformable data type, have slammed the door on
 * insertion of stub routines to handle the conversions needed for other
 * data types.  Intelligent linkers could solve this problem, but these are
 * not anywhere close to becoming universal.  So meanwhile, one must live
 * with either stub routines for the inevitable data conversions, or a
 * different API.  The former is what is used here, but is made far more
 * difficult in a braindead shared Fortran/C namespace.
\*--------------------------------------------------------------------------*/

#ifndef BRAINDEAD
#define BRAINDEAD
#endif

#ifdef NOBRAINDEAD
#undef BRAINDEAD
#endif

#ifdef BRAINDEAD

#ifndef __PLSTUBS_H__	/* i.e. do not expand this in the stubs */

#define    pladv	c_pladv
#define    plclear	c_plclear
#define    plaxes	c_plaxes
#define    plbin	c_plbin
#define    plbop	c_plbop
#define    plbox	c_plbox
#define    plbox3	c_plbox3
#define    plcol0	c_plcol0
#define    plcol1	c_plcol1
#define    plxormod	c_plxormod
#define    plcont	c_plcont
#define    plcpstrm	c_plcpstrm
#define    pl_setcontlabelparam c_pl_setcontlabelparam
#define    pl_setcontlabelformat c_pl_setcontlabelformat
#define    plend	c_plend
#define    plend1	c_plend1
#define    plenv	c_plenv
#define    pleop	c_pleop
#define    plerrx	c_plerrx
#define    plerry	c_plerry
#define    plfamadv	c_plfamadv
#define    plfill	c_plfill
#define    plfill3	c_plfill3
#define    plflush	c_plflush
#define    plfont	c_plfont
#define    plfontld	c_plfontld
#define    plgchr	c_plgchr
#define    plgcol0	c_plgcol0
#define    plgcolbg	c_plgcolbg
#define    plgcompression	c_plgcompression
#define    plgdev	c_plgdev
#define    plgdidev	c_plgdidev
#define    plgdiori	c_plgdiori
#define    plgdiplt	c_plgdiplt
#define    plgfam	c_plgfam
#define    plgfnam	c_plgfnam
#define    plglevel	c_plglevel
#define    plgpage	c_plgpage
#define    plgra	c_plgra
#define    plgspa	c_plgspa
#define    plgstrm	c_plgstrm
#define    plgver	c_plgver
#define    plgxax	c_plgxax
#define    plgyax	c_plgyax
#define    plgzax	c_plgzax
#define    plhist	c_plhist
#define    plhls        c_plhls
#define    plinit	c_plinit
#define    pljoin	c_pljoin
#define    pllab	c_pllab
#define    pllightsource	c_pllightsource
#define    plline	c_plline
#define    plline3	c_plline3
#define    pllsty	c_pllsty
#define    plmesh	c_plmesh
#define    plmkstrm	c_plmkstrm
#define    plmtex	c_plmtex
#define    plot3d	c_plot3d
#define    plotsh3d	c_plotsh3d
#define    plpat	c_plpat
#define    plpoin	c_plpoin
#define    plpoin3	c_plpoin3
#define    plpoly3	c_plpoly3
#define    plprec	c_plprec
#define    plpsty	c_plpsty
#define    plptex	c_plptex
#define    plreplot	c_plreplot
#define    plrgb	c_plrgb
#define    plrgb1	c_plrgb1
#define    plschr	c_plschr
#define    plscmap0	c_plscmap0
#define    plscmap1	c_plscmap1
#define    plscmap0n	c_plscmap0n
#define    plscmap1n	c_plscmap1n
#define    plscmap1l	c_plscmap1l
#define    plscol0	c_plscol0
#define    plscolbg	c_plscolbg
#define    plscolor	c_plscolor
#define    plscompression	c_plscompression
#define    plsdev	c_plsdev
#define    plsdiplt	c_plsdiplt
#define    plsdiplz	c_plsdiplz
#define    plsdidev	c_plsdidev
#define    plsdimap	c_plsdimap
#define    plsdiori	c_plsdiori
#define    plsesc	c_plsesc
#define    plsfam	c_plsfam
#define    plsfnam	c_plsfnam
#define    plshades	c_plshades
#define    plshade	c_plshade
#define    plshade1	c_plshade1
#define    plsmaj	c_plsmaj
#define    plsmin	c_plsmin
#define    plsori	c_plsori
#define    plspage	c_plspage
#define    plspause	c_plspause
#define    plsstrm	c_plsstrm
#define    plssub	c_plssub
#define    plssym	c_plssym
#define    plstar	c_plstar
#define    plstart	c_plstart
#define    plstripa	c_plstripa
#define    plstripc	c_plstripc
#define    plstripd	c_plstripd
#define    plstyl	c_plstyl
#define    plsvpa	c_plsvpa
#define    plsxax	c_plsxax
#define    plsyax	c_plsyax
#define    plsym	c_plsym
#define    plszax	c_plszax
#define    pltext	c_pltext
#define    plvasp	c_plvasp
#define    plvpas	c_plvpas
#define    plvpor	c_plvpor
#define    plvsta	c_plvsta
#define    plw3d	c_plw3d
#define    plwid	c_plwid
#define    plwind	c_plwind

#endif /* __PLSTUBS_H__ */

#else

#define    c_pladv	pladv
#define    c_plaxes	plaxes
#define    c_plbin	plbin
#define    c_plbop	plbop
#define    c_plbox	plbox
#define    c_plbox3	plbox3
#define    c_plcol0	plcol0
#define    c_plcol1	plcol1
#define    c_plxormod	plxormod
#define    c_plcpstrm	plcpstrm
#define    c_plcont	plcont
#define    c_pl_setcontlabelparam pl_setcontlabelparam
#define    c_pl_setcontlabelformat pl_setcontlabelformat
#define    c_plend	plend
#define    c_plend1	plend1
#define    c_plenv	plenv
#define    c_pleop	pleop
#define    c_plerrx	plerrx
#define    c_plerry	plerry
#define    c_plfamadv	plfamadv
#define    c_plfill	plfill
#define    c_plfill3	plfill3
#define    c_plflush	plflush
#define    c_plfont	plfont
#define    c_plfontld	plfontld
#define    c_plgchr	plgchr
#define    c_plgcol0	plgcol0
#define    c_plgcolbg	plgcolbg
#define    c_plgcompression	plgcompression
#define    c_plgdev	plgdev
#define    c_plgdidev	plgdidev
#define    c_plgdiori	plgdiori
#define    c_plgdiplt	plgdiplt
#define    c_plgfam	plgfam
#define    c_plgfnam	plgfnam
#define    c_plglevel	plglevel
#define    c_plgpage	plgpage
#define    c_plgra	plgra
#define    c_plgspa	plgspa
#define    c_plgstrm	plgstrm
#define    c_plgver	plgver
#define    c_plgxax	plgxax
#define    c_plgyax	plgyax
#define    c_plgzax	plgzax
#define    c_plhist	plhist
#define    c_plhls	plhls       
#define    c_plinit	plinit
#define    c_pljoin	pljoin
#define    c_pllab	pllab
#define    c_pllightsource pllightsource
#define    c_plline	plline
#define    c_plline3	plline3
#define    c_pllsty	pllsty
#define    c_plmesh	plmesh
#define    c_plmkstrm	plmkstrm
#define    c_plmtex	plmtex
#define    c_plot3d	plot3d
#define    c_plotsh3d	plotsh3d
#define    c_plpat	plpat
#define    c_plpoin	plpoin
#define    c_plpoin3	plpoin3
#define    c_plpoly3	plpoly3
#define    c_plprec	plprec
#define    c_plpsty	plpsty
#define    c_plptex	plptex
#define    c_plreplot	plreplot
#define    c_plrgb	plrgb
#define    c_plrgb1	plrgb1
#define    c_plschr	plschr
#define    c_plscmap0	plscmap0
#define    c_plscmap1	plscmap1
#define    c_plscmap0n	plscmap0n
#define    c_plscmap1n	plscmap1n
#define    c_plscmap1l	plscmap1l
#define    c_plscol0	plscol0
#define    c_plscolbg	plscolbg
#define    c_plscolor	plscolor
#define    c_plscompression	plscompression
#define    c_plsdev	plsdev
#define    c_plsdiplt	plsdiplt
#define    c_plsdiplz	plsdiplz
#define    c_plsdidev	plsdidev
#define    c_plsdimap	plsdimap
#define    c_plsdiori	plsdiori
#define    c_plsesc	plsesc
#define    c_plsfam	plsfam
#define    c_plsfnam	plsfnam
#define    c_plshades	plshades
#define    c_plshade	plshade
#define    c_plshade1	plshade1
#define    c_plsmaj	plsmaj
#define    c_plsmin	plsmin
#define    c_plsori	plsori
#define    c_plspage	plspage
#define    c_plspause	plspause
#define    c_plsstrm	plsstrm
#define    c_plssub	plssub
#define    c_plssym	plssym
#define    c_plstar	plstar
#define    c_plstart	plstart
#define    c_plstripa	plstripa
#define    c_plstripc	plstripc
#define    c_plstripd	plstripd
#define    c_plstyl	plstyl
#define    c_plsvpa	plsvpa
#define    c_plsxax	plsxax
#define    c_plsyax	plsyax
#define    c_plsym	plsym
#define    c_plszax	plszax
#define    c_pltext	pltext
#define    c_plvasp	plvasp
#define    c_plvpas	plvpas
#define    c_plvpor	plvpor
#define    c_plvsta	plvsta
#define    c_plw3d	plw3d
#define    c_plwid	plwid
#define    c_plwind	plwind

#endif	/* BRAINDEAD */

/* Redefine some old function names for backward compatibility */

#ifndef __PLSTUBS_H__	/* i.e. do not expand this in the stubs */

#define    plclr	pleop
#define    plpage	plbop
#define    plcol	plcol0
#define    plcontf	plfcont
#define	   Alloc2dGrid	plAlloc2dGrid
#define	   Free2dGrid	plFree2dGrid

#endif /* __PLSTUBS_H__ */

/*--------------------------------------------------------------------------*\
 *		Function Prototypes
\*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include "plplot_octave_rej.h"

	/* All void types */

	/* C routines callable from stub routines come first */

/* Advance to subpage "page", or to the next one if "page" = 0. */

void c_pladv(PLINT page); //%name pladv

/* simple arrow plotter. */

void plarrows(PLFLT *u, PLFLT *v, PLFLT *x, PLFLT *y, PLINT n,
         PLFLT scale, PLFLT dx, PLFLT dy) ;

/* This functions similarly to plbox() except that the origin of the axes */
/* is placed at the user-specified point (x0, y0). */

void c_plaxes(PLFLT x0, PLFLT y0, const char *xopt, PLFLT xtick, PLINT nxsub,
	 const char *yopt, PLFLT ytick, PLINT nysub); //%name plaxes

/* Plot a histogram using x to store data values and y to store frequencies */

void c_plbin(PLINT nbin, PLFLT *x, PLFLT *y, PLINT center); //%name plbin //%input x(nbin), y(nbin)

/* Start new page.  Should only be used with pleop(). */

void c_plbop(void); //%name plbop

/* This draws a box around the current viewport. */

void c_plbox(const char *xopt, PLFLT xtick, PLINT nxsub,
	const char *yopt, PLFLT ytick, PLINT nysub); //%name plbox

/* This is the 3-d analogue of plbox(). */

void c_plbox3(const char *xopt, const char *xlabel, PLFLT xtick, PLINT nsubx,
	 const char *yopt, const char *ylabel, PLFLT ytick, PLINT nsuby,
	 const char *zopt, const char *zlabel, PLFLT ztick, PLINT nsubz); //%name plbox3

/* Set xor mode; 1-enter, 0-leave */

void c_plxormod(PLINT mode); //%name plxormod

/* Set color, map 0.  Argument is integer between 0 and 15. */

void c_plcol0(PLINT icol0); //%name plcol0

inline void my_plcol(PLINT icol0) {
	c_plcol0(icol0);} //%name plcol

/* Set color, map 1.  Argument is a float between 0. and 1. */

void c_plcol1(PLFLT col1); //%name plcol1

/* Identity transformation. */

void pltr0(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data);  //%nowrap

/* Does linear interpolation from singly dimensioned coord arrays. */

void pltr1(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data); //%nowrap

/* Does linear interpolation from doubly dimensioned coord arrays */
/* (column dominant, as per normal C 2d arrays). */

void pltr2(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data); //%nowrap

/* Just like pltr2() but uses pointer arithmetic to get coordinates from */
/* 2d grid tables.  */

void pltr2p(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data); //%nowrap

/* One more hack. As it is not possible (and would not be desirable) to pass
 * an Octave function to plcont(), I have defined three plcont():
 *	plcont uses a defined here xform()
 *	plcont0 uses pltr0()
 *	plcont1 uses pltr1() 
 *	plcont2 uses pltr2()
 *	plcont2p uses pltr2p()
 *
 * Also, as plplot expect vectorized bidimensional arrays, I provided a
 * f2c, which is a #define that does the necessary conversion.
*/

/* static PLFLT _pl_tr[6];*/

void xform(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data) {
	*tx = *((PLFLT *)pltr_data+0) * x + *((PLFLT *)pltr_data+1) * y + *((PLFLT *)pltr_data+2);
	*ty = *((PLFLT *)pltr_data+3) * x + *((PLFLT *)pltr_data+4) * y + *((PLFLT *)pltr_data+5);} //%nowrap

/* convert from bidimensional array f(nx,ny) to vectorized array ff(nx,ny) */

#define	 f2c(f, ff, nx, ny) \
	PLFLT   **ff; \
	ff = (PLFLT **)alloca(nx*sizeof(PLFLT *)); \
	for (int i=0; i<nx; i++){	\
		ff[i] = (PLFLT *)alloca(ny*sizeof(PLFLT)); \
		for (int j=0;j<ny;j++) \
			*(ff[i]+j) =  *(f+nx*j+i);}

/* simpler plcont() for use with xform() */
	 
void my_plcont(PLFLT *f, PLINT nx, PLINT ny, PLINT kx, PLINT lx, PLINT ky,
	PLINT ly, PLFLT *clevel, PLINT nlevel, PLFLT *tr) {
	f2c(f, ff, nx,ny);
	c_plcont(ff, nx, ny, kx, lx, ky, ly, clevel, nlevel, xform, tr);
}  //%name plcont //%input f(nx,ny), clevel(nlevel), tr(6)

/* plcont() for use with pltr0() NOT TESTED */
	 
void my_plcont0(PLFLT *f, PLINT nx, PLINT ny, PLINT kx, PLINT lx, PLINT ky,
	PLINT ly, PLFLT *clevel, PLINT nlevel) {
	f2c(f, ff, nx,ny);
	c_plcont(ff, nx, ny, kx, lx, ky, ly, clevel, nlevel, pltr0, NULL);
}  //%name plcont0 //%input f(nx,ny), clevel(nlevel)

/* plcont() for use with pltr1() */
	 
void my_plcont1(PLFLT *f, PLINT nx, PLINT ny, PLINT kx, PLINT lx, PLINT ky,
	PLINT ly, PLFLT *clevel, PLINT nlevel, PLFLT *xg, PLFLT *yg) {
	PLcGrid grid1;
	grid1.nx = nx;	grid1.ny = ny;
	grid1.xg = xg;	grid1.yg = yg;
	f2c(f, ff, nx,ny);
	c_plcont(ff, nx, ny, kx, lx, ky, ly, clevel, nlevel, pltr1, &grid1);
}  //%name plcont1 //%input f(nx,ny), clevel(nlevel), xg(nx), yg(ny)

/* plcont() for use with pltr2() */
	 
void my_plcont2(PLFLT *f, PLINT nx, PLINT ny, PLINT kx, PLINT lx, PLINT ky,
	PLINT ly, PLFLT *clevel, PLINT nlevel, PLFLT *xg, PLFLT *yg) {
	PLcGrid2 grid2;
	f2c(xg, xgg, nx, ny); f2c(yg, ygg, nx, ny);
	grid2.nx = nx;	grid2.ny = ny;
	grid2.xg = xgg;	grid2.yg = ygg;
	f2c(f, ff, nx,ny);
	c_plcont(ff, nx, ny, kx, lx, ky, ly, clevel, nlevel, pltr2, &grid2);
}  //%name plcont2 //%input f(nx,ny), clevel(nlevel), xg(nx,ny), yg(nx,ny)

/* plcont() for use with pltr2p() */
	 
void my_plcont2p(PLFLT *f, PLINT nx, PLINT ny, PLINT kx, PLINT lx, PLINT ky,
	PLINT ly, PLFLT *clevel, PLINT nlevel, PLFLT *xg, PLFLT *yg) {
	PLcGrid2 grid2;
	f2c(xg, xgg, nx, ny); f2c(yg, ygg, nx, ny);
	grid2.nx = nx;	grid2.ny = ny;
	grid2.xg = xgg;	grid2.yg = ygg;
	f2c(f, ff, nx,ny);
	c_plcont(ff, nx, ny, kx, lx, ky, ly, clevel, nlevel, pltr2, &grid2);
}  //%name plcont2p //%input f(nx,ny), clevel(nlevel), xg(nx,ny), yg(nx,ny)

/* Copies state parameters from the reference stream to the current stream. */

void c_plcpstrm(PLINT iplsr, PLINT flags); //%name plcpstrm

/* Converts input values from relative device coordinates to relative plot */
/* coordinates. */

void pldid2pc(PLFLT *xmin, PLFLT *ymin, PLFLT *xmax, PLFLT *ymax);

/* Converts input values from relative plot coordinates to relative */
/* device coordinates. */

void pldip2dc(PLFLT *xmin, PLFLT *ymin, PLFLT *xmax, PLFLT *ymax);

/* End a plotting session for all open streams. */

void c_plend(void); //%name plend

/* End a plotting session for the current stream only. */

void c_plend1(void); //%name plend1

/* Simple interface for defining viewport and window. */

void c_plenv(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	PLINT just, PLINT axis); //%name plenv

/* End current page.  Should only be used with plbop(). */

void c_pleop(void); //%name pleop

/* Plot horizontal error bars (xmin(i),y(i)) to (xmax(i),y(i)) */

void c_plerrx(PLINT n, PLFLT *xmin, PLFLT *xmax, PLFLT *y); //%name plerrx //%input xmin(n), xmax(n), y(n)

/* Plot vertical error bars (x,ymin(i)) to (x(i),ymax(i)) */

void c_plerry(PLINT n, PLFLT *x, PLFLT *ymin, PLFLT *ymax);  //%name plerry //%input ymin(n), ymax(n), x(n)

/* Advance to the next family file on the next new page */

void c_plfamadv(void); //%name plfamadv

/* Pattern fills the polygon bounded by the input points. */

void c_plfill(PLINT n, PLFLT *x, PLFLT *y); //%name plfill //%input x(n), y(n)

/* Pattern fills the 3d polygon bounded by the input points. */

void c_plfill3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z); //%name plfill3 //%input x(n), y(n), z(n)

/* Flushes the output stream.  Use sparingly, if at all. */

void c_plflush(void); //%name plflush

/* Sets the global font flag to 'ifont'. */

void c_plfont(PLINT ifont); //%name plfont

/* Load specified font set. */

void c_plfontld(PLINT fnt); //%name plfontld

/* Get character default height and current (scaled) height */

void c_plgchr(PLFLT *p_def, PLFLT *p_ht); //%name plgchr //%output p_def, p_ht

/* Returns 8 bit RGB values for given color from color map 0 */

void c_plgcol0(PLINT icol0, PLINT *r, PLINT *g, PLINT *b); //%name plgcol0 //%output r, g, b

/* Returns the background color by 8 bit RGB value */

void c_plgcolbg(PLINT *r, PLINT *g, PLINT *b); //%name plgcolbg //%output r, g, b

/* Returns the current compression setting */

void c_plgcompression(PLINT *compression); //%name plgcompression

/* Get the current device (keyword) name */

void c_plgdev(char *p_dev);	 //%name plgdev //%output p_dev(80)

/* Retrieve current window into device space */

void c_plgdidev(PLFLT *p_mar, PLFLT *p_aspect, PLFLT *p_jx, PLFLT *p_jy); //%name plgdidev //%output p_mar, p_aspect, p_jx, p_jy

/* Get plot orientation */

void c_plgdiori(PLFLT *p_rot); //%name plgdiori //%output p_rot

/* Retrieve current window into plot space */

void c_plgdiplt(PLFLT *p_xmin, PLFLT *p_ymin, PLFLT *p_xmax, PLFLT *p_ymax); //%name plgdiplt //%output p_xmin, p_ymin, p_xmax, p_ymax

/* Get family file parameters */

void c_plgfam(PLINT *p_fam, PLINT *p_num, PLINT *p_bmax); //%name plgfam //%output p_fam, p_num, p_bmax

/* Get the (current) output file name.  Must be preallocated to >80 bytes */

void c_plgfnam(char *fnam); //%name plgfnam //%output fnam(80)

/* Get the (current) run level.  */

void	c_plglevel(PLINT *p_level); //%name plglevel //%output p_level

/* Get output device parameters. */

void c_plgpage(PLFLT *p_xp, PLFLT *p_yp, PLINT *p_xleng, PLINT *p_yleng,
	PLINT *p_xoff, PLINT *p_yoff); //%name plgpage //%output p_xp, p_yp, p_xleng, p_yleng, p_xoff, p_yoff

/* Switches to graphics screen. */

void c_plgra(void); //%name plgra

/* Get subpage boundaries in absolute coordinates */

void c_plgspa(PLFLT *xmin, PLFLT *xmax, PLFLT *ymin, PLFLT *ymax); //%name plgspa //%output xmin, xmax, ymin, ymax

/* Get current stream number. */

void c_plgstrm(PLINT *p_strm); //%name plgstrm //%output p_strm

/* Get the current library version number */

void c_plgver(char *p_ver); //%output p_ver(20) //%name plgver 

/* Get x axis labeling parameters */

void c_plgxax(PLINT *p_digmax, PLINT *p_digits); //%name plgxax //%output p_digmax, p_digits

/* Get y axis labeling parameters */

void c_plgyax(PLINT *p_digmax, PLINT *p_digits); //%name plgyax //%output p_digmax, p_digits

/* Get z axis labeling parameters */

void c_plgzax(PLINT *p_digmax, PLINT *p_digits); //%name plgzax //%output p_digmax, p_digits

/* Draws a histogram of n values of a variable in array data[0..n-1] */

void c_plhist(PLINT n, PLFLT *data, PLFLT datmin, PLFLT datmax,
	 PLINT nbin, PLINT oldwin); //%name plhist //%input data(n)

/* Set current color (map 0) by hue, lightness, and saturation. */

void c_plhls(PLFLT h, PLFLT l, PLFLT s); //%name plhls

/* Initializes PLplot, using preset or default options */

void c_plinit(void); //%name plinit

/* Draws a line segment from (x1, y1) to (x2, y2). */

void c_pljoin(PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2); //%name pljoin

/* Simple routine for labelling graphs. */

void c_pllab(const char *xlabel, const char *ylabel, const char *tlabel); //%name pllab

/* Sets position of the light source */

void c_pllightsource(PLFLT x, PLFLT y, PLFLT z); //%name pllightsource

/* Draws line segments connecting a series of points. */

void c_plline(PLINT n, PLFLT *x, PLFLT *y); //%input x(n), y(n) //%name plline

/* Draws a line in 3 space.  */

void c_plline3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z); //%name plline3 //%input x(n), y(n), z(n)

/* Set line style. */

void c_pllsty(PLINT lin); //%name pllsty

/* Plots a mesh representation of the function z[x][y]. */

void c_plmesh(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt); //%nowrap

void my_plmesh(PLFLT *x, PLFLT *y, PLFLT *z, PLINT nx, PLINT ny, PLINT opt) {
	f2c(z, zz, nx, ny);
	c_plmesh(x, y, zz, nx, ny, opt);} //%name plmesh //%input x(nx), y(ny), z(nx,ny)

/* Creates a new stream and makes it the default.  */

void c_plmkstrm(PLINT *p_strm); //%name plmkstrm //%output p_strm

/* Prints out "text" at specified position relative to viewport */

void c_plmtex(const char *side, PLFLT disp, PLFLT pos, PLFLT just,
	 const char *text); //%name plmtex

/* Plots a 3-d representation of the function z[x][y]. */

void c_plot3d(PLFLT *x, PLFLT *y, PLFLT **z,
	 PLINT nx, PLINT ny, PLINT opt, PLINT side); //%nowrap

void my_plot3d(PLFLT *x, PLFLT *y, PLFLT *z,
	PLINT nx, PLINT ny, PLINT opt, PLINT side) {
	f2c(z, zz, nx, ny)
	c_plot3d(x, y, zz, nx, ny, opt, side);
} //%name plot3d //%input x(nx), y(ny), z(nx,ny)

/* Plots a 3-d shaded representation of the function z[x][y]. */

void c_plotsh3d(PLFLT *x, PLFLT *y, PLFLT **z,
	PLINT nx, PLINT ny, PLINT side); //%nowrap

void my_plotsh3d(PLFLT *x, PLFLT *y, PLFLT *z,
	PLINT nx, PLINT ny, PLINT side) {
	f2c(z, zz, nx, ny)
	c_plotsh3d(x, y, zz, nx, ny, side);
} //%name plotsh3d //%input x(nx), y(ny), z(nx,ny)
	
/* Set fill pattern directly. */

void c_plpat(PLINT nlin, PLINT *inc, PLINT *del); //%name plpat //%input inc(nlin), del(nlin)

/* Plots array y against x for n points using ASCII code "code".*/

void c_plpoin(PLINT n, PLFLT *x, PLFLT *y, PLINT code); //%input x(n), y(n) //%name plpoin

/* Draws a series of points in 3 space. */

void c_plpoin3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT code); //%name plpoin3 //%input x(n), y(n), z(n)

/* Draws a polygon in 3 space.  */

void c_plpoly3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT *draw); //%nowrap

void my_plpoly3(PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT *draw, PLINT clockwise) {
	c_plpoly3(n*clockwise,x,y,z,draw); } //%name plpoly3 //%input x(n), y(n), z(n), draw(4)

/* Set the floating point precision (in number of places) in numeric labels. */

void c_plprec(PLINT setp, PLINT prec); //%name plprec

/* Set fill pattern, using one of the predefined patterns.*/

void c_plpsty(PLINT patt); //%name plpsty

/* Prints out "text" at world cooordinate (x,y). */

void c_plptex(PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just, const char *text); //%name plptex

/* Replays contents of plot buffer to current device/file. */

void c_plreplot(void); //%name plreplot

/* Set line color by red, green, blue from  0. to 1. */

void c_plrgb(PLFLT r, PLFLT g, PLFLT b); //%name plrgb

/* Set line color by 8 bit RGB values. */

void c_plrgb1(PLINT r, PLINT g, PLINT b); //%name plrgb1

/* Set character height. */

void c_plschr(PLFLT def, PLFLT scale); //%name plschr

/* Set number of colors in cmap 0 */

void c_plscmap0n(PLINT ncol0); //%name plscmap0n

/* Set number of colors in cmap 1 */

void c_plscmap1n(PLINT ncol1); //%name plscmap1n

/* Set color map 0 colors by 8 bit RGB values */

void c_plscmap0(PLINT *r, PLINT *g, PLINT *b, PLINT ncol0); //%name plscmap0 //%input r(ncol0), g(ncol0), b(ncol0)

/* Set color map 1 colors by 8 bit RGB values */

void c_plscmap1(PLINT *r, PLINT *g, PLINT *b, PLINT ncol1); //%name plscmap1 //%input r(ncol1), g(ncol1), b(ncol1)

/* Set color map 1 colors using a piece-wise linear relationship between */
/* intensity [0,1] (cmap 1 index) and position in HLS or RGB color space. */

void c_plscmap1l(PLINT itype, PLINT npts, PLFLT *intensity,
	    PLFLT *coord1, PLFLT *coord2, PLFLT *coord3, PLINT *rev);
//%name plscmap1l //%input intensity(npts), coord1(npts), coord2(npts), coord3(npts), rev(npts)

/* Set a given color from color map 0 by 8 bit RGB value */

void c_plscol0(PLINT icol0, PLINT r, PLINT g, PLINT b); //%name plscol0

/* Set the background color by 8 bit RGB value */

void c_plscolbg(PLINT r, PLINT g, PLINT b); //%name plscolbg

/* Used to globally turn color output on/off */

void c_plscolor(PLINT color); //%name plscolor

/* Set the compression level */

void c_plscompression(PLINT compression); //%name plscompression

/* Set the device (keyword) name */

void c_plsdev(const char *devname); //%name plsdev

/* Set window into device space using margin, aspect ratio, and */
/* justification */

void c_plsdidev(PLFLT mar, PLFLT aspect, PLFLT jx, PLFLT jy); //%name plsdidev

/* Set up transformation from metafile coordinates. */

void c_plsdimap(PLINT dimxmin, PLINT dimxmax, PLINT dimymin, PLINT dimymax,
	   PLFLT dimxpmm, PLFLT dimypmm); //%name plsdimap

/* Set plot orientation, specifying rotation in units of pi/2. */

void c_plsdiori(PLFLT rot); //%name plsdiori

/* Set window into plot space */

void c_plsdiplt(PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax); //%name plsdiplt

/* Set window into plot space incrementally (zoom) */

void c_plsdiplz(PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax); //%name plsdiplz

/* Set the escape character for text strings. */

void c_plsesc(char esc); //%name plsesc

/* set offset and spacing of contour labels */

void c_pl_setcontlabelparam(PLFLT offset, PLFLT size, PLFLT spacing, PLINT active); //%name plclabel_param

/* set the format of the contour labels */

void c_pl_setcontlabelformat(PLINT lexp, PLINT sigdig); //%name plclabel_format

/* Set family file parameters */

void c_plsfam(PLINT fam, PLINT num, PLINT bmax); //%name plsfam

/* Set the output file name. */

void c_plsfnam(const char *fnam); //%name plsfnam

/* The same as in plcont. I have hardcoded the first function pointer
 * to plfill(). The second function pointer will use the same convention
 * as in plcont().
*/

/* the simpler plshade() */

//void my_plshade(PLFLT *a, PLINT nx, PLINT ny, const char *defined,
void my_plshade(PLFLT *a, PLINT nx, PLINT ny, PLFLT *defined,
	 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	 PLFLT shade_min, PLFLT shade_max,
	 PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
	 PLINT min_color, PLINT min_width,
	 PLINT max_color, PLINT max_width,
	 PLINT rectangular, PLFLT *tr) {

	f2c(a,aa,nx,ny);
	c_plshade(aa, nx, ny, NULL, left, right, bottom, top,
		shade_min, shade_max, sh_cmap, sh_color, sh_width,
		min_color, min_width, max_color, max_width, 
		plfill, rectangular, xform, tr);
}  //%name plshade //%input a(nx, ny), tr(6)

/*  plshade() for use with pltr1 */
	  
void my_plshade1(PLFLT *a, PLINT nx, PLINT ny, const char *defined,
	 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	 PLFLT shade_min, PLFLT shade_max,
	 PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
	 PLINT min_color, PLINT min_width,
	 PLINT max_color, PLINT max_width,
	 PLINT rectangular, PLFLT *xg, PLFLT *yg) {
	PLcGrid grid1;
	grid1.nx = nx;	grid1.ny = ny;
	grid1.xg = xg;	grid1.yg = yg;
	f2c(a,aa,nx,ny);
	c_plshade(aa, nx, ny, NULL, left, right, bottom, top,
		shade_min, shade_max, sh_cmap, sh_color, sh_width,
		min_color, min_width, max_color, max_width, 
		plfill, rectangular, pltr1, &grid1);
}  //%name plshade1 //%input a(nx, ny), xg(nx), yg(ny)

/*  plshade() for use with pltr2 */
	  
void my_plshade2(PLFLT *a, PLINT nx, PLINT ny, const char *defined,
	 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	 PLFLT shade_min, PLFLT shade_max,
	 PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
	 PLINT min_color, PLINT min_width,
	 PLINT max_color, PLINT max_width,
	 PLINT rectangular, PLFLT *xg, PLFLT *yg) {
	PLcGrid2 grid2;
	f2c(xg, xgg, nx, ny); f2c(yg, ygg, nx, ny);
	grid2.nx = nx;	grid2.ny = ny;
	grid2.xg = xgg;	grid2.yg = ygg;
	f2c(a,aa,nx,ny);
	c_plshade(aa, nx, ny, NULL, left, right, bottom, top,
		shade_min, shade_max, sh_cmap, sh_color, sh_width,
		min_color, min_width, max_color, max_width, 
	 	plfill, rectangular, pltr2, &grid2);
}  //%name plshade2 //%input a(nx, ny), xg(nx,ny), yg(nx,ny)

void my_plshades(PLFLT *a, PLINT nx, PLINT ny, const char *defined,
	 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
	 PLFLT *clevel, PLINT nlevel, PLINT fill_width,
	 PLINT cont_color, PLINT cont_width,
	 PLINT rectangular, PLFLT *tr) {

	f2c(a,aa,nx,ny);
	c_plshades(aa, nx, ny, NULL, left, right, bottom, top,
		clevel, nlevel, fill_width, cont_color, cont_width,
		plfill, rectangular, xform, tr);
}  //%name plshades //%input a(nx, ny), clevel(nlevel), tr(6)

/* Set up lengths of major tick marks. */

void c_plsmaj(PLFLT def, PLFLT scale); //%name plsmaj

/* Set up lengths of minor tick marks. */

void c_plsmin(PLFLT def, PLFLT scale); //%name plsmin

/* Set orientation.  Must be done before calling plinit. */

void c_plsori(PLINT ori); //%name plsori

/* Set output device parameters.  Usually ignored by the driver. */

void c_plspage(PLFLT xp, PLFLT yp, PLINT xleng, PLINT yleng,
	  PLINT xoff, PLINT yoff); //%name plspage

/* Set the pause (on end-of-page) status */

void c_plspause(PLINT pause); //%name plspause

/* Set stream number.  */

void c_plsstrm(PLINT strm); //%name plsstrm

/* Set the number of subwindows in x and y */

void c_plssub(PLINT nx, PLINT ny); //%name plssub

/* Set symbol height. */

void c_plssym(PLFLT def, PLFLT scale); //%name plssym

/* Initialize PLplot, passing in the windows/page settings. */

void c_plstar(PLINT nx, PLINT ny); //%name plstar

/* Initialize PLplot, passing the device name and windows/page settings. */

void c_plstart(const char *devname, PLINT nx, PLINT ny); //%name plstart

/* Create 1d stripchart */

void my_plstripc(int *id, char *xspec, char *yspec,
	PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
	PLFLT xlpos, PLFLT ylpos,
	PLINT y_ascl, PLINT acc,
	PLINT colbox, PLINT collab,
	PLINT *colline, PLINT *styline,
	char *legline1, char *legline2, char *legline3, char *legline4,
	char *labx, char *laby, char *labtop) {
		char *legline[4];
		legline[0]=legline1; legline[1]=legline2;
		legline[2]=legline3; legline[3]=legline4;
		c_plstripc(id, xspec, yspec, xmin, xmax, xjump, ymin, ymax,
		xlpos, ylpos, y_ascl, acc, colbox, collab, colline, styline, legline,
		labx, laby, labtop);
	} //%novectorize //%name plstripc //%output id //%input colline(4), styline(4)
	 
/* Add a point to a stripchart.  */

void c_plstripa(PLINT id, PLINT pen, PLFLT x, PLFLT y); //%name plstripa

/* Deletes and releases memory used by a stripchart.  */

void c_plstripd(PLINT id); //%name plstripd

/* Set up a new line style */

void c_plstyl(PLINT nms, PLINT *mark, PLINT *space); //%name plstyl

/* Sets the edges of the viewport to the specified absolute coordinates */

void c_plsvpa(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax); //%name plsvpa

/* Set x axis labeling parameters */

void c_plsxax(PLINT digmax, PLINT digits); //%name plsxax

/* Set inferior X window */

void plsxwin(PLINT window_id);

/* Set y axis labeling parameters */

void c_plsyax(PLINT digmax, PLINT digits); //%name plsyax

/* Plots array y against x for n points using Hershey symbol "code" */

void c_plsym(PLINT n, PLFLT *x, PLFLT *y, PLINT code); //%name plsym //%input x(n), y(n)

/* Set z axis labeling parameters */

void c_plszax(PLINT digmax, PLINT digits); //%name plszax

/* Switches to text screen. */

void c_pltext(void); //%name pltext

/* Sets the edges of the viewport with the given aspect ratio, leaving */
/* room for labels. */

void c_plvasp(PLFLT aspect); //%name plvasp

/* Creates the largest viewport of the specified aspect ratio that fits */
/* within the specified normalized subpage coordinates. */

void c_plvpas(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect); //%name plvpas

/* Creates a viewport with the specified normalized subpage coordinates. */

void c_plvpor(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax); //%name plvpor

/* Defines a "standard" viewport with seven character heights for */
/* the left margin and four character heights everywhere else. */

void c_plvsta(void); //%name plvsta

/* Set up a window for three-dimensional plotting. */

void c_plw3d(PLFLT basex, PLFLT basey, PLFLT height, PLFLT xmin0,
	PLFLT xmax0, PLFLT ymin0, PLFLT ymax0, PLFLT zmin0,
	PLFLT zmax0, PLFLT alt, PLFLT az); //%name plw3d

/* Set pen width. */

void c_plwid(PLINT width); //%name plwid

/* Set up world coordinates of the viewport boundaries (2d plots). */

void c_plwind(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax); //%name plwind

/*--------------------------------------------------------------------------*\
 *		Functions for use from C or C++ only
\*--------------------------------------------------------------------------*/

/* Returns a list of file-oriented device names and their menu strings */

void plgFileDevs(char ***p_menustr, char ***p_devname, int *p_ndev);  //%nowrap

/* Returns a list of all device names and their menu strings */

void plgDevs(char ***p_menustr, char ***p_devname, int *p_ndev);  //%nowrap

/* Set the variables to be used for storing error info */

void plsError(PLINT *errcode, char *errmsg);

	/* Transformation routines */

/* Identity transformation for plots from Fortran. */

void pltr0f(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data); //%nowrap

/* Does linear interpolation from doubly dimensioned coord arrays */
/* (row dominant, i.e. Fortran ordering). */

void pltr2f(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data); //%nowrap

/* Example linear transformation function for contour plotter. */

// jc: defined above. void xform(PLFLT x, PLFLT y, PLFLT * tx, PLFLT * ty);

	/* Function evaluators */

/* Does a lookup from a 2d function array.  Array is of type (PLFLT **), */
/* and is column dominant (normal C ordering). */

PLFLT plf2eval2(PLINT ix, PLINT iy, PLPointer plf2eval_data);  //%nowrap

/* Does a lookup from a 2d function array.  Array is of type (PLFLT *), */
/* and is column dominant (normal C ordering). */

PLFLT plf2eval(PLINT ix, PLINT iy, PLPointer plf2eval_data);  //%nowrap

/* Does a lookup from a 2d function array.  Array is of type (PLFLT *), */
/* and is row dominant (Fortran ordering). */

PLFLT plf2evalr(PLINT ix, PLINT iy, PLPointer plf2eval_data);  //%nowrap

	/* Command line parsing utilities */

/* Clear internal option table info structure. */

void plClearOpts(void);

/* Reset internal option table info structure. */

void plResetOpts(void);

/* Merge user option table into internal info structure. */

int plMergeOpts(PLOptionTable *options, char *name, char **notes);  //%nowrap

/* Set the strings used in usage and syntax messages. */

void plSetUsage(char *program_string, char *usage_string);

/* Process input strings, treating them as an option and argument pair. */

int plSetOpt(char *opt, char *optarg);

/* Process options list using current options info. */

int plParseOpts(int *p_argc, char **argv, PLINT mode); //%novectorize

/* Print usage & syntax message. */

void plOptUsage(void);

	/* Miscellaneous */

/* Set the output file pointer */

void plgfile(FILE **p_file);  //%nowrap

/* Get the output file pointer */

void plsfile(FILE *file); 	  //%nowrap

/* Get the escape character for text strings. */

void plgesc(char *p_esc); //%nowrap

/* Front-end to driver escape function. */

void pl_cmd(PLINT op, void *ptr);  //%nowrap

/* Return full pathname for given file if executable */

int plFindName(char *p); //%nowrap

/* Looks for the specified executable file according to usual search path. */

char *plFindCommand(char *fn);  //%nowrap

/* Gets search name for file by concatenating the dir, subdir, and file */
/* name, allocating memory as needed.  */

void plGetName(char *dir, char *subdir, char *filename, char **filespec); //%nowrap

/* Prompts human to input an integer in response to given message. */

PLINT plGetInt(char *s);

/* Prompts human to input a float in response to given message. */

PLFLT plGetFlt(char *s);

	/* Nice way to allocate space for a vectored 2d grid */

/* Allocates a block of memory for use as a 2-d grid of PLFLT's.  */

void plAlloc2dGrid(PLFLT ***f, PLINT nx, PLINT ny); //%nowrap

/* Frees a block of memory allocated with plAlloc2dGrid(). */

void plFree2dGrid(PLFLT **f, PLINT nx, PLINT ny);  //%nowrap

/* Functions for converting between HLS and RGB color space */

void plHLS_RGB(PLFLT h, PLFLT l, PLFLT s, PLFLT *p_r, PLFLT *p_g, PLFLT *p_b); //%output p_r, p_g, p_b

void plRGB_HLS(PLFLT r, PLFLT g, PLFLT b, PLFLT *p_h, PLFLT *p_l, PLFLT *p_s); //%output p_h, p_l, p_s

/* Wait for graphics input event and translate to world coordinates */

int plGetCursor(PLGraphicsIn *gin); //%nowrap

int my_plGetCursor(int *state, int *keysym, int *button, char *string, int *pX, int *pY, PLFLT *dX, PLFLT *dY, PLFLT *wX, PLFLT *wY) {
	PLGraphicsIn gin;
	int	status; status=plGetCursor(&gin);
	*state=gin.state; *keysym=gin.keysym; *button=gin.button; *string=gin.string[0];
	*pX=gin.pX; *pY=gin.pY; *dX=gin.dX; *dY=gin.dY; *wX=gin.wX; *wY=gin.wY;
	return status;
} //%name plGetCursor //%output state, keysym, button, string(1), pX, pY, dX, dY,  wX, wY

/* Translates relative device coordinates to world coordinates.  */

int plTranslateCursor(PLGraphicsIn *gin); //%nowrap

int my_plTranslateCursor(PLFLT *x, PLFLT *y, PLFLT x_in, PLFLT y_in) {
	PLGraphicsIn	gin;
	int	st;
	gin.dX = x_in; gin.dY = y_in;
	st = plTranslateCursor(&gin);
	*x = gin.wX; *y = gin.wY;
	return st;
} //%name plTranslateCursor //%output x, y

#ifdef __cplusplus
}
#endif

#endif	/* __PLPLOT_H__ */

