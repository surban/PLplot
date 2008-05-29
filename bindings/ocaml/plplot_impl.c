/*
Copyright 2007  Hezekiah M. Carty

This file is part of ocaml-plplot.

ocaml-plplot is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with ocaml-plplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <plplotP.h>

/* The "usual" OCaml includes */
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/misc.h>
#include <caml/mlvalues.h>
#include <caml/bigarray.h>

#include <plplot.h>

#include <stdio.h>

#define MAX_EXCEPTION_MESSAGE_LENGTH 1000
#define CAML_PLPLOT_PLOTTER_FUNC_NAME "caml_plplot_plotter"
#define CAML_PLPLOT_MAPFORM_FUNC_NAME "caml_plplot_mapform"
#define CAML_PLPLOT_DEFINED_FUNC_NAME "caml_plplot_defined"

typedef void(*ML_PLOTTER_FUNC)(PLFLT, PLFLT, PLFLT*, PLFLT*, PLPointer);
typedef PLINT(*ML_DEFINED_FUNC)(PLFLT, PLFLT);
typedef void(*ML_MAPFORM_FUNC)(PLINT, PLFLT*, PLFLT*);

/*

  CALLBACK WRAPPERS

 */

// A simple routine to wrap a properly registered OCaml callback in a form
// usable by PLPlot routines.  If an appropriate callback is not registered
// then the PLPlot built-in pltr0 function is used instead.
void ml_plotter(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data) {
    CAMLparam0();
    CAMLlocal1(result);

    // Get the OCaml callback function (if there is one)
    static value * pltr = NULL;
    if (pltr == NULL)
        pltr = caml_named_value(CAML_PLPLOT_PLOTTER_FUNC_NAME);

    // No check to see if a callback function has been designated yet,
    // because that is checked before we get to this point.
    result =
        caml_callback2(*pltr, caml_copy_double(x), caml_copy_double(y));
    double new_x, new_y;
    new_x = Double_val( Field(result, 0) );
    new_y = Double_val( Field(result, 1) );

    *tx = new_x;
    *ty = new_y;

    CAMLreturn0;
}

// A simple routine to wrap a properly registered OCaml callback in a form
// usable by PLPlot routines.  If an appropriate callback is not registered
// then the result is always 1 (the data point is defined).
// This function is used in the plshade* functions to determine if a given data
// point is valid/defined or not.
PLINT ml_defined(PLFLT x, PLFLT y) {
    CAMLparam0();
    CAMLlocal1(result);

    // The result which will be returned to the user.
    PLINT is_it_defined;

    // Get the OCaml callback function (if there is one)
    static value * defined = NULL;
    if (defined == NULL)
        defined = caml_named_value(CAML_PLPLOT_DEFINED_FUNC_NAME);

    // No check to see if a callback function has been designated yet,
    // because that is checked before we get to this point.
    result =
        caml_callback2(*defined, caml_copy_double(x), caml_copy_double(y));
    is_it_defined = Int_val(result);

    CAMLreturn(is_it_defined);
}

// A simple routine to wrap a properly registered OCaml callback in a form
// usable by PLPlot routines.  If an appropriate callback is not registered
// then nothing is done.
void ml_mapform(PLINT n, PLFLT *x, PLFLT *y) {
    CAMLparam0();
    CAMLlocal1(result);

    // Get the OCaml callback function (if there is one)
    static value * mapform = NULL;
    if (mapform == NULL)
        mapform = caml_named_value(CAML_PLPLOT_MAPFORM_FUNC_NAME);

    // No check to see if a callback function has been designated yet,
    // because that is checked before we get to this point.
    int i;
    for (i = 0; i < n; i++) {
        result =
            caml_callback2(*mapform,
                    caml_copy_double(x[i]), caml_copy_double(y[i]));

        double new_x, new_y;
        new_x = Double_val( Field(result, 0) );
        new_y = Double_val( Field(result, 1) );

        x[i] = new_x;
        y[i] = new_y;
    }

    CAMLreturn0;
}

