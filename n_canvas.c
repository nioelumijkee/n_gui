//----------------------------------------------------------------------------//
#include <stdlib.h>
#include <string.h>
#include "m_pd.h"
#include "m_imp.h"
#include "g_canvas.h"
#include "g_all_guis.h"

//----------------------------------------------------------------------------//
#define DEBUG(X) X
#define MAXEL 65536 // 2^16
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

//----------------------------------------------------------------------------//
#define CLIP_MINMAX(MIN, MAX, IN)    \
  if ((IN) < (MIN))                  \
    (IN) = (MIN);                    \
  else if ((IN) > (MAX))             \
    (IN) = (MAX);

#define CLIP_MIN(MIN, IN)    \
  if ((IN) < (MIN))          \
    (IN) = (MIN);

#define CLIP_MAX(MAX, IN)    \
  if ((IN) > (MAX))          \
    (IN) = (MAX);


//----------------------------------------------------------------------------//
t_widgetbehavior n_canvas_widgetbehavior;
static t_class *n_canvas_class;

//----------------------------------------------------------------------------//
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

//----------------------------------------------------------------------------//
typedef struct _n_canvas
{
  t_object x_obj;
  t_canvas *x_canvas;
  t_glist *x_glist;
  t_symbol *x_bindname;
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
  int m_bp;
  int m_shift;
  int m_alt;
  int m_br;
  int m_dbl;
  int maxel; /* max elements */
  struct _n_canvas_element *e;
  t_symbol *s_empty; /* */
  int dollarzero;
} t_n_canvas;

//----------------------------------------------------------------------------//
// pd function
//----------------------------------------------------------------------------//
void pd_dollar_in_string(char *str)
{
	int i = 0;
	int dollar = -1;
	while (str[i] != '\0')
	{
		if (str[i] == '$')
			dollar = i;
		i++;
	}
	if (dollar != -1)
	{
		str[i + 1] = '\0';
		while (i != dollar)
		{
			str[i] = str[i - 1];
			i--;
		}
		str[i] = '\\';
	}
}

//----------------------------------------------------------------------------//
int pd_getarg_int(int n, int ac, t_atom *av)
{
	t_symbol *buf;
	if (IS_A_FLOAT(av, n))
	{
		return (int)atom_getfloatarg(n, ac, av);
	}
	else
	{
		buf = (t_symbol *)atom_getsymbolarg(n, ac, av);
		return atoi(buf->s_name);
	}
}

//----------------------------------------------------------------------------//
t_symbol *pd_getarg_symbol(int n, int ac, t_atom *av)
{
	char buf[20];
	if (IS_A_FLOAT(av, n))
	{
		sprintf(buf, "%d", (int)atom_getfloatarg(n, ac, av));
		return (t_symbol *)gensym(buf);
	}
	else
	{
		return (t_symbol *)atom_getsymbolarg(n, ac, av);
	}
}

//----------------------------------------------------------------------------//
t_symbol *pd_dollarzero2sym(t_symbol *s, int id)
{
	char buf[256];
	char buf_id[8];
	int i,j,k;
	sprintf(buf_id,"%d",id);
	i = 0;
	j = 0;
	while (s->s_name[i] != '\0')
	{
		buf[j] = s->s_name[i];
		if (s->s_name[i] == '$' && s->s_name[i+1] == '0')
		{
			k = 0;
			while(buf_id[k] != '\0')
			{
				buf[j] = buf_id[k];
				k++;
				j++;
			}
			j--;
			i++;
		}
		i++;
		j++;
	}
	buf[j] = '\0';
	return gensym(buf);
}

//----------------------------------------------------------------------------//
int pd_color_30[] =
  {
    16579836, 10526880, 4210752, 16572640, 16572608,
    16579784, 14220504, 14220540, 14476540, 16308476,
    14737632, 8158332, 2105376, 16525352, 16559172,
    15263784, 1370132, 2684148, 3952892, 16003312,
    12369084, 6316128, 0, 9177096, 5779456,
    7874580, 2641940, 17488, 5256, 5767248};

