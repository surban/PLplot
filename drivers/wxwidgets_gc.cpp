
#include "plDevs.h"

#ifdef PLD_wxwidgets

/* plplot headers */
#include "plplotP.h"

/* os specific headers */
#ifdef __WIN32__
  #include <windows.h>
#endif

/* wxwidgets headers */
#include "wx/wx.h"
#include "wx/except.h"
#include "wx/image.h"
#include "wx/filedlg.h"
#include "wx/display.h"
    
#include "wxwidgets.h"

#if wxUSE_GRAPHICS_CONTEXT
  
wxPLDevGC::wxPLDevGC( void ) : wxPLDevBase()
{
  m_dc=NULL;
  m_bitmap=NULL;
  m_context=NULL;
}


wxPLDevGC::~wxPLDevGC()
{
  if( ownGUI ) {
    if( m_dc ) {
        ((wxMemoryDC*)m_dc)->SelectObject( wxNullBitmap );
        delete m_dc;
    }
    if( m_bitmap )
      delete m_bitmap;
  }
  if( m_context )
    delete m_context;
}

void wxPLDevGC::DrawLine( short x1a, short y1a, short x2a, short y2a )
{
	x1a=(short)(x1a/scalex); y1a=(short)(height-y1a/scaley);
	x2a=(short)(x2a/scalex);	y2a=(short)(height-y2a/scaley);

  wxGraphicsPath path=m_context->CreatePath();
  path.MoveToPoint( x1a, y1a );
  path.AddLineToPoint( x2a, y2a );
  m_context->StrokePath( path );
}

void wxPLDevGC::DrawPolyline( short *xa, short *ya, PLINT npts )
{
	wxDouble x1a, y1a, x2a, y2a;
  
  x2a=xa[0]/scalex;
  y2a=height-ya[0]/scaley;
  wxGraphicsPath path=m_context->CreatePath();
  for( PLINT i=1; i<npts; i++ ) {
    x1a=x2a; y1a=y2a;
    path.MoveToPoint( x1a, y1a );
    x2a=xa[i]/scalex;
    y2a=height-ya[i]/scaley;

    if( !resizing && ownGUI ) 
      AddtoClipRegion( this, (int)x1a, (int)y1a, (int)x2a, (int)y2a );
  }
  path.MoveToPoint( x2a, y2a );
  m_context->StrokePath( path );
}

void wxPLDevGC::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
{
  m_dc->SetBackground( wxBrush(wxColour(bgr, bgg, bgb)) );
  m_dc->Clear();
}

void wxPLDevGC::FillPolygon( PLStream *pls )
{
  wxPoint2DDouble* points = new wxPoint2DDouble[pls->dev_npts];

  for( int i=0; i < pls->dev_npts; i++ ) {
    points[i].m_x=(int)(pls->dev_x[i]/scalex);
    points[i].m_y=(int)(height-pls->dev_y[i]/scaley);
  }

  m_context->DrawLines( pls->dev_npts, points );
  delete[] points;
}

void wxPLDevGC::BlitRectangle( wxPaintDC* dc, int vX, int vY, int vW, int vH )
{
  if( m_dc )
    dc->Blit( vX, vY, vW, vH, m_dc, vX, vY );
}

void wxPLDevGC::CreateCanvas()
{
  if( ownGUI ) {
    if( !m_dc )
      m_dc = new wxMemoryDC();

    ((wxMemoryDC*)m_dc)->SelectObject( wxNullBitmap );   /* deselect bitmap */
    if( m_bitmap )
      delete m_bitmap;
    m_bitmap = new wxBitmap( bm_width, bm_height, 32 );
    ((wxMemoryDC*)m_dc)->SelectObject( *m_bitmap );   /* select new bitmap */
    
    if( m_context )
      delete m_context;
    m_context = wxGraphicsContext::Create( m_dc );
  }
}

void wxPLDevGC::SetWidth( PLStream *pls )
{
  m_context->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b),
                                                      pls->width>0 ? pls->width : 1, wxSOLID)) );
}

void wxPLDevGC::SetColor0( PLStream *pls )
{
  m_context->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(pls->cmap0[pls->icol0].r, pls->cmap0[pls->icol0].g,
                                                               pls->cmap0[pls->icol0].b),
                                                      pls->width>0 ? pls->width : 1, wxSOLID)) );
  m_context->SetBrush( wxBrush(wxColour(pls->cmap0[pls->icol0].r, pls->cmap0[pls->icol0].g,
                                         pls->cmap0[pls->icol0].b)) );
}

void wxPLDevGC::SetColor1( PLStream *pls )
{
  m_context->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(pls->curcolor.r, pls->curcolor.g,
                                                               pls->curcolor.b),
                                                      pls->width>0 ? pls->width : 1, wxSOLID)) );
  m_context->SetBrush( wxBrush(wxColour(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b)) );
}

/*--------------------------------------------------------------------------*\
 *  void wx_set_dc( PLStream* pls, wxDC* dc )
 *
 *  Adds a dc to the stream. The associated device is attached to the canvas
 *  as the property "dev".
\*--------------------------------------------------------------------------*/
void wxPLDevGC::SetExternalBuffer( void* dc )
{
  m_dc=(wxDC*)dc;  /* Add the dc to the device */
  if( m_context )
    delete m_context;
  m_context = wxGraphicsContext::Create( m_dc );
  ready = true;
  ownGUI = false;
}

void wxPLDevGC::PutPixel( short x, short y, PLINT color )
{
  const wxPen oldpen=m_dc->GetPen();
  m_context->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(GetRValue(color), GetGValue(color), GetBValue(color)),
                                                      1, wxSOLID)) );
  //m_context->DrawPoint( x, y );
  m_context->SetPen( oldpen );
}

void wxPLDevGC::PutPixel( short x, short y )
{
  //m_dc->DrawPoint( x, y );
}

PLINT wxPLDevGC::GetPixel( short x, short y )
{
#ifdef __WXGTK__
    // The GetPixel method is incredible slow for wxGTK. Therefore we set the colour
    // always to the background color, since this is the case anyway 99% of the time.
    PLINT bgr, bgg, bgb;  /* red, green, blue */
    plgcolbg( &bgr, &bgg, &bgb );  /* get background color information */
    return RGB( bgr, bgg, bgb );
#else
    wxColour col;
    //m_dc->GetPixel( x, y, &col );
    return RGB( col.Red(), col.Green(), col.Blue());
#endif
}

#endif

#endif				/* PLD_wxwidgets */