// Check if the matching OCaml callback is defined.  Return NULL if it is not,
// and the proper function pointer if it is.
ML_PLOTTER_FUNC get_ml_plotter_func() {
    static value * pltr = NULL;
    if (pltr == NULL)
        pltr = caml_named_value(CAML_PLPLOT_PLOTTER_FUNC_NAME);

    if (pltr == NULL || Val_int(0) == *pltr) {
        // No plotter defined
        return NULL;
    }
    else {
        // Plotter is defined
        return ml_plotter;
    }
}
ML_DEFINED_FUNC get_ml_defined_func() {
    static value * defined = NULL;
    if (defined == NULL)
        defined = caml_named_value(CAML_PLPLOT_DEFINED_FUNC_NAME);

    if (defined == NULL || Val_int(0) == *defined) {
        // No plotter defined
        return NULL;
    }
    else {
        // Plotter is defined
        return ml_defined;
    }
}
ML_MAPFORM_FUNC get_ml_mapform_func() {
    static value * mapform = NULL;
    if (mapform == NULL)
        mapform = caml_named_value(CAML_PLPLOT_MAPFORM_FUNC_NAME);

    if (mapform == NULL || Val_int(0) == *mapform) {
        // No plotter defined
        return NULL;
    }
    else {
        // Plotter is defined
        return ml_mapform;
    }
}


/*

  CONTOURING, SHADING and IMAGE FUNCTIONS

*/

/*
 void
c_plcont(PLFLT **f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
  PLINT ky, PLINT ly, PLFLT *clevel, PLINT nlevel,
  void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
  PLPointer pltr_data);
*/
void ml_plcont(PLFLT **f, PLINT nx, PLINT ny,
               PLINT kx, PLINT lx, PLINT ky, PLINT ly,
               PLFLT *clevel, PLINT nlevel) {
    if (get_ml_plotter_func() == NULL) {
        // This is handled in PLplot, but the error is raised here to clarify
        // what the user needs to do since the custom plotter is defined
        // separately from the call to plcont.
        caml_invalid_argument("A custom plotter must be defined \
                               before calling plcont");
    }
    else {
        c_plcont(f, nx, ny, kx, lx, ky, ly, clevel, nlevel,
                 get_ml_plotter_func(), (void*)1);
    }
}

/*
 void
c_plshade(PLFLT **a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
   PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
   PLFLT shade_min, PLFLT shade_max,
   PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
   PLINT min_color, PLINT min_width,
   PLINT max_color, PLINT max_width,
   void (*fill) (PLINT, PLFLT *, PLFLT *), PLBOOL rectangular,
   void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
   PLPointer pltr_data);
*/
void ml_plshade(PLFLT **a, PLINT nx, PLINT ny,
                PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                PLFLT shade_min, PLFLT shade_max,
                PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
                PLINT min_color, PLINT min_width,
                PLINT max_color, PLINT max_width,
                PLBOOL rectangular) {
    c_plshade(a, nx, ny,
              get_ml_defined_func(),
              left, right, bottom, top,
              shade_min, shade_max,
              sh_cmap, sh_color, sh_width, min_color, min_width,
              max_color, max_width, plfill, rectangular,
              get_ml_plotter_func(), (void*)1);
}

/*
 void
c_plshade1(PLFLT *a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
  PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
  PLFLT shade_min, PLFLT shade_max,
  PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
  PLINT min_color, PLINT min_width,
  PLINT max_color, PLINT max_width,
  void (*fill) (PLINT, PLFLT *, PLFLT *), PLBOOL rectangular,
  void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
  PLPointer pltr_data);
*/

/*
 void
c_plshades( PLFLT **a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
   PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
   PLFLT *clevel, PLINT nlevel, PLINT fill_width,
   PLINT cont_color, PLINT cont_width,
   void (*fill) (PLINT, PLFLT *, PLFLT *), PLBOOL rectangular,
   void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
   PLPointer pltr_data);
*/
void ml_plshades(PLFLT **a, PLINT nx, PLINT ny,
                 PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                 PLFLT *clevel, PLINT nlevel, PLINT fill_width,
                 PLINT cont_color, PLINT cont_width,
                 PLBOOL rectangular) {
    c_plshades(a, nx, ny,
               get_ml_defined_func(),
               xmin, xmax, ymin, ymax,
               clevel, nlevel, fill_width,
               cont_color, cont_width,
               plfill, rectangular,
               get_ml_plotter_func(),
               (void*)1);
}

/*
 void
c_plimagefr(PLFLT **idata, PLINT nx, PLINT ny,
        PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
        PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax, 
        PLFLT valuemin, PLFLT valuemax);
*/
void ml_plimagefr(PLFLT **idata, PLINT nx, PLINT ny,
                  PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                  PLFLT zmin, PLFLT zmax,
                  PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax,
                  PLFLT valuemin, PLFLT valuemax) {
    c_plimagefr(idata, nx, ny,
                xmin, xmax, ymin, ymax,
                zmin, zmax,
                Dxmin, Dxmax, Dymin, Dymax,
                valuemin, valuemax);
}

