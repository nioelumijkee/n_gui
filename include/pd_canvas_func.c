/* canvas function */
#define DEBUG(X)

//----------------------------------------------------------------------------//
inline void pd_cf_erase(long x,
			t_canvas *canvas,
			int id)
{
  sys_vgui(".x%lx.c delete %lx%d\n",
	   canvas,
	   x,
	   id);
  DEBUG(post(".x%lx.c delete %lx%d\n",
	   canvas,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_coords_4(long x,
			   t_canvas *canvas,
			   int id,
			   int x0,
			   int y0,
			   int x1,
			   int y1)
{
  sys_vgui(".x%lx.c coords %lx%d %d %d %d %d\n",
	   canvas,
	   x,
	   id,
	   x0,
	   y0,
	   x1,
	   y1);
  DEBUG(post(".x%lx.c coords %lx%d %d %d %d %d\n",
	   canvas,
	   x,
	   id,
	   x0,
	   y0,
	   x1,
	   y1));
}

//----------------------------------------------------------------------------//
inline void pd_cf_coords_2(long x,
			   t_canvas *canvas,
			   int id,
			   int x0,
			   int y0)
{
  sys_vgui(".x%lx.c coords %lx%d %d %d\n",
	   canvas,
	   x,
	   id,
	   x0,
	   y0);
  DEBUG(post(".x%lx.c coords %lx%d %d %d\n",
	   canvas,
	   x,
	   id,
	   x0,
	   y0));
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_line(long x,
			     t_canvas *canvas,
			     int id,
			     int col)
{
  sys_vgui(".x%lx.c itemconfigure %lx%d -fill #%6.6x\n",
	   canvas,
	   x,
	   id,
	   col);
  DEBUG(post(".x%lx.c itemconfigure %lx%d -fill #%6.6x\n",
	   canvas,
	   x,
	   id,
	   col));
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_1(long x,
			  t_canvas *canvas,
			  int id,
			  int col)
{
  sys_vgui(".x%lx.c itemconfigure %lx%d -outline #%6.6x\n",
	   canvas,
	   x,
	   id,
	   col);
  DEBUG(post(".x%lx.c itemconfigure %lx%d -outline #%6.6x\n",
	   canvas,
	   x,
	   id,
	   col));
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_2(long x,
			  t_canvas *canvas,
			  int id,
			  int fcol,
			  int bcol)
{
  sys_vgui(".x%lx.c itemconfigure %lx%d -outline #%6.6x -fill #%6.6x\n",
	   canvas,
	   x,
	   id,
	   fcol,
	   bcol);
  DEBUG(post(".x%lx.c itemconfigure %lx%d -outline #%6.6x -fill #%6.6x\n",
	   canvas,
	   x,
	   id,
	   fcol,
	   bcol));
}

//----------------------------------------------------------------------------//
inline void pd_cf_w(long x,
		    t_canvas *canvas,
		    int id,
		    int w)
{
  sys_vgui(".x%lx.c itemconfigure %lx%d -width %d\n",
	   canvas,
	   x,
	   id,
	   w);
  DEBUG(post(".x%lx.c itemconfigure %lx%d -width %d\n",
	   canvas,
	   x,
	   id,
	   w));
}

//----------------------------------------------------------------------------//
inline void pd_cf_fs(long x,
		     t_canvas *canvas,
		     int id,
		     int fs)
{
  sys_vgui(".x%lx.c itemconfigure %lx%d -anchor w -font {{%s} -%d %s}\n",
	   canvas,
	   x,
	   id,
	   sys_font,
	   fs,
	   sys_fontweight);
  DEBUG(post(".x%lx.c itemconfigure %lx%d -anchor w -font {{%s} -%d %s}\n",
	   canvas,
	   x,
	   id,
	   sys_font,
	   fs,
	   sys_fontweight));
}

//----------------------------------------------------------------------------//
inline void pd_cf_stex(long x,
		       t_canvas *canvas,
		       int id,
		       int st,
		       int ex)
{
  sys_vgui(".x%lx.c itemconfigure %lx%d  -start %d -extent %d\n",
	   canvas,
	   x,
	   id,
	   st,
	   ex);
  DEBUG(post(".x%lx.c itemconfigure %lx%d  -start %d -extent %d\n",
	   canvas,
	   x,
	   id,
	   st,
	   ex));
}

//----------------------------------------------------------------------------//
inline void pd_cf_tx(long x,
		     t_canvas *canvas,
		     int id,
		     const char text[])
{
  sys_vgui(".x%lx.c itemconfigure %lx%d -text {%s}\n",
	   canvas,
	   x,
	   id,
	   text);
  DEBUG(post(".x%lx.c itemconfigure %lx%d -text {%s}\n",
	   canvas,
	   x,
	   id,
	   text));
}

//----------------------------------------------------------------------------//
inline void pd_cf_line(long x,
		       t_canvas *canvas,
		       int id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui(".x%lx.c create line %d %d %d %d -fill #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x,
	   id);
  DEBUG(post(".x%lx.c create line %d %d %d %d -fill #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_rect(long x,
		       t_canvas *canvas,
		       int id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x,
	   id);
  DEBUG(post(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_rect_filled(long x,
			      t_canvas *canvas,
			      int id,
			      int fcol,
			      int bcol,
			      int w,
			      int x0,
			      int y0,
			      int x1,
			      int y1)
{
  sys_vgui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   x,
	   id);
  DEBUG(post(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_oval(long x,
		       t_canvas *canvas,
		       int id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui(".x%lx.c create oval %d %d %d %d -outline #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x,
	   id);
  DEBUG(post(".x%lx.c create oval %d %d %d %d -outline #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_oval_filled(long x,
			      t_canvas *canvas,
			      int id,
			      int fcol,
			      int bcol,
			      int w,
			      int x0,
			      int y0,
			      int x1,
			      int y1)
{
  sys_vgui(".x%lx.c create oval %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   x,
	   id);
  DEBUG(post(".x%lx.c create oval %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_arc(long x,
		      t_canvas *canvas,
		      int id,
		      int col,
		      int w,
		      int st,
		      int ex,
		      int x0,
		      int y0,
		      int x1,
		      int y1)
{
  sys_vgui(".x%lx.c create arc %d %d %d %d -outline #%6.6x -width %d -start %d -extent %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   st,
	   ex,
	   x,
	   id);
  DEBUG(post(".x%lx.c create arc %d %d %d %d -outline #%6.6x -width %d -start %d -extent %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   st,
	   ex,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_arc_filled(long x,
			     t_canvas *canvas,
			     int id,
			     int fcol,
			     int bcol,
			     int w,
			     int st,
			     int ex,
			     int x0,
			     int y0,
			     int x1,
			     int y1)
{
  sys_vgui(".x%lx.c create arc %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -start %d -extent %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   st,
	   ex,
	   x,
	   id);
  DEBUG(post(".x%lx.c create arc %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -start %d -extent %d -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   st,
	   ex,
	   x,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_text(long x,
		       t_canvas *canvas,
		       int id,
		       int col,
		       int fs,
		       int x0,
		       int y0,
		       const char text[])
{
  sys_vgui(".x%lx.c create text %d %d -text {%s} -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   text,
	   sys_font,
	   fs,
	   sys_fontweight,
	   col,
	   x,
	   id);
  DEBUG(post(".x%lx.c create text %d %d -text {%s} -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags %lx%d\n",
	   canvas,
	   x0,
	   y0,
	   text,
	   sys_font,
	   fs,
	   sys_fontweight,
	   col,
	   x,
	   id));
}
