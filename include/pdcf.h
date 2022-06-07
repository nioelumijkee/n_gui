#include "m_pd.h"

//----------------------------------------------------------------------------//
inline void pdcf_erase(long x, long c, int id)
{
  sys_vgui(".x%lx.c delete t%lx%d\n",
	   c,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_coords_4(long x, long c, int id,
			  int x0,
			  int y0,
			  int x1,
			  int y1)
{
  sys_vgui(".x%lx.c coords t%lx%d %d %d %d %d\n",
	   c,
	   x, id,
	   x0,
	   y0,
	   x1,
	   y1);
}

//----------------------------------------------------------------------------//
inline void pdcf_coords_2(long x, long c, int id,
			  int x0,
			  int y0)
{
  sys_vgui(".x%lx.c coords t%lx%d %d %d\n",
	   c,
	   x, id,
	   x0,
	   y0);
}

//----------------------------------------------------------------------------//
inline void pdcf_color_line(long x, long c, int id,
			    int col)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -fill #%6.6x\n",
	   c,
	   x, id,
	   col);
}

//----------------------------------------------------------------------------//
inline void pdcf_color_1(long x, long c, int id,
			 int col)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -outline #%6.6x\n",
	   c,
	   x, id,
	   col);
}

//----------------------------------------------------------------------------//
inline void pdcf_color_2(long x, long c, int id,
			 int fcol,
			 int bcol)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -outline #%6.6x -fill #%6.6x\n",
	   c,
	   x, id,
	   fcol,
	   bcol);
}

//----------------------------------------------------------------------------//
inline void pdcf_w(long x, long c, int id,
		   int w)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -width %d\n",
	   c,
	   x, id,
	   w);
}

//----------------------------------------------------------------------------//
inline void pdcf_fs(long x, long c, int id,
		    int fs)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -anchor w -font {{%s} -%d %s}\n",
	   c,
	   x, id,
	   sys_font,
	   fs,
	   sys_fontweight);
}

//----------------------------------------------------------------------------//
inline void pdcf_stex(long x, long c, int id,
		      int st,
		      int ex)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d  -start %d -extent %d\n",
	   c,
	   x, id,
	   st,
	   ex);
}

//----------------------------------------------------------------------------//
inline void pdcf_tx(long x, long c, int id,
		    const char text[])
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -text {%s}\n",
	   c,
	   x, id,
	   text);
}

//----------------------------------------------------------------------------//
inline void pdcf_line(long x, long c, int id,
		      int col,
		      int w,
		      int x0,
		      int y0,
		      int x1,
		      int y1)
{
  sys_vgui(".x%lx.c create line %d %d %d %d -fill #%6.6x -width %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_rect(long x, long c, int id,
		      int col,
		      int w,
		      int x0,
		      int y0,
		      int x1,
		      int y1)
{
  sys_vgui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -width %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_rect_filled(long x, long c, int id,
			     int fcol,
			     int bcol,
			     int w,
			     int x0,
			     int y0,
			     int x1,
			     int y1)
{
  sys_vgui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_oval(long x, long c, int id,
		      int col,
		      int w,
		      int x0,
		      int y0,
		      int x1,
		      int y1)
{
  sys_vgui(".x%lx.c create oval %d %d %d %d -outline #%6.6x -width %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_oval_filled(long x, long c, int id,
			     int fcol,
			     int bcol,
			     int w,
			     int x0,
			     int y0,
			     int x1,
			     int y1)
{
  sys_vgui(".x%lx.c create oval %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_arc(long x, long c, int id,
		     int col,
		     int w,
		     int st,
		     int ex,
		     int x0,
		     int y0,
		     int x1,
		     int y1)
{
  sys_vgui(".x%lx.c create arc %d %d %d %d -outline #%6.6x -width %d -start %d -extent %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   st,
	   ex,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_arc_filled(long x, long c, int id,
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
  sys_vgui(".x%lx.c create arc %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -start %d -extent %d -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   st,
	   ex,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pdcf_text(long x, long c, int id,
		      int col,
		      int fs,
		      int x0,
		      int y0,
		      const char text[])
{
  sys_vgui(".x%lx.c create text %d %d -text {%s} -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags t%lx%d\n",
	   c,
	   x0,
	   y0,
	   text,
	   sys_font,
	   fs,
	   sys_fontweight,
	   col,
	   x, id);
}