/*
 void
c_plvect(PLFLT **u, PLFLT **v, PLINT nx, PLINT ny, PLFLT scale,
 void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
        PLPointer pltr_data);
*/
void ml_plvect(PLFLT **u, PLFLT **v, PLINT nx, PLINT ny, PLFLT scale) {
    c_plvect(u, v, nx, ny, scale,
             get_ml_plotter_func(),
             (void*)1);
}

/*
 void
c_plmap( void (*mapform)(PLINT, PLFLT *, PLFLT *), const char *type,
         PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat );
*/
void ml_plmap(const char *type,
              PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat ) {
    c_plmap(get_ml_mapform_func(),
            type, minlong, maxlong, minlat, maxlat);
}

/*
 void
c_plmeridians( void (*mapform)(PLINT, PLFLT *, PLFLT *),
               PLFLT dlong, PLFLT dlat,
               PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat );
*/
void ml_plmeridians(PLFLT dlong, PLFLT dlat,
                    PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat) {
    c_plmeridians(get_ml_mapform_func(),
                  dlong, dlat, minlong, maxlong, minlat, maxlat);
}

/*
 void
c_plgriddata(PLFLT *x, PLFLT *y, PLFLT *z, PLINT npts,
    PLFLT *xg, PLINT nptsx, PLFLT *yg, PLINT nptsy,
    PLFLT **zg, PLINT type, PLFLT data);
*/
// This one is currently wrapped by hand, as I am not sure how to get camlidl
// to allocate zg in a way that makes plgriddata happy and doesn't require the
// user to pre-allocate the space.
value ml_plgriddata(value x, value y, value z,
                    value xg, value yg,
                    value type, value data) {
    CAMLparam5(x,y,z,xg,yg);
    CAMLxparam2(type,data);

    // zg holds the OCaml float array array.
    // y_ml_array is a temporary structure which will be used to form each
    // float array making up zg.
    CAMLlocal2(zg, y_ml_array);

    PLFLT **zg_local;

    int npts, nptsx, nptsy;
    int i, j;

    // Check to make sure x, y and z are all the same length.
    npts = Wosize_val(x) / Double_wosize;
    if ((Wosize_val(y) / Double_wosize != Wosize_val(z) / Double_wosize) ||
        (Wosize_val(y) / Double_wosize != npts) ||
        (Wosize_val(z) / Double_wosize != npts)
        ) {
        caml_failwith("ml_plgriddata: x, y, z must all have the same dimensions");
    }

    nptsx = Wosize_val(xg) / Double_wosize;
    nptsy = Wosize_val(yg) / Double_wosize;

    // Allocate the 2D grid in a way that will make PLplot happy
    plAlloc2dGrid(&zg_local, nptsx, nptsy);

    // Using "type + 1" because "type" is passed in as a variant type, so
    // the indexing starts from 0 rather than 1.
    c_plgriddata((double*)x, (double*)y, (double*)z, npts, (double*)xg, nptsx,
                 (double*)yg, nptsy, zg_local, Int_val(type) + 1,
                 Double_val(data));

    // Allocate the X-dimension of the to-be-returned OCaml array
    zg = caml_alloc(nptsx, 0);

    for (i = 0; i < nptsx; i++) {
        // Allocate each Y-dimension array of the OCaml array
        y_ml_array = caml_alloc(nptsy * Double_wosize, Double_array_tag);
        for (j = 0; j < nptsy; j++) {
            Store_double_field(y_ml_array, j, zg_local[i][j]);
        }
        caml_modify(&Field(zg, i), y_ml_array);
    }

    // Free the memory used by the C array
    plFree2dGrid(zg_local, nptsx, nptsy);

    CAMLreturn(zg);
}

value ml_plgriddata_bytecode(value* argv, int argn) {
    return ml_plgriddata(argv[0], argv[1], argv[2], argv[3], argv[4],
                         argv[5], argv[6]);
}



/* XXX Non-core functions follow XXX */
/**
 * The following functions are here for (my?) convenience.  As far as I can
 * tell, they are not defined in the core PLplot library.
 */

/* Get the current color map 0 color index */
int plg_current_col0(void) {
    return plsc->icol0;
}

/* Get the current color map 1 color index */
float plg_current_col1(void) {
    return plsc->icol1;
}

/* Get the current pen width. TODO: Remove this, as I think this information
   can be retrieved from another proper PLplot function. */
int plgwid(void) {
    return plsc->width;
}

/* Get the current character (text) height in mm.  TODO: Remove this, as I
   think this information can be retrieved from another proper PLplot
   function */
float plgchrht(void) {
    return plsc->chrht;
}
