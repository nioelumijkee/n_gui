#include <stdlib.h>
#include <string.h>
#include "m_pd.h"
#include "m_imp.h"
#include "g_canvas.h"
#include "g_all_guis.h"
#include "include/math.h"
#include "include/clip.h"
#include "include/pd_canvas_func.c"
#include "include/pd_color.c"
#include "include/pd_func.c"

#define M_BASE -2
#define M_LABEL -1
#define ERASE 0
#define LINE 1
#define RECT 2
#define RECT_F 3
#define OVAL 4
#define OVAL_F 5
#define ARC 6
#define ARC_F 7
#define TEXT 8
#define MOVE 9
#define COLOR 10
#define SIZE 11
#define STEX 12
#define TX 13


t_widgetbehavior n_canvas_widgetbehavior;
static t_class *n_canvas_class;

typedef struct _n_canvas_element
{
  int type;
  int fcol;
  int bcol;
  int s;
  int st;
  int ex;
  int x[2];
  int y[2];
  t_symbol *text;
} t_n_canvas_element;

typedef struct _n_canvas
{
  t_object x_obj;
  t_canvas *x_canvas;
  t_glist *x_glist;
  int xpos;
  int ypos;
  int x_w; /* par */
  int x_h;
  t_symbol *x_snd;
  t_symbol *x_rcv;
  t_symbol *x_lab;
  t_symbol *x_snd_real;
  t_symbol *x_rcv_real;
  int x_ldx;
  int x_ldy;
  int x_fontsize;
  int x_bcol;
  int x_fcol;
  int x_lcol;
  int x_snd_able;
  int x_rcv_able;
  int m_xp; /* mouse keys */
  int m_yp;
  int m_pos;
  int m_shift;
  int m_alt;
  int maxel; /* max elements */
  struct _n_canvas_element *e;
  t_symbol *s_empty; /* */
  int dollarzero;
} t_n_canvas;

