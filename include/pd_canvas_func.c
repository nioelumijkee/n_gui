/* canvas function */

/* #define DEBUG(X) X */
#define DEBUG(X)

//----------------------------------------------------------------------------//
inline void pd_cf_erase(char *idcnv,
			char *id)
{
  sys_vgui("%s delete %s\n",
	   idcnv,
	   id);
  DEBUG(post("%s delete %s\n",
	   idcnv,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_coords_4(char *idcnv,
			   char *id,
			   int x0,
			   int y0,
			   int x1,
			   int y1)
{
  sys_vgui("%s coords %s %d %d %d %d\n",
	   idcnv,
	   id,
	   x0,
	   y0,
	   x1,
	   y1);
  DEBUG(post("%s coords %s %d %d %d %d\n",
	   idcnv,
	   id,
	   x0,
	   y0,
	   x1,
	   y1));
}

//----------------------------------------------------------------------------//
inline void pd_cf_coords_2(char *idcnv,
			   char *id,
			   int x0,
			   int y0)
{
  sys_vgui("%s coords %s %d %d\n",
	   idcnv,
	   id,
	   x0,
	   y0);
  DEBUG(post("%s coords %s %d %d\n",
	   idcnv,
	   id,
	   x0,
	   y0));
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_line(char *idcnv,
			     char *id,
			     int col)
{
  sys_vgui("%s itemconfigure %s -fill #%6.6x\n",
	   idcnv,
	   id,
	   col);
  DEBUG(post("%s itemconfigure %s -fill #%6.6x\n",
	   idcnv,
	   id,
	   col));
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_1(char *idcnv,
			  char *id,
			  int col)
{
  sys_vgui("%s itemconfigure %s -outline #%6.6x\n",
	   idcnv,
	   id,
	   col);
  DEBUG(post("%s itemconfigure %s -outline #%6.6x\n",
	   idcnv,
	   id,
	   col));
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_2(char *idcnv,
			  char *id,
			  int fcol,
			  int bcol)
{
  sys_vgui("%s itemconfigure %s -outline #%6.6x -fill #%6.6x\n",
	   idcnv,
	   id,
	   fcol,
	   bcol);
  DEBUG(post("%s itemconfigure %s -outline #%6.6x -fill #%6.6x\n",
	   idcnv,
	   id,
	   fcol,
	   bcol));
}

//----------------------------------------------------------------------------//
inline void pd_cf_w(char *idcnv,
		    char *id,
		    int w)
{
  sys_vgui("%s itemconfigure %s -width %d\n",
	   idcnv,
	   id,
	   w);
  DEBUG(post("%s itemconfigure %s -width %d\n",
	   idcnv,
	   id,
	   w));
}

//----------------------------------------------------------------------------//
inline void pd_cf_fs(char *idcnv,
		     char *id,
		     int fs)
{
  sys_vgui("%s itemconfigure %s -anchor w -font {{%s} -%d %s}\n",
	   idcnv,
	   id,
	   sys_font,
	   fs,
	   sys_fontweight);
  DEBUG(post("%s itemconfigure %s -anchor w -font {{%s} -%d %s}\n",
	   idcnv,
	   id,
	   sys_font,
	   fs,
	   sys_fontweight));
}

//----------------------------------------------------------------------------//
inline void pd_cf_stex(char *idcnv,
		       char *id,
		       int st,
		       int ex)
{
  sys_vgui("%s itemconfigure %s  -start %d -extent %d\n",
	   idcnv,
	   id,
	   st,
	   ex);
  DEBUG(post("%s itemconfigure %s  -start %d -extent %d\n",
	   idcnv,
	   id,
	   st,
	   ex));
}

//----------------------------------------------------------------------------//
inline void pd_cf_tx(char *idcnv,
		     char *id,
		     const char text[])
{
  sys_vgui("%s itemconfigure %s -text {%s}\n",
	   idcnv,
	   id,
	   text);
  DEBUG(post("%s itemconfigure %s -text {%s}\n",
	   idcnv,
	   id,
	   text));
}

//----------------------------------------------------------------------------//
inline void pd_cf_line(char *idcnv,
		       char *id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui("%s create line %d %d %d %d -fill #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   id);
  DEBUG(post("%s create line %d %d %d %d -fill #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_rect(char *idcnv,
		       char *id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui("%s create rectangle %d %d %d %d -outline #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   id);
  DEBUG(post("%s create rectangle %d %d %d %d -outline #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_rect_filled(char *idcnv,
			      char *id,
			      int fcol,
			      int bcol,
			      int w,
			      int x0,
			      int y0,
			      int x1,
			      int y1)
{
  sys_vgui("%s create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   id);
  DEBUG(post("%s create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_oval(char *idcnv,
		       char *id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui("%s create oval %d %d %d %d -outline #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   id);
  DEBUG(post("%s create oval %d %d %d %d -outline #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_oval_filled(char *idcnv,
			      char *id,
			      int fcol,
			      int bcol,
			      int w,
			      int x0,
			      int y0,
			      int x1,
			      int y1)
{
  sys_vgui("%s create oval %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   id);
  DEBUG(post("%s create oval %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_arc(char *idcnv,
		      char *id,
		      int col,
		      int w,
		      int st,
		      int ex,
		      int x0,
		      int y0,
		      int x1,
		      int y1)
{
  sys_vgui("%s create arc %d %d %d %d -outline #%6.6x -width %d -start %d -extent %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   st,
	   ex,
	   id);
  DEBUG(post("%s create arc %d %d %d %d -outline #%6.6x -width %d -start %d -extent %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   st,
	   ex,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_arc_filled(char *idcnv,
			     char *id,
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
  sys_vgui("%s create arc %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -start %d -extent %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   st,
	   ex,
	   id);
  DEBUG(post("%s create arc %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -start %d -extent %d -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   x1,
	   y1,
	   fcol,
	   bcol,
	   w,
	   st,
	   ex,
	   id));
}

//----------------------------------------------------------------------------//
inline void pd_cf_text(char *idcnv,
		       char *id,
		       int col,
		       int fs,
		       int x0,
		       int y0,
		       const char text[])
{
  sys_vgui("%s create text %d %d -text {%s} -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   text,
	   sys_font,
	   fs,
	   sys_fontweight,
	   col,
	   id);
  DEBUG(post("%s create text %d %d -text {%s} -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags %s\n",
	   idcnv,
	   x0,
	   y0,
	   text,
	   sys_font,
	   fs,
	   sys_fontweight,
	   col,
	   id));
}