//----------------------------------------------------------------------------//
void pd_color(int *col)
{
    if (*col >= 0)
    {
        *col = *col % 30;
        *col = pd_color_30[*col];
    }
    else
    {
        *col = (0 - *col) & 0x00ffffff;
    }
}

//----------------------------------------------------------------------------//
// draw function
//----------------------------------------------------------------------------//
inline void pd_cf_erase(t_n_canvas *x, int id)
{
  sys_vgui(".x%lx.c delete t%lx%d\n",
	   x->x_canvas,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pd_cf_coords_4(t_n_canvas *x, int id,
			   int x0,
			   int y0,
			   int x1,
			   int y1)
{
  sys_vgui(".x%lx.c coords t%lx%d %d %d %d %d\n",
	   x->x_canvas,
	   x, id,
	   x0,
	   y0,
	   x1,
	   y1);
}

//----------------------------------------------------------------------------//
inline void pd_cf_coords_2(t_n_canvas *x, int id,
			   int x0,
			   int y0)
{
  sys_vgui(".x%lx.c coords t%lx%d %d %d\n",
	   x->x_canvas,
	   x, id,
	   x0,
	   y0);
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_line(t_n_canvas *x, int id,
			     int col)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -fill #%6.6x\n",
	   x->x_canvas,
	   x, id,
	   col);
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_1(t_n_canvas *x, int id,
			  int col)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -outline #%6.6x\n",
	   x->x_canvas,
	   x, id,
	   col);
}

//----------------------------------------------------------------------------//
inline void pd_cf_color_2(t_n_canvas *x, int id,
			  int fcol,
			  int bcol)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -outline #%6.6x -fill #%6.6x\n",
	   x->x_canvas,
	   x, id,
	   fcol,
	   bcol);
}

//----------------------------------------------------------------------------//
inline void pd_cf_w(t_n_canvas *x, int id,
		    int w)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -width %d\n",
	   x->x_canvas,
	   x, id,
	   w);
}

//----------------------------------------------------------------------------//
inline void pd_cf_fs(t_n_canvas *x, int id,
		     int fs)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -anchor w -font {{%s} -%d %s}\n",
	   x->x_canvas,
	   x, id,
	   sys_font,
	   fs,
	   sys_fontweight);
}

//----------------------------------------------------------------------------//
inline void pd_cf_stex(t_n_canvas *x, int id,
		       int st,
		       int ex)
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d  -start %d -extent %d\n",
	   x->x_canvas,
	   x, id,
	   st,
	   ex);
}

//----------------------------------------------------------------------------//
inline void pd_cf_tx(t_n_canvas *x, int id,
		     const char text[])
{
  sys_vgui(".x%lx.c itemconfigure t%lx%d -text {%s}\n",
	   x->x_canvas,
	   x, id,
	   text);
}