//----------------------------------------------------------------------------//
static void n_canvas_list(t_n_canvas *x, t_symbol *s, int ac, t_atom *av)
{
  x->x_canvas = glist_getcanvas(x->x_glist);
  
  int id, op;
  
  op = atom_getfloatarg(0, ac, av);
  
  switch (op)
    {
    case ERASE:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase((long)x, x->x_canvas, id);
	      x->e[id].type = ERASE;
	    }
	  else
	    x->e[id].type = ERASE;
	}
      break;
      
    case LINE:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].s = atom_getfloatarg(3, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(4, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(5, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(6, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(7, ac, av);
	  pd_color(&x->e[id].fcol);
	  AF_CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase((long)x, x->x_canvas, id);
	      x->e[id].type = LINE;
	      
	      // create
	      pd_cf_line((long)x, x->x_canvas, id,
			 x->e[id].fcol,
			 x->e[id].s,
			 x->xpos + x->e[id].x[0],
			 x->ypos + x->e[id].y[0],
			 x->xpos + x->e[id].x[1],
			 x->ypos + x->e[id].y[1]);
	    }
	  else
	    x->e[id].type = LINE;
	}
      break;
      
    case RECT:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].s = atom_getfloatarg(3, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(4, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(5, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(6, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(7, ac, av);
	  pd_color(&x->e[id].fcol);
	  AF_CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase((long)x, x->x_canvas, id);
	      x->e[id].type = RECT;
	      
	      // create
	      pd_cf_rect((long)x, x->x_canvas, id,
			 x->e[id].fcol,
			 x->e[id].s,
			 x->xpos + x->e[id].x[0],
			 x->ypos + x->e[id].y[0],
			 x->xpos + x->e[id].x[1],
			 x->ypos + x->e[id].y[1]);
	    }
	  else
	    x->e[id].type = RECT;
	}
      break;
      
    case RECT_F:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].bcol = atom_getfloatarg(3, ac, av);
	  x->e[id].s = atom_getfloatarg(4, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(5, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(6, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(7, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(8, ac, av);
	  pd_color(&x->e[id].fcol);
	  pd_color(&x->e[id].bcol);
	  AF_CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase((long)x, x->x_canvas, id);
	      x->e[id].type = RECT_F;
	      
	      // create
	      pd_cf_rect_filled((long)x, x->x_canvas, id,
				x->e[id].fcol,
				x->e[id].bcol,
				x->e[id].s,
				x->xpos + x->e[id].x[0],
				x->ypos + x->e[id].y[0],
				x->xpos + x->e[id].x[1],
				x->ypos + x->e[id].y[1]);
	    }
	  else
	    x->e[id].type = RECT_F;
	}
      break;
      
    case OVAL:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].s = atom_getfloatarg(3, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(4, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(5, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(6, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(7, ac, av);
	  pd_color(&x->e[id].fcol);
	  AF_CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase((long)x, x->x_canvas, id);
	      x->e[id].type = OVAL;
	      
	      // create
	      pd_cf_oval((long)x, x->x_canvas, id,
			 x->e[id].fcol,
			 x->e[id].s,
			 x->xpos + x->e[id].x[0],
			 x->ypos + x->e[id].y[0],
			 x->xpos + x->e[id].x[1],
			 x->ypos + x->e[id].y[1]);
	    }
	  else
	    x->e[id].type = OVAL;
	}
      break;
      
    case OVAL_F:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].bcol = atom_getfloatarg(3, ac, av);
	  x->e[id].s = atom_getfloatarg(4, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(5, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(6, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(7, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(8, ac, av);
	  pd_color(&x->e[id].fcol);
	  pd_color(&x->e[id].bcol);
	  AF_CLIP_MIN(1, x->e[id].s);
	    
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase((long)x, x->x_canvas, id);
	      x->e[id].type = OVAL_F;
		
		// create
		pd_cf_oval_filled((long)x, x->x_canvas, id,
				  x->e[id].fcol,
				  x->e[id].bcol,
				  x->e[id].s,
				  x->xpos + x->e[id].x[0],
				  x->ypos + x->e[id].y[0],
				  x->xpos + x->e[id].x[1],
				  x->ypos + x->e[id].y[1]);
	      }
	    else
	      x->e[id].type = OVAL_F;
	}
      break;
      
    case ARC:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].s = atom_getfloatarg(3, ac, av);
	  x->e[id].st = atom_getfloatarg(4, ac, av);
	  x->e[id].ex = atom_getfloatarg(5, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(6, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(7, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(8, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(9, ac, av);
	  pd_color(&x->e[id].fcol);
	  AF_CLIP_MIN(1, x->e[id].s)
	    
	    if (glist_isvisible(x->x_glist))
	      {
		// erase
		if (x->e[id].type != ERASE)
		  pd_cf_erase((long)x, x->x_canvas, id);
		x->e[id].type = ARC;
		
		// create
		pd_cf_arc((long)x, x->x_canvas, id,
			  x->e[id].fcol,
			  x->e[id].s,
			  x->e[id].st,
			  x->e[id].ex,
			  x->xpos + x->e[id].x[0],
			  x->ypos + x->e[id].y[0],
			  x->xpos + x->e[id].x[1],
			  x->ypos + x->e[id].y[1]);
	      }
	    else
	      x->e[id].type = ARC;
	}
      break;
      
    case ARC_F:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].bcol = atom_getfloatarg(3, ac, av);
	  x->e[id].s = atom_getfloatarg(4, ac, av);
	  x->e[id].st = atom_getfloatarg(5, ac, av);
	  x->e[id].ex = atom_getfloatarg(6, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(7, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(8, ac, av);
	  x->e[id].x[1] = atom_getfloatarg(9, ac, av);
	  x->e[id].y[1] = atom_getfloatarg(10, ac, av);
	  pd_color(&x->e[id].fcol);
	  pd_color(&x->e[id].bcol);
	  AF_CLIP_MIN(1, x->e[id].s)
	    
	    if (glist_isvisible(x->x_glist))
	      {
		// erase
		if (x->e[id].type != ERASE)
		  pd_cf_erase((long)x, x->x_canvas, id);
		x->e[id].type = ARC_F;
		
		// create
		pd_cf_arc_filled((long)x, x->x_canvas, id,
				 x->e[id].fcol,
				 x->e[id].bcol,
				 x->e[id].s,
				 x->e[id].st,
				 x->e[id].ex,
				 x->xpos + x->e[id].x[0],
				 x->ypos + x->e[id].y[0],
				 x->xpos + x->e[id].x[1],
				 x->ypos + x->e[id].y[1]);
	      }
	    else
	      x->e[id].type = ARC_F;
	}
      break;
      
    case TEXT:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // write
	  x->e[id].fcol = atom_getfloatarg(2, ac, av);
	  x->e[id].s = atom_getfloatarg(3, ac, av);
	  x->e[id].x[0] = atom_getfloatarg(4, ac, av);
	  x->e[id].y[0] = atom_getfloatarg(5, ac, av);
	  x->e[id].text = (t_symbol *)atom_getsymbolarg(6, ac, av);
	  pd_color(&x->e[id].fcol);
	  AF_CLIP_MIN(4, x->e[id].s)
	    
	    if (glist_isvisible(x->x_glist))
	      {
		// erase
		if (x->e[id].type != ERASE)
		  pd_cf_erase((long)x, x->x_canvas, id);
		x->e[id].type = TEXT;
		
		// create
		pd_cf_text((long)x, x->x_canvas, id,
			   x->e[id].fcol,
			   x->e[id].s,
			   x->xpos + x->e[id].x[0],
			   x->ypos + x->e[id].y[0],
			   x->e[id].text->s_name);
	      }
	    else
	      x->e[id].type = TEXT;
	}
      break;
      
    case MOVE:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // coords 2
	  if (x->e[id].type == TEXT)
	    {
	      // write
	      x->e[id].x[0] = atom_getfloatarg(2, ac, av);
	      x->e[id].y[0] = atom_getfloatarg(3, ac, av);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // move
		  pd_cf_coords_2((long)x, x->x_canvas, id,
				 x->xpos + x->e[id].x[0],
				 x->ypos + x->e[id].y[0]);
		}
	    }
	  
	  // coords 4
	  else if (x->e[id].type > ERASE)
	    {
	      // write
	      x->e[id].x[0] = atom_getfloatarg(2, ac, av);
	      x->e[id].y[0] = atom_getfloatarg(3, ac, av);
	      x->e[id].x[1] = atom_getfloatarg(4, ac, av);
	      x->e[id].y[1] = atom_getfloatarg(5, ac, av);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // move
		  pd_cf_coords_4((long)x, x->x_canvas, id,
				 x->xpos + x->e[id].x[0],
				 x->ypos + x->e[id].y[0],
				 x->xpos + x->e[id].x[1],
				 x->ypos + x->e[id].y[1]);
		}
	    }
	}
      break;
      
    case COLOR:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // color line
	  if (x->e[id].type == LINE ||
	      x->e[id].type == TEXT)
	    {
	      // write
	      x->e[id].fcol = atom_getfloatarg(2, ac, av);
	      pd_color(&x->e[id].fcol);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // color
		  pd_cf_color_line((long)x, x->x_canvas, id,
				   x->e[id].fcol);
		}
	    }
	  
	  // color 1
	  else if (x->e[id].type == RECT ||
		   x->e[id].type == OVAL ||
		   x->e[id].type == ARC)
	    {
	      // write
	      x->e[id].fcol = atom_getfloatarg(2, ac, av);
	      pd_color(&x->e[id].fcol);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // color
		  pd_cf_color_1((long)x, x->x_canvas, id,
				x->e[id].fcol);
		}
	    }
	  
	  // color 2
	  else if (x->e[id].type == RECT_F ||
		   x->e[id].type == OVAL_F ||
		   x->e[id].type == ARC_F)
	    {
	      // write
	      x->e[id].fcol = atom_getfloatarg(2, ac, av);
	      x->e[id].bcol = atom_getfloatarg(3, ac, av);
	      pd_color(&x->e[id].fcol);
	      pd_color(&x->e[id].bcol);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // color
		  pd_cf_color_2((long)x, x->x_canvas, id,
				x->e[id].fcol,
				x->e[id].bcol);
		}
	    }
	}
      break;
      
    case SIZE:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // width
	  if (x->e[id].type > ERASE && x->e[id].type < TEXT)
	    {
	      // write
	      x->e[id].s = atom_getfloatarg(2, ac, av);
	      AF_CLIP_MIN(1, x->e[id].s)
		
		if (glist_isvisible(x->x_glist))
		  {
		    // w
		    pd_cf_w((long)x, x->x_canvas, id,
			    x->e[id].s);
		  }
	    }
	  
	  // fs
	  else if (x->e[id].type == TEXT)
	    {
	      // write
	      x->e[id].s = atom_getfloatarg(2, ac, av);
	      AF_CLIP_MIN(4, x->e[id].s)
		
		if (glist_isvisible(x->x_glist))
		  {
		    // fs
		    pd_cf_fs((long)x, x->x_canvas, id,
			     x->e[id].s);
		  }
	    }
	}
      break;
      
    case STEX:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // stex
	  if (x->e[id].type == ARC || x->e[id].type == ARC_F)
	    {
	      // write
	      x->e[id].st = atom_getfloatarg(2, ac, av);
	      x->e[id].ex = atom_getfloatarg(3, ac, av);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // stex
		  pd_cf_stex((long)x, x->x_canvas, id,
			     x->e[id].st,
			     x->e[id].ex);
		}
	    }
	}
      break;
      
    case TX:
      id = atom_getfloatarg(1, ac, av);
      if (id > -1 && id < x->maxel)
	{
	  // text
	  if (x->e[id].type == TEXT)
	    {
	      // write
	      x->e[id].text = (t_symbol *)atom_getsymbolarg(2, ac, av);
	      
	      if (glist_isvisible(x->x_glist))
		{
		  // text
		  pd_cf_tx((long)x, x->x_canvas, id,
			   x->e[id].text->s_name);
		}
	    }
	}
      break;
    }
  if (s) {} // disabled
}