//----------------------------------------------------------------------------//
inline void pd_cf_line(t_n_canvas *x, int id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui(".x%lx.c create line %d %d %d %d -fill #%6.6x -width %d -tags t%lx%d\n",
	   x->x_canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pd_cf_rect(t_n_canvas *x, int id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -width %d -tags t%lx%d\n",
	   x->x_canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pd_cf_rect_filled(t_n_canvas *x, int id,
			      int fcol,
			      int bcol,
			      int w,
			      int x0,
			      int y0,
			      int x1,
			      int y1)
{
  sys_vgui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags t%lx%d\n",
	   x->x_canvas,
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
inline void pd_cf_oval(t_n_canvas *x, int id,
		       int col,
		       int w,
		       int x0,
		       int y0,
		       int x1,
		       int y1)
{
  sys_vgui(".x%lx.c create oval %d %d %d %d -outline #%6.6x -width %d -tags t%lx%d\n",
	   x->x_canvas,
	   x0,
	   y0,
	   x1,
	   y1,
	   col,
	   w,
	   x, id);
}

//----------------------------------------------------------------------------//
inline void pd_cf_oval_filled(t_n_canvas *x, int id,
			      int fcol,
			      int bcol,
			      int w,
			      int x0,
			      int y0,
			      int x1,
			      int y1)
{
  sys_vgui(".x%lx.c create oval %d %d %d %d -outline #%6.6x -fill #%6.6x -width %d -tags t%lx%d\n",
	   x->x_canvas,
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
inline void pd_cf_arc(t_n_canvas *x, int id,
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
	   x->x_canvas,
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
inline void pd_cf_arc_filled(t_n_canvas *x, int id,
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
	   x->x_canvas,
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
inline void pd_cf_text(t_n_canvas *x, int id,
		       int col,
		       int fs,
		       int x0,
		       int y0,
		       const char text[])
{
  sys_vgui(".x%lx.c create text %d %d -text {%s} -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags t%lx%d\n",
	   x->x_canvas,
	   x0,
	   y0,
	   text,
	   sys_font,
	   fs,
	   sys_fontweight,
	   col,
	   x, id);
}


//----------------------------------------------------------------------------//
// draw
//----------------------------------------------------------------------------//
void n_canvas_list(t_n_canvas *x, t_symbol *s, int ac, t_atom *av)
{
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
		pd_cf_erase(x, id);
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
	  CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase(x, id);
	      x->e[id].type = LINE;
	      
	      // create
	      pd_cf_line(x, id,
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
	  CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase(x, id);
	      x->e[id].type = RECT;
	      
	      // create
	      pd_cf_rect(x, id,
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
	  CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase(x, id);
	      x->e[id].type = RECT_F;
	      
	      // create
	      pd_cf_rect_filled(x, id,
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
	  CLIP_MIN(1, x->e[id].s);
	  
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase(x, id);
	      x->e[id].type = OVAL;
	      
	      // create
	      pd_cf_oval(x, id,
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
	  CLIP_MIN(1, x->e[id].s);
	    
	  if (glist_isvisible(x->x_glist))
	    {
	      // erase
	      if (x->e[id].type != ERASE)
		pd_cf_erase(x, id);
	      x->e[id].type = OVAL_F;
		
		// create
		pd_cf_oval_filled(x, id,
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
	  CLIP_MIN(1, x->e[id].s)
	    
	    if (glist_isvisible(x->x_glist))
	      {
		// erase
		if (x->e[id].type != ERASE)
		  pd_cf_erase(x, id);
		x->e[id].type = ARC;
		
		// create
		pd_cf_arc(x, id,
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
	  CLIP_MIN(1, x->e[id].s)
	    
	    if (glist_isvisible(x->x_glist))
	      {
		// erase
		if (x->e[id].type != ERASE)
		  pd_cf_erase(x, id);
		x->e[id].type = ARC_F;
		
		// create
		pd_cf_arc_filled(x, id,
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
	  CLIP_MIN(4, x->e[id].s)
	    
	    if (glist_isvisible(x->x_glist))
	      {
		// erase
		if (x->e[id].type != ERASE)
		  pd_cf_erase(x, id);
		x->e[id].type = TEXT;
		
		// create
		pd_cf_text(x, id,
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
		  pd_cf_coords_2(x, id,
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
		  pd_cf_coords_4(x, id,
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
		  pd_cf_color_line(x, id,
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
		  pd_cf_color_1(x, id,
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
		  pd_cf_color_2(x, id,
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
	      CLIP_MIN(1, x->e[id].s)
		
		if (glist_isvisible(x->x_glist))
		  {
		    // w
		    pd_cf_w(x, id,
			    x->e[id].s);
		  }
	    }
	  
	  // fs
	  else if (x->e[id].type == TEXT)
	    {
	      // write
	      x->e[id].s = atom_getfloatarg(2, ac, av);
	      CLIP_MIN(4, x->e[id].s)
		
		if (glist_isvisible(x->x_glist))
		  {
		    // fs
		    pd_cf_fs(x, id,
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
		  pd_cf_stex(x, id,
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
		  pd_cf_tx(x, id,
			   x->e[id].text->s_name);
		}
	    }
	}
      break;
    }
  if (s) {} // disabled
}

//----------------------------------------------------------------------------//
void n_canvas_draw_new(t_n_canvas *x, t_glist *glist)
{
  x->xpos = text_xpix(&x->x_obj, glist);
  x->ypos = text_ypix(&x->x_obj, glist);
  x->x_canvas = glist_getcanvas(glist);
  
  pd_cf_rect_filled(x, M_BASE,
		    x->x_fcol,
		    x->x_bcol,
		    1,
		    x->xpos,
		    x->ypos,
		    x->xpos + x->x_w,
		    x->ypos + x->x_h);

  // bind main rect
  sys_vgui(".x%lx.c bind t%lx%d <ButtonRelease> \
           {pdsend [concat %s _br \\;]}\n", 
           x->x_canvas, x, M_BASE, 
           x->x_bindname->s_name);
 
  pd_cf_text(x, M_LABEL,
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
	  pd_cf_line(x, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->xpos + x->e[id].x[1],
		     x->ypos + x->e[id].y[1]);
	  break;
	  
	case RECT:
	  pd_cf_rect(x, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->xpos + x->e[id].x[1],
		     x->ypos + x->e[id].y[1]);
	  break;
	  
	case RECT_F:
	  pd_cf_rect_filled(x, id,
			    x->e[id].fcol,
			    x->e[id].bcol,
			    x->e[id].s,
			    x->xpos + x->e[id].x[0],
			    x->ypos + x->e[id].y[0],
			    x->xpos + x->e[id].x[1],
			    x->ypos + x->e[id].y[1]);
	  break;
	  
	case OVAL:
	  pd_cf_oval(x, id,
		     x->e[id].fcol,
		     x->e[id].s,
		     x->xpos + x->e[id].x[0],
		     x->ypos + x->e[id].y[0],
		     x->xpos + x->e[id].x[1],
		     x->ypos + x->e[id].y[1]);
	  break;
	  
	case OVAL_F:
	  pd_cf_oval_filled(x, id,
			    x->e[id].fcol,
			    x->e[id].bcol,
			    x->e[id].s,
			    x->xpos + x->e[id].x[0],
			    x->ypos + x->e[id].y[0],
			    x->xpos + x->e[id].x[1],
			    x->ypos + x->e[id].y[1]);
	  break;
	  
	case ARC:
	  pd_cf_arc(x, id,
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
	  pd_cf_arc_filled(x, id,
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
	  pd_cf_text(x, id,
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
void n_canvas_draw_move(t_n_canvas *x, t_glist *glist)
{
  x->xpos = text_xpix(&x->x_obj, glist);
  x->ypos = text_ypix(&x->x_obj, glist);
  x->x_canvas = glist_getcanvas(glist);
  
  pd_cf_coords_4(x, M_BASE,
		 x->xpos,
		 x->ypos,
		 x->xpos + x->x_w,
		 x->ypos + x->x_h);
  
  pd_cf_coords_2(x, M_LABEL,
		 x->xpos + x->x_ldx,
		 x->ypos + x->x_ldy);
  
  for (int id = 0; id < x->maxel; id++)
    {
      if (x->e[id].type > ERASE && x->e[id].type < TEXT)
	pd_cf_coords_4(x, id,
		       x->xpos + x->e[id].x[0],
		       x->ypos + x->e[id].y[0],
		       x->xpos + x->e[id].x[1],
		       x->ypos + x->e[id].y[1]);
      
      else if (x->e[id].type == TEXT)
	pd_cf_coords_2(x, id,
		       x->xpos + x->e[id].x[0],
		       x->ypos + x->e[id].y[0]);
    }
}

//----------------------------------------------------------------------------//
void n_canvas_draw_erase(t_n_canvas *x, t_glist *glist)
{
  x->x_canvas = glist_getcanvas(glist);
  if(glist_isvisible(x->x_glist))
    { 
      pd_cf_erase(x, M_BASE);
      pd_cf_erase(x, M_LABEL);
      
      for (int id = 0; id < x->maxel; id++)
	if (x->e[id].type > 0 && x->e[id].type < 9)
	  pd_cf_erase(x, id);
    }
}

//----------------------------------------------------------------------------//
void n_canvas_draw_config(t_n_canvas *x)
{
  if(glist_isvisible(x->x_glist))
    { 
      pd_cf_coords_4(x, M_BASE,
		     x->xpos,
		     x->ypos,
		     x->xpos + x->x_w,
		     x->ypos + x->x_h);
      
      pd_cf_color_2(x, M_BASE,
		    x->x_fcol,
		    x->x_bcol);
      
      pd_cf_coords_2(x, M_LABEL,
		     x->xpos + x->x_ldx,
		     x->ypos + x->x_ldy);
      
      pd_cf_color_line(x, M_LABEL,
		       x->x_lcol);
      
      pd_cf_tx(x, M_LABEL,
	       (strcmp(x->x_lab->s_name, "empty") ? x->x_lab->s_name : " "));
      
      pd_cf_fs(x, M_LABEL,
	       x->x_fontsize);
    }
}

//----------------------------------------------------------------------------//
// motion
//----------------------------------------------------------------------------//
void n_canvas_mouseout(t_n_canvas *x)
{
  t_atom a[6];
  int bs; // button state
  if (x->m_br == 0) bs = x->m_bp;  // press
  else              bs = 0;        // release
  SETFLOAT(a,     (t_float)bs);
  SETFLOAT(a + 1, (t_float)x->m_dbl);
  SETFLOAT(a + 2, (t_float)x->m_xp);
  SETFLOAT(a + 3, (t_float)x->m_yp);
  SETFLOAT(a + 4, (t_float)x->m_shift);
  SETFLOAT(a + 5, (t_float)x->m_alt);
  outlet_list(x->x_obj.ob_outlet, &s_list, 6, a);
  if (x->x_snd_able && x->x_snd_real->s_thing)
    {
      pd_list(x->x_snd_real->s_thing, &s_list, 6, a);
    }
}

//----------------------------------------------------------------------------//
void n_canvas_motion(t_n_canvas *x, t_floatarg dx, t_floatarg dy)
{
  x->m_xp += dx;
  x->m_yp += dy;
  x->m_bp = 2;
  n_canvas_mouseout(x);
}

//----------------------------------------------------------------------------//
void n_canvas_click(t_n_canvas *x, int xpix, int ypix)
{
  glist_grab(x->x_glist,
	     &x->x_obj.te_g,
	     (t_glistmotionfn)n_canvas_motion,
	     (t_glistkeyfn)NULL,
	     (t_floatarg)xpix,
	     (t_floatarg)ypix);
  x->m_xp = xpix - x->xpos;
  x->m_yp = ypix - x->ypos;
  x->m_bp = 1;
  x->m_br = 0; // release
  n_canvas_mouseout(x);
}

//----------------------------------------------------------------------------//
int n_canvas_newclick(t_gobj *z, struct _glist *glist, 
		      int xpix, int ypix, 
		      int shift, int alt, int dbl, int doit)
{
  t_n_canvas *x = (t_n_canvas *)z;
  x->xpos = text_xpix(&x->x_obj, glist);
  x->ypos = text_ypix(&x->x_obj, glist);
  x->m_xp = xpix - x->xpos;
  x->m_yp = ypix - x->ypos;
  x->m_bp = 0;
  x->m_shift = shift;
  x->m_alt = alt;
  x->m_dbl = dbl;
  n_canvas_mouseout(x);
  if (doit)
    n_canvas_click(x, xpix, ypix);
  return (1);
}

//----------------------------------------------------------------------------//
void n_canvas_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1, int *xp2, int *yp2)
{
  t_n_canvas *x = (t_n_canvas *)z;
  *xp1 = text_xpix(&x->x_obj, glist);
  *yp1 = text_ypix(&x->x_obj, glist);
  *xp2 = *xp1 + x->x_w;
  *yp2 = *yp1 + x->x_h;
}

//----------------------------------------------------------------------------//
// mem
//----------------------------------------------------------------------------//
void n_canvas_free(t_n_canvas *x)
{
  free(x->e);
}

//----------------------------------------------------------------------------//
void n_canvas_mem(t_n_canvas *x)
{
  x->e = calloc(x->maxel, sizeof(struct _n_canvas_element));
  if (!x->e)
    {
      post("n_canvas: error allocating memory");
    }
}

//----------------------------------------------------------------------------//
// input
//----------------------------------------------------------------------------//
void n_canvas_size(t_n_canvas *x, t_floatarg w, t_floatarg h)
{
  x->x_w = w;
  x->x_h = h;
  CLIP_MIN(1, x->x_w);
  CLIP_MIN(1, x->x_h);
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
void n_canvas_send(t_n_canvas *x, t_symbol *s)
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
void n_canvas_receive(t_n_canvas *x, t_symbol *s)
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
void n_canvas_label(t_n_canvas *x, t_symbol *s)
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
void n_canvas_label_pos(t_n_canvas *x, t_floatarg ldx, t_floatarg ldy)
{
  x->x_ldx = ldx;
  x->x_ldy = ldy;
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
void n_canvas_label_font(t_n_canvas *x, t_floatarg fs)
{
  CLIP_MINMAX(4, 256, fs);
  x->x_fontsize = fs;
  n_canvas_draw_config(x);
}

//----------------------------------------------------------------------------//
void n_canvas_color(t_n_canvas *x, t_floatarg bcol, t_floatarg fcol, t_floatarg lcol)
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
void n_canvas_maxel(t_n_canvas *x, t_floatarg f)
{
  // erase
  if (glist_isvisible(x->x_glist))
    {
      for (int id = 0; id < x->maxel; id++)
	{
	  if (x->e[id].type > ERASE && x->e[id].type <= TEXT)
	    {
	      pd_cf_erase(x, id);
	    }
	}
    }
  
  // memory
  x->maxel = f;
  CLIP_MINMAX(1, MAXEL, x->maxel);
  n_canvas_free(x);
  n_canvas_mem(x);
}

//----------------------------------------------------------------------------//
// callback
// -------------------------------------------------------------------------- //
static void n_canvas_br_callback(t_n_canvas* x)
{ 
  if(!x->x_glist->gl_edit)
    {
      x->m_br = 1; // release
    }
}

//----------------------------------------------------------------------------//
// save
//----------------------------------------------------------------------------//
void n_canvas_save(t_gobj *z, t_binbuf *b)
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
void n_canvas_properties(t_gobj *z, t_glist *owner)
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
void n_canvas_dialog(t_n_canvas *x, t_symbol *s, int ac, t_atom *av)
{
  if (!x)
    {
      post("n_canvas: error:  unexisting object");
      return;
    }
  if (ac != 12)
    {
      post("n_canvas: error: number arguments");
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
// displays
//----------------------------------------------------------------------------//
void n_canvas_displace(t_gobj *z, t_glist *glist, int dx, int dy)
{
  t_n_canvas *x = (t_n_canvas *)z;
  x->x_obj.te_xpix += dx;
  x->x_obj.te_ypix += dy;
  n_canvas_draw_move(x, glist);
  canvas_fixlinesfor(glist, (t_text *)z);
}

//----------------------------------------------------------------------------//
void n_canvas_delete(t_gobj *z, t_glist *glist)
{
  t_n_canvas *x = (t_n_canvas *)z;
  n_canvas_draw_erase(x, glist);
  canvas_deletelinesfor(glist, (t_text *)z);
}

//----------------------------------------------------------------------------//
void n_canvas_vis(t_gobj *z, t_glist *glist, int vis)
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
// setup
//----------------------------------------------------------------------------//
void *n_canvas_new(t_symbol *s, int ac, t_atom *av)
{
  char buf[MAXPDSTRING];
  t_n_canvas *x = (t_n_canvas *)pd_new(n_canvas_class);
  
  // empty symbol
  x->s_empty = gensym("empty");
  
  // $0
  x->dollarzero = canvas_getdollarzero();

  // bind
  sprintf(buf, "#%lx", (long)x);
  pd_bind(&x->x_obj.ob_pd, x->x_bindname = gensym(buf));
  
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
  CLIP_MIN(1, x->x_w);
  CLIP_MIN(1, x->x_h);
  
  // snd
  n_canvas_send(x, x->x_snd);
  
  // rcv
  x->x_rcv_able = 0;
  n_canvas_receive(x, x->x_rcv);
  
  // fontsize
  CLIP_MINMAX(4, 256, x->x_fontsize);
  
  // ...
  x->x_glist = (t_glist *)canvas_getcurrent();
  x->x_canvas = glist_getcanvas(x->x_glist);

  // maxel
  CLIP_MINMAX(1, MAXEL, x->maxel);
  n_canvas_mem(x);
  
  // outlet
  outlet_new(&x->x_obj, 0);

  return (x);
  if (s) {} // disabled
}

//----------------------------------------------------------------------------//
void n_canvas_ff(t_n_canvas *x)
{
  n_canvas_free(x);
  pd_unbind(&x->x_obj.ob_pd, x->x_bindname);
  // unbind
  if (x->x_rcv_able)
    {
      pd_unbind(&x->x_obj.ob_pd, x->x_rcv_real);
    }
}

//----------------------------------------------------------------------------//
void n_canvas_setup(void)
{
  n_canvas_class=class_new(gensym("n_canvas"),(t_newmethod)n_canvas_new,
			   (t_method)n_canvas_ff,sizeof(t_n_canvas),0,A_GIMME,0);
  class_addlist(n_canvas_class,(t_method)n_canvas_list);
  class_addmethod(n_canvas_class,(t_method)n_canvas_size,
		  gensym("size"),A_FLOAT,A_FLOAT,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_maxel,
		  gensym("maxel"),A_FLOAT,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_send,
		  gensym("send"),A_DEFSYMBOL,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_receive,
		  gensym("receive"),A_DEFSYMBOL,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_label,
		  gensym("label"),A_DEFSYMBOL,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_label_pos,
		  gensym("label_pos"),A_FLOAT,A_FLOAT,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_label_font,
		  gensym("label_font"),A_FLOAT,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_color,
		  gensym("color"),A_FLOAT,A_FLOAT,A_FLOAT,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_dialog,
		  gensym("dialog"),A_GIMME,0);
  class_addmethod(n_canvas_class,(t_method)n_canvas_br_callback,
		  gensym("_br"),0);
  n_canvas_widgetbehavior.w_getrectfn=n_canvas_getrect;
  n_canvas_widgetbehavior.w_displacefn=n_canvas_displace;
  n_canvas_widgetbehavior.w_selectfn=NULL;
  n_canvas_widgetbehavior.w_activatefn=NULL;
  n_canvas_widgetbehavior.w_deletefn=n_canvas_delete;
  n_canvas_widgetbehavior.w_visfn=n_canvas_vis;
  n_canvas_widgetbehavior.w_clickfn=n_canvas_newclick;
  class_setwidget(n_canvas_class,&n_canvas_widgetbehavior);
  class_setsavefn(n_canvas_class,n_canvas_save);
  class_setpropertiesfn(n_canvas_class,n_canvas_properties);
  sys_vgui("eval [read [open {%s/n_canvas.tcl}]]\n", n_canvas_class->c_externdir->s_name);
}