//----------------------------------------------------------------------------//
static void n_canvas_draw_new(t_n_canvas *x, t_glist *glist)
{
//  x->x_glist = glist; // ???
  x->xpos = text_xpix(&x->x_obj, glist);
  x->ypos = text_ypix(&x->x_obj, glist);
  x->x_canvas = glist_getcanvas(glist);
  
  pd_cf_rect_filled((long)x, x->x_canvas, M_BASE,
		    x->x_fcol,
		    x->x_bcol,
		    1,
		    x->xpos,
		    x->ypos,
		    x->xpos + x->x_w,
		    x->ypos + x->x_h);
  
  pd_cf_text((long)x, x->x_canvas, M_LABEL,
	     x->x_lcol,
	     x->x_fontsize,
	     x->xpos + x->x_ldx,
	     x->ypos + x->x_ldy,
	     (strcmp(x->x_lab->s_name, "empty") ? x->x_lab->s_name : " "));
  
  for (int id = 0; id < x->maxel; id++)
    {
      switch (x->e[id].type)
	{
	case LINE:
	  pd_cf_line((long)x, x->x_canvas, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->xpos + x->e[id].x[1],
		     x->ypos + x->e[id].y[1]);
	  break;
	  
	case RECT:
	  pd_cf_rect((long)x, x->x_canvas, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->xpos + x->e[id].x[1],
		     x->ypos + x->e[id].y[1]);
	  break;
	  
	case RECT_F:
	  pd_cf_rect_filled((long)x, x->x_canvas, id,
			    x->e[id].fcol,
			    x->e[id].bcol,
			    x->e[id].s,
			    x->xpos + x->e[id].x[0],
			    x->ypos + x->e[id].y[0],
			    x->xpos + x->e[id].x[1],
			    x->ypos + x->e[id].y[1]);
	  break;
	  
	case OVAL:
	  pd_cf_oval((long)x, x->x_canvas, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->xpos + x->e[id].x[1],
		     x->ypos + x->e[id].y[1]);
	  break;
	  
	case OVAL_F:
	  pd_cf_oval_filled((long)x, x->x_canvas, id,
			    x->e[id].fcol,
			    x->e[id].bcol,
			    x->e[id].s,
			    x->xpos + x->e[id].x[0],
			    x->ypos + x->e[id].y[0],
			    x->xpos + x->e[id].x[1],
			    x->ypos + x->e[id].y[1]);
	  break;
	  
	case ARC:
	  pd_cf_arc((long)x, x->x_canvas, id,
		    x->e[id].fcol,
		    x->e[id].s,
		    x->e[id].st,
		    x->e[id].ex,
		    x->xpos + x->e[id].x[0],
		    x->ypos + x->e[id].y[0],
		    x->xpos + x->e[id].x[1],
		    x->ypos + x->e[id].y[1]);
	  break;
	  
	case ARC_F:
	  pd_cf_arc_filled((long)x, x->x_canvas, id,
			   x->e[id].fcol,
			   x->e[id].bcol,
			   x->e[id].s,
			   x->e[id].st,
			   x->e[id].ex,
			   x->xpos + x->e[id].x[0],
			   x->ypos + x->e[id].y[0],
			   x->xpos + x->e[id].x[1],
			   x->ypos + x->e[id].y[1]);
	  break;
	  
	case TEXT:
	  pd_cf_text((long)x, x->x_canvas, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->e[id].text->s_name);
	  break;
	}
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_draw_move(t_n_canvas *x, t_glist *glist)
{
//  x->x_glist = glist; // ???
  x->xpos = text_xpix(&x->x_obj, glist);
  x->ypos = text_ypix(&x->x_obj, glist);
  x->x_canvas = glist_getcanvas(glist);
  
  pd_cf_coords_4((long)x, x->x_canvas, M_BASE,
		 x->xpos,
		 x->ypos,
		 x->xpos + x->x_w,
		 x->ypos + x->x_h);
  
  pd_cf_coords_2((long)x, x->x_canvas, M_LABEL,
		 x->xpos + x->x_ldx,
		 x->ypos + x->x_ldy);
  
  for (int id = 0; id < x->maxel; id++)
    {
      if (x->e[id].type > ERASE && x->e[id].type < TEXT)
	pd_cf_coords_4((long)x, x->x_canvas, id,
		       x->xpos + x->e[id].x[0],
		       x->ypos + x->e[id].y[0],
		       x->xpos + x->e[id].x[1],
		       x->ypos + x->e[id].y[1]);
      
      else if (x->e[id].type == TEXT)
	pd_cf_coords_2((long)x, x->x_canvas, id,
		       x->xpos + x->e[id].x[0],
		       x->ypos + x->e[id].y[0]);
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_draw_erase(t_n_canvas *x, t_glist *glist)
{
//  x->x_glist = glist; // ???
  x->x_canvas = glist_getcanvas(glist);
  
  if(glist_isvisible(x->x_glist))
    { 
      pd_cf_erase((long)x, x->x_canvas, M_BASE);
      pd_cf_erase((long)x, x->x_canvas, M_LABEL);
      
      for (int id = 0; id < x->maxel; id++)
	if (x->e[id].type > 0 && x->e[id].type < 9)
	  pd_cf_erase((long)x, x->x_canvas, id);
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_draw_config(t_n_canvas *x)
{
  if(glist_isvisible(x->x_glist))
    { 
      pd_cf_coords_4((long)x, x->x_canvas, M_BASE,
		     x->xpos,
		     x->ypos,
		     x->xpos + x->x_w,
		     x->ypos + x->x_h);
      
      pd_cf_color_2((long)x, x->x_canvas, M_BASE,
		    x->x_fcol,
		    x->x_bcol);
      
      pd_cf_coords_2((long)x, x->x_canvas, M_LABEL,
		     x->xpos + x->x_ldx,
		     x->ypos + x->x_ldy);
      
      pd_cf_color_line((long)x, x->x_canvas, M_LABEL,
		       x->x_lcol);
      
      pd_cf_tx((long)x, x->x_canvas, M_LABEL,
	       (strcmp(x->x_lab->s_name, "empty") ? x->x_lab->s_name : " "));
      
      pd_cf_fs((long)x, x->x_canvas, M_LABEL,
	       x->x_fontsize);
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_free(t_n_canvas *x)
{
  free(x->e);
}

//----------------------------------------------------------------------------//
static void n_canvas_mem(t_n_canvas *x)
{
  x->e = calloc(x->maxel, sizeof(struct _n_canvas_element));
  if (!x->e)
    error("n_canvas: error allocating memory");
}

//----------------------------------------------------------------------------//
static void n_canvas_mouseout(t_n_canvas *x)
{
  t_atom a[5];
  SETFLOAT(a, (t_float)x->m_pos);
  SETFLOAT(a + 1, (t_float)x->m_xp);
  SETFLOAT(a + 2, (t_float)x->m_yp);
  SETFLOAT(a + 3, (t_float)x->m_shift);
  SETFLOAT(a + 4, (t_float)x->m_alt);
  outlet_list(x->x_obj.ob_outlet, &s_list, 5, a);
  if (x->x_snd_able && x->x_snd_real->s_thing)
    {
      pd_list(x->x_snd_real->s_thing, &s_list, 5, a);
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_motion(t_n_canvas *x, t_floatarg dx, t_floatarg dy)
{
  x->m_xp += dx;
  x->m_yp += dy;
  x->m_pos = 2;
  n_canvas_mouseout(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_click(t_n_canvas *x, int xpix, int ypix)
{
  glist_grab(x->x_glist,
	     &x->x_obj.te_g,
	     (t_glistmotionfn)n_canvas_motion,
	     (t_glistkeyfn)NULL,
	     (t_floatarg)xpix,
	     (t_floatarg)ypix);
  x->m_xp = xpix - x->xpos;
  x->m_yp = ypix - x->ypos;
  x->m_pos = 1;
  n_canvas_mouseout(x);
}

//----------------------------------------------------------------------------//
static int n_canvas_newclick(t_gobj *z, struct _glist *glist, int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
  t_n_canvas *x = (t_n_canvas *)z;
  x->xpos = text_xpix(&x->x_obj, glist);
  x->ypos = text_ypix(&x->x_obj, glist);
  x->m_xp = xpix - x->xpos;
  x->m_yp = ypix - x->ypos;
  x->m_pos = 0;
  x->m_shift = dbl;
  x->m_shift = shift;
  x->m_alt = alt;
  n_canvas_mouseout(x);
  if (doit)
    n_canvas_click(x, xpix, ypix);
  return (1);
}

//----------------------------------------------------------------------------//
static void n_canvas_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1, int *xp2, int *yp2)
{
  t_n_canvas *x = (t_n_canvas *)z;
  *xp1 = text_xpix(&x->x_obj, glist);
  *yp1 = text_ypix(&x->x_obj, glist);
  *xp2 = *xp1 + x->x_w;
  *yp2 = *yp1 + x->x_h;
}

//----------------------------------------------------------------------------//
static void n_canvas_size(t_n_canvas *x, t_floatarg w, t_floatarg h)
{
  x->x_w = w;
  x->x_h = h;
  AF_CLIP_MIN(1, x->x_w);
  AF_CLIP_MIN(1, x->x_h);
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_send(t_n_canvas *x, t_symbol *s)
{
  if (!(strcmp(s->s_name, "empty")) || s->s_name[0] == '\0')
    {
      x->x_snd = x->s_empty;
      x->x_snd_real = x->s_empty;
      x->x_snd_able = 0;
    }
  else
    {
      x->x_snd = s;
      x->x_snd_real = pd_dollarzero2sym(s, x->dollarzero);
      x->x_snd_able = 1;
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_receive(t_n_canvas *x, t_symbol *s)
{
  // unbind
  if (x->x_rcv_able)
    pd_unbind(&x->x_obj.ob_pd, x->x_rcv_real);
  if (!(strcmp(s->s_name, "empty")) || s->s_name[0] == '\0')
    {
      x->x_rcv = x->s_empty;
      x->x_rcv_real = x->s_empty;
      x->x_rcv_able = 0;
    }
  else
    {
      x->x_rcv = s;
      x->x_rcv_real = pd_dollarzero2sym(s, x->dollarzero);
      x->x_rcv_able = 1;
    }
  // bind
  if (x->x_rcv_able)
    pd_bind(&x->x_obj.ob_pd, x->x_rcv_real);
}

//----------------------------------------------------------------------------//
static void n_canvas_label(t_n_canvas *x, t_symbol *s)
{
  if (!(strcmp(s->s_name, "empty")) || s->s_name[0] == '\0')
    {
      x->x_lab = x->s_empty;
    }
  else
    {
      x->x_lab = s;
    }
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_label_pos(t_n_canvas *x, t_floatarg ldx, t_floatarg ldy)
{
  x->x_ldx = ldx;
  x->x_ldy = ldy;
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_label_font(t_n_canvas *x, t_floatarg fs)
{
  AF_CLIP_MINMAX(4, 256, fs);
  x->x_fontsize = fs;
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_color(t_n_canvas *x, t_floatarg bcol, t_floatarg fcol, t_floatarg lcol)
{
  x->x_bcol = bcol;
  x->x_fcol = fcol;
  x->x_lcol = lcol;
  pd_color(&x->x_bcol);
  pd_color(&x->x_fcol);
  pd_color(&x->x_lcol);
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_maxel(t_n_canvas *x, t_floatarg f)
{
  // erase if VIEW !?
  if (glist_isvisible(x->x_glist))
    {
      for (int id = 0; id < x->maxel; id++)
	{
	  if (x->e[id].type > ERASE && x->e[id].type <= TEXT)
	    {
	      pd_cf_erase((long)x, x->x_canvas, id);
	    }
	}
    }
  
  // memory
  x->maxel = f;
  AF_CLIP_MIN(1, x->maxel);
  n_canvas_free(x);
  n_canvas_mem(x);
}

//----------------------------------------------------------------------------//
static void n_canvas_save(t_gobj *z, t_binbuf *b)
{
  t_n_canvas *x = (t_n_canvas *)z;
  t_symbol *col[3];
  t_symbol *srl[3];
  char buf[128];
  
  // color
  sprintf(buf, "%d", x->x_bcol);
  col[0] = gensym(buf);
  sprintf(buf, "%d", x->x_fcol);
  col[1] = gensym(buf);
  sprintf(buf, "%d", x->x_lcol);
  col[2] = gensym(buf);
  
  // snd rcv lab
  sprintf(buf, "%s", x->x_snd->s_name);
  pd_dollar_in_string(buf);
  srl[0] = gensym(buf);
  sprintf(buf, "%s", x->x_rcv->s_name);
  pd_dollar_in_string(buf);
  srl[1] = gensym(buf);
  sprintf(buf, "%s", x->x_lab->s_name);
  pd_dollar_in_string(buf);
  srl[2] = gensym(buf);
  
  binbuf_addv(b,
	      "ssiisiisssiiisssi",
	      gensym("#X"),
	      gensym("obj"),
	      (int)x->x_obj.te_xpix,
	      (int)x->x_obj.te_ypix,
	      gensym("n_canvas"),
	      (int)x->x_w,
	      (int)x->x_h,
	      srl[0],
	      srl[1],
	      srl[2],
	      x->x_ldx,
	      x->x_ldy,
	      x->x_fontsize,
	      col[0],
	      col[1],
	      col[2],
	      (int)x->maxel);
  binbuf_addv(b, ";");
}

//----------------------------------------------------------------------------//
static void n_canvas_properties(t_gobj *z, t_glist *owner)
{
  char buf[512];
  t_n_canvas *x = (t_n_canvas *)z;
  
  // snd rcv lab
  char srl[3][128];
  sprintf(srl[0], "%s", x->x_snd->s_name);
  pd_dollar_in_string(srl[0]);
  sprintf(srl[1], "%s", x->x_rcv->s_name);
  pd_dollar_in_string(srl[1]);
  sprintf(srl[2], "%s", x->x_lab->s_name);
  pd_dollar_in_string(srl[2]);
  
  sprintf(buf, "pdtk_n_canvas_dialog %%s %d %d %s %s %s %d %d %d #%06x #%06x #%06x %d\n",
	  x->x_w,
	  x->x_h,
	  srl[0],
	  srl[1],
	  srl[2],
	  x->x_ldx,
	  x->x_ldy,
	  x->x_fontsize,
	  0xffffff & x->x_bcol,
	  0xffffff & x->x_fcol,
	  0xffffff & x->x_lcol,
	  x->maxel);
  gfxstub_new(&x->x_obj.ob_pd, x, buf);
  if (owner) {} // disabled
}

//----------------------------------------------------------------------------//
static void n_canvas_dialog(t_n_canvas *x, t_symbol *s, int ac, t_atom *av)
{
  if (!x)
    {
      error("n_canvas: error:  unexisting object");
      return;
    }
  if (ac != 12)
    {
      error("n_canvas: error: number arguments");
      return;
    }
  int w = (int)atom_getfloatarg(0, ac, av);
  int h = (int)atom_getfloatarg(1, ac, av);
  t_symbol *snd = pd_getarg_symbol(2, ac, av);
  t_symbol *rcv = pd_getarg_symbol(3, ac, av);
  t_symbol *lab = pd_getarg_symbol(4, ac, av);
  int ldx = (int)atom_getfloatarg(5, ac, av);
  int ldy = (int)atom_getfloatarg(6, ac, av);
  int fontsize = (int)atom_getfloatarg(7, ac, av);
  int bcol = pd_getarg_int(8, ac, av);
  int fcol = pd_getarg_int(9, ac, av);
  int lcol = pd_getarg_int(10, ac, av);
  int maxel = (int)atom_getfloatarg(11, ac, av);
  
  n_canvas_size(x, w, h);
  n_canvas_send(x, snd);
  n_canvas_receive(x, rcv);
  n_canvas_label(x, lab);
  n_canvas_label_pos(x, ldx, ldy);
  n_canvas_label_font(x, fontsize);
  n_canvas_color(x, bcol, fcol, lcol);
  if (maxel != x->maxel)
    {
      n_canvas_maxel(x, maxel);
    }
  if (s) {} // disabled
}



//----------------------------------------------------------------------------//
static void *n_canvas_new(t_symbol *s, int ac, t_atom *av)
{
  t_n_canvas *x = (t_n_canvas *)pd_new(n_canvas_class);
  
  // empty symbol
  x->s_empty = gensym("empty");
  
  // $0
  x->dollarzero = canvas_getdollarzero();
  
  // arguments
  if ((ac == 12) 
      && IS_A_FLOAT(av, 0)                           //w
      && IS_A_FLOAT(av, 1)                           //h
      && (IS_A_FLOAT(av, 2) || IS_A_SYMBOL(av, 2))   //send
      && (IS_A_FLOAT(av, 3) || IS_A_SYMBOL(av, 3))   //receive
      && (IS_A_FLOAT(av, 4) || IS_A_SYMBOL(av, 4))   //label
      && IS_A_FLOAT(av, 5)                           //ldx
      && IS_A_FLOAT(av, 6)                           //ldy
      && IS_A_FLOAT(av, 7)                           //fontsize
      && (IS_A_FLOAT(av, 8) || IS_A_SYMBOL(av, 8))   //bcol
      && (IS_A_FLOAT(av, 9) || IS_A_SYMBOL(av, 9))   //fcol
      && (IS_A_FLOAT(av, 10) || IS_A_SYMBOL(av, 10)) //lcol
      && IS_A_FLOAT(av, 11))                         //maxel
    {
      x->x_w = atom_getfloatarg(0, ac, av);
      x->x_h = atom_getfloatarg(1, ac, av);
      x->x_snd = pd_getarg_symbol(2, ac, av);
      x->x_rcv = pd_getarg_symbol(3, ac, av);
      x->x_lab = pd_getarg_symbol(4, ac, av);
      x->x_ldx = atom_getfloatarg(5, ac, av);
      x->x_ldy = atom_getfloatarg(6, ac, av);
      x->x_fontsize = atom_getfloatarg(7, ac, av);
      x->x_bcol = pd_getarg_int(8, ac, av);
      x->x_fcol = pd_getarg_int(9, ac, av);
      x->x_lcol = pd_getarg_int(10, ac, av);
      x->maxel = atom_getfloatarg(11, ac, av);
    }
  else
    {
      x->x_w = 100;
      x->x_h = 100;
      x->x_snd = x->s_empty;
      x->x_rcv = x->s_empty;
      x->x_lab = x->s_empty;
      x->x_ldx = 10;
      x->x_ldy = 10;
      x->x_fontsize = 11;
      x->x_bcol = 0;
      x->x_fcol = 22;
      x->x_lcol = 22;
      x->maxel = 1;
      pd_color(&x->x_bcol);
      pd_color(&x->x_fcol);
      pd_color(&x->x_lcol);
    }
  
  // size
  AF_CLIP_MIN(1, x->x_w);
  AF_CLIP_MIN(1, x->x_h);
  
  // snd
  n_canvas_send(x, x->x_snd);
  
  // rcv
  x->x_rcv_able = 0;
  n_canvas_receive(x, x->x_rcv);
  
  // fontsize
  AF_CLIP_MINMAX(4, 256, x->x_fontsize);
  
  // maxel
  AF_CLIP_MIN(1, x->maxel);
  
  // mem
  n_canvas_mem(x);
  
  // ...
  x->x_glist = (t_glist *)canvas_getcurrent();
  x->x_canvas = glist_getcanvas(x->x_glist);
  
  // outlet
  outlet_new(&x->x_obj, 0);

  return (x);
  if (s) {} // disabled
}

//----------------------------------------------------------------------------//
static void n_canvas_ff(t_n_canvas *x)
{
  n_canvas_free(x);
  // unbind
  if (x->x_rcv_able)
    {
      pd_unbind(&x->x_obj.ob_pd, x->x_rcv_real);
    }
}

//----------------------------------------------------------------------------//
static void n_canvas_displace(t_gobj *z, t_glist *glist, int dx, int dy)
{
  t_n_canvas *x = (t_n_canvas *)z;
  x->x_obj.te_xpix += dx;
  x->x_obj.te_ypix += dy;
  n_canvas_draw_move(x, glist);
  canvas_fixlinesfor(glist, (t_text *)z);
}

//----------------------------------------------------------------------------//
static void n_canvas_delete(t_gobj *z, t_glist *glist)
{
  t_n_canvas *x = (t_n_canvas *)z;
  n_canvas_draw_erase(x, glist);
  canvas_deletelinesfor(glist, (t_text *)z);
}

//----------------------------------------------------------------------------//
static void n_canvas_vis(t_gobj *z, t_glist *glist, int vis)
{
  t_n_canvas *x = (t_n_canvas *)z;
  
  if (vis)
    {
      n_canvas_draw_new(x, glist);
    }
  else
    {
      n_canvas_draw_erase(x, glist);
      sys_unqueuegui(z);
    }
}



//----------------------------------------------------------------------------//
void n_canvas_setup(void)
{
  n_canvas_class = class_new(gensym("n_canvas"), (t_newmethod)n_canvas_new, (t_method)n_canvas_ff, sizeof(t_n_canvas), 0, A_GIMME, 0);
  class_addlist(n_canvas_class, (t_method)n_canvas_list);
  class_addmethod(n_canvas_class, (t_method)n_canvas_size, gensym("size"), A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_maxel, gensym("maxel"), A_DEFFLOAT, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_send, gensym("send"), A_DEFSYMBOL, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_receive, gensym("receive"), A_DEFSYMBOL, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_label, gensym("label"), A_DEFSYMBOL, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_label_pos, gensym("label_pos"), A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_label_font, gensym("label_font"), A_DEFFLOAT, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_color, gensym("color"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(n_canvas_class, (t_method)n_canvas_dialog, gensym("dialog"), A_GIMME, 0);
  n_canvas_widgetbehavior.w_getrectfn    = n_canvas_getrect;
  n_canvas_widgetbehavior.w_displacefn   = n_canvas_displace;
  n_canvas_widgetbehavior.w_selectfn     = NULL;
  n_canvas_widgetbehavior.w_activatefn   = NULL;
  n_canvas_widgetbehavior.w_deletefn     = n_canvas_delete;
  n_canvas_widgetbehavior.w_visfn        = n_canvas_vis;
  n_canvas_widgetbehavior.w_clickfn      = n_canvas_newclick;
  class_setwidget(n_canvas_class, &n_canvas_widgetbehavior);
  class_setsavefn(n_canvas_class, n_canvas_save);
  class_setpropertiesfn(n_canvas_class, n_canvas_properties);
  sys_vgui("eval [read [open {%s/n_canvas.tcl}]]\n", n_canvas_class->c_externdir->s_name);
}
