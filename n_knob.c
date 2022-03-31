/*

  sources of ideas:
  ggee library: image
  else library: pic
  GPL v.3
  2021(c)Nioelumijke

*/

#include <string.h>
#include "m_pd.h"
#include "m_imp.h"
#include "g_canvas.h"
#include "g_all_guis.h"
#include "include/pd_func2.h"
#include "include/clip.h"

#define HOR 0
#define VER 1
#define NOINIT 0
#define INIT 1
#define NEVERST -99999999


static t_class *n_knob_class;
t_widgetbehavior n_knob_widgetbehavior;
t_symbol *extdir;

typedef struct _n_knob
{
  t_object x_obj;
  t_glist * x_glist;
  t_symbol *x_bindname;
  int dollarzero;
  t_symbol *curdir;

  t_symbol *filename;
  int orientation;
  int frames;
  t_float min;
  t_float max;
  t_float resolution;
  t_float step;
  t_float default_state;
  t_symbol *snd;
  t_symbol *rcv;
  t_symbol *lab;
  int lab_fs;
  int ldx;
  int ldy;
  int lcol;
  t_float state;
  int init;
  int num_vis;
  int numw;
  int num_fs;
  int ndx;
  int ndy;
  int ncol;

  int img_w;
  int img_h;
  int frame_w;
  int frame_h;
  int frame_w_2;
  int frame_h_2;
  t_float state_old;
  t_float diff;
  int minmaxdir;
  int framepos;
  int framepos_old;
  int sel;
  int shift;
  t_float count;
  int lab_vis;
  t_symbol *snd_real;
  int snd_able;
  t_symbol *rcv_real;
  int rcv_able;
} t_n_knob;


// -------------------------------------------------------------------------- //
t_float quantize(t_float f, t_float step)
{
  int i;
  if (step > 0)
    {
      if (f > 0)
        {
          i = (f / step) + 0.49999;
          f = i * step;
        }
      else
        {
          i = (f / step) - 0.49999;
          f = i * step;
        }
    }
  return (f);
}

// -------------------------------------------------------------------------- //
// sys_vgui
// -------------------------------------------------------------------------- //
void n_knob_bind_image(t_n_knob *x)
{
  if (x->img_w == -1) return;
  sys_vgui(".x%x.c bind %xI <Double-Button-1> \
           {pdsend [concat %s _double \\;]}\n", 
           glist_getcanvas(x->x_glist), 
           x, 
           x->x_bindname->s_name);
}

// -------------------------------------------------------------------------- //
static void n_knob_draw_image(t_n_knob *x)
{
  if (x->img_w == -1) return;
  int ofs_x0, ofs_y0, ofs_x1, ofs_y1;
  // only vertical
  ofs_x0 = 0;
  ofs_y0 = x->framepos * x->frame_h;
  ofs_x1 = x->frame_w;
  ofs_y1 = ofs_y0 + x->frame_h;

  sys_vgui("image create photo img%x\n",x);
  sys_vgui("img%x copy imgsrc%x -from %d %d %d %d\n",
           x,
           x,
           ofs_x0,
           ofs_y0,
           ofs_x1,
           ofs_y1);
  sys_vgui(".x%x.c create image %d %d -image img%x -tags %xI\n", 
           glist_getcanvas(x->x_glist),
           text_xpix(&x->x_obj, x->x_glist) + x->frame_w_2, 
           text_ypix(&x->x_obj, x->x_glist) + x->frame_h_2,
           x,
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_draw_frame(t_n_knob *x)
{
  sys_vgui(".x%x.c create rectangle %d %d %d %d \
           -tags %xF -outline blue\n",
           glist_getcanvas(x->x_glist),
           text_xpix(&x->x_obj, x->x_glist) - 1, 
           text_ypix(&x->x_obj, x->x_glist) - 1,
           text_xpix(&x->x_obj, x->x_glist) + x->frame_w, 
           text_ypix(&x->x_obj, x->x_glist) + x->frame_h,
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_draw_label(t_n_knob *x)
{
  sys_vgui(".x%x.c create text %d %d -text {%s} \
       -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags %xL\n",
           glist_getcanvas(x->x_glist),
           text_xpix(&x->x_obj, x->x_glist) + x->ldx, 
           text_ypix(&x->x_obj, x->x_glist) + x->ldy,
           x->lab->s_name,
           sys_font,
           x->lab_fs,
           sys_fontweight,
           x->lcol,
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_draw_num(t_n_knob *x)
{
  char buf[32];
  if (x->state >= 0)   sprintf(buf," %-f",x->state);
  else                 sprintf(buf,"%-f",x->state);
  buf[x->numw + 1] = '\0';
  sys_vgui(".x%x.c create text %d %d -text {%s} \
           -anchor w -font {{%s} -%d %s} -fill #%6.6x -tags %xN\n",
           glist_getcanvas(x->x_glist),
           text_xpix(&x->x_obj, x->x_glist) + x->ndx, 
           text_ypix(&x->x_obj, x->x_glist) + x->ndy,
           buf,
           sys_font,
           x->num_fs,
           sys_fontweight,
           x->ncol,
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_coords_image(t_n_knob *x)
{
  sys_vgui(".x%x.c coords %xI %d %d\n",
           glist_getcanvas(x->x_glist), 
           x,
           text_xpix(&x->x_obj, x->x_glist) + x->frame_w_2, 
           text_ypix(&x->x_obj, x->x_glist) + x->frame_h_2);
}

// -------------------------------------------------------------------------- //
static void n_knob_coords_frame(t_n_knob *x)
{
  sys_vgui(".x%x.c coords %xF %d %d %d %d\n",
           glist_getcanvas(x->x_glist), 
           x,
           text_xpix(&x->x_obj, x->x_glist) - 1, 
           text_ypix(&x->x_obj, x->x_glist) - 1,
           text_xpix(&x->x_obj, x->x_glist) + x->frame_w, 
           text_ypix(&x->x_obj, x->x_glist) + x->frame_h);
}

// -------------------------------------------------------------------------- //
static void n_knob_coords_label(t_n_knob *x)
{
  sys_vgui(".x%x.c coords %xL %d %d\n",
           glist_getcanvas(x->x_glist), 
           x,
           text_xpix(&x->x_obj, x->x_glist) + x->ldx, 
           text_ypix(&x->x_obj, x->x_glist) + x->ldy);
}

// -------------------------------------------------------------------------- //
static void n_knob_coords_num(t_n_knob *x)
{
  sys_vgui(".x%x.c coords %xN %d %d\n",
           glist_getcanvas(x->x_glist), 
           x,
           text_xpix(&x->x_obj, x->x_glist) + x->ndx, 
           text_ypix(&x->x_obj, x->x_glist) + x->ndy);
}

// -------------------------------------------------------------------------- //
static void n_knob_erase_image(t_n_knob *x)
{
  if (x->img_w == -1) return;
  sys_vgui(".x%x.c delete %xI\n",
           glist_getcanvas(x->x_glist), 
           x);
  sys_vgui("image delete img%x\n",
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_erase_frame(t_n_knob *x)
{
  sys_vgui(".x%x.c delete %xF\n",
           glist_getcanvas(x->x_glist), 
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_erase_label(t_n_knob *x)
{
  sys_vgui(".x%x.c delete %xL\n",
           glist_getcanvas(x->x_glist), 
           x);
}

// -------------------------------------------------------------------------- //
static void n_knob_erase_num(t_n_knob *x)
{
  sys_vgui(".x%x.c delete %xN\n",
           glist_getcanvas(x->x_glist), 
           x);
}

// -------------------------------------------------------------------------- //
// widget
// -------------------------------------------------------------------------- //
static void n_knob_getrect(t_gobj *z, 
                           t_glist *glist,
                           int *xp1, 
                           int *yp1, 
                           int *xp2, 
                           int *yp2)
{
  t_n_knob* x = (t_n_knob*)z;
  *xp1 = text_xpix(&x->x_obj, glist);
  *yp1 = text_ypix(&x->x_obj, glist);
  *xp2 = text_xpix(&x->x_obj, glist) + x->frame_w;
  *yp2 = text_ypix(&x->x_obj, glist) + x->frame_h;
}

// -------------------------------------------------------------------------- //
static void n_knob_displace(t_gobj *z, 
                            t_glist *glist, 
                            int dx, 
                            int dy)
{
  t_n_knob *x = (t_n_knob *)z;
  x->x_obj.te_xpix += dx;
  x->x_obj.te_ypix += dy;
  n_knob_coords_image(x);
  if (x->sel)
    {
      n_knob_coords_frame(x);
    }
  if (x->lab_vis)
    {
      n_knob_coords_label(x);
    }
  if (x->num_vis)
    {
      n_knob_coords_num(x);
    }
  canvas_fixlinesfor(glist,(t_text*) x);
}

// -------------------------------------------------------------------------- //
static void n_knob_select(t_gobj *z, t_glist *glist, int state)
{
  t_n_knob *x = (t_n_knob *)z;
  x->x_glist = glist;
  if (state)
    {
      x->sel = 1;
      n_knob_draw_frame(x);
    }
  else 
    {
      x->sel = 0;
      n_knob_erase_frame(x);
    }
}


// -------------------------------------------------------------------------- //
static void n_knob_delete(t_gobj *z, t_glist *glist)
{
  t_text *x = (t_text *)z;
  canvas_deletelinesfor(glist, x);
}

       
// -------------------------------------------------------------------------- //
static void n_knob_vis(t_gobj *z, t_glist *glist, int vis)
{
  t_n_knob* x = (t_n_knob*)z;
  x->x_glist = glist;
  if (vis)
    {
      n_knob_draw_image(x);
      n_knob_bind_image(x);
      if (x->sel)
        {
          n_knob_draw_frame(x);
        }
      if (x->lab_vis)
        {
          n_knob_draw_label(x);
        }
      if (x->num_vis)
        {
          n_knob_draw_num(x);
        }
    }
  else
    {
      n_knob_erase_image(x);
      if (x->sel)
        {
          n_knob_erase_frame(x);
        }
      if (x->lab_vis)
        {
          n_knob_erase_label(x);
        }
      if (x->num_vis)
        {
          n_knob_erase_num(x);
        }
    }
}

// -------------------------------------------------------------------------- //
// out
//----------------------------------------------------------------------------//
static void n_knob_out(t_n_knob *x)
{
  outlet_float(x->x_obj.ob_outlet, x->state);
  if (x->snd_able && x->snd_real->s_thing)
    {
      pd_float(x->snd_real->s_thing, x->state);
    }
}

// -------------------------------------------------------------------------- //
// motion
//----------------------------------------------------------------------------//
static void n_knob_motion(t_n_knob *x, t_float dx, t_float dy)
{
  if (x->orientation == VER)
    {
      dy = 0 - dy;
      if (x->shift) dy = dy * 0.05;
      x->count += dy;
    }
  else
    {
      if (x->shift) dx = dx * 0.05;
      x->count += dx;
    }
  AF_CLIP_MINMAX(0, x->resolution, x->count);
  

  x->state = ((x->count / x->resolution) * x->diff) + x->min;
  x->state = quantize(x->state, x->step);

  if (x->state != x->state_old)
    {
      x->framepos = (x->count / x->resolution) * (x->frames - 1);
      if(glist_isvisible(x->x_glist) && gobj_shouldvis((t_gobj *)x, x->x_glist))
        {
          if (x->framepos != x->framepos_old)
            {
              n_knob_erase_image(x);
              n_knob_draw_image(x);
              n_knob_bind_image(x);
            }
          if (x->num_vis)
            {
              n_knob_erase_num(x);
              n_knob_draw_num(x);
            }
        }
      x->framepos_old = x->framepos;
      n_knob_out(x);
    }
  x->state_old = x->state;
}

// -------------------------------------------------------------------------- //
static int n_knob_newclick(t_gobj *z, 
                           t_glist *glist, 
                           int xpix, 
                           int ypix, 
                           int shift, 
                           int alt, 
                           int c, 
                           int doit)
{
  t_n_knob *x = (t_n_knob *)z;
  if (doit)
    {
      x->shift = shift;
      glist_grab(glist,
                 &x->x_obj.te_g,
                 (t_glistmotionfn)n_knob_motion,
                 (t_glistkeyfn)NULL,
                 (t_floatarg)xpix,
                 (t_floatarg)ypix);
    }
  return (1);
  if (alt) {};
  if (c) {};
}

// -------------------------------------------------------------------------- //
// methods
// -------------------------------------------------------------------------- //
static void n_knob_loadimage(t_n_knob *x, 
                             t_symbol *filename,
                             t_floatarg orientation,
                             t_floatarg frames)
{
  if (filename->s_name[0] == '\0') 
    {
      error("n_knob: bad filename");
      return;
    }
  x->img_w = -1;
  x->img_h = -1;
  x->filename = filename;
  x->orientation = orientation;
  AF_CLIP_MIN(1,frames);
  x->frames = frames;
  sys_vgui("image create photo img%x\n",x);
  sys_vgui("if { [file exists {%s/%s}] == 1 } { \
image create photo imgsrc%x -file {%s/%s};                              \
pdsend [concat %s _size [image width imgsrc%x] [image height imgsrc%x]] \
} elseif { [file exists {%s/images/%s}] == 1 } {			\
image create photo imgsrc%x -file {%s/images/%s};			\
pdsend [concat %s _size [image width imgsrc%x] [image height imgsrc%x]] \
} else {                                                                \
pdsend [concat %s _size -1 -1]}\n",
           x->curdir->s_name, x->filename->s_name,
           x, x->curdir->s_name, x->filename->s_name,
           x->x_bindname->s_name, x, x,
           extdir->s_name, x->filename->s_name,
           x, extdir->s_name, x->filename->s_name,
           x->x_bindname->s_name, x, x,
           x->x_bindname->s_name);
}


// -------------------------------------------------------------------------- //
static void n_knob_knobpar(t_n_knob* x, 
                    t_floatarg min, 
                    t_floatarg max,
                    t_floatarg step,
                    t_floatarg default_state,
                    t_floatarg resolution)
{
  t_float f;
  t_float maxstep;

  x->min = min;
  x->max = max;
  if (x->min > x->max) x->minmaxdir = 1;
  else                 x->minmaxdir = 0;

  if (step < 0) step = 0;
  if (resolution < 1) resolution = 1;

  x->diff = x->max - x->min;

  maxstep = x->diff;
  if (maxstep < 0) maxstep = 0 - maxstep;

  if      (step < 0) step = 0;
  else if (step > maxstep) step = maxstep;

  if (x->minmaxdir)
    {
      AF_CLIP_MINMAX(x->max, x->min, default_state);
    }
  else
    {
      AF_CLIP_MINMAX(x->min, x->max, default_state);
    }

  x->step = step;
  x->default_state = default_state;
  x->resolution = resolution;

  if (x->minmaxdir)
    {
      AF_CLIP_MINMAX(x->max, x->min, x->state);
    }
  else
    {
      AF_CLIP_MINMAX(x->min, x->max, x->state);
    }

  x->state = quantize(x->state, x->step);
  f = x->state - x->min;
  f = f / x->diff; 

  x->count = f * x->resolution;
  x->framepos = (x->count / x->resolution) * (x->frames - 1);
  if(glist_isvisible(x->x_glist) && gobj_shouldvis((t_gobj *)x, x->x_glist))
    {
      n_knob_erase_image(x);
      n_knob_draw_image(x);
      n_knob_bind_image(x);
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_snd(t_n_knob* x, t_symbol *s)
{
  if (!(strcmp(s->s_name, "empty")))
    {
      x->snd = s;
      x->snd_real = s;
      x->snd_able = 0;
    }
  else
    {
      x->snd = s;
      x->snd_real = dollarzero2sym(s, x->dollarzero);
      x->snd_able = 1;
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_rcv(t_n_knob* x, t_symbol *s)
{
  // unbind
  if (x->rcv_able)
    {
      pd_unbind(&x->x_obj.ob_pd, x->rcv_real);
    }
  if (!(strcmp(s->s_name, "empty")))
    {
      x->rcv = s;
      x->rcv_real = s;
      x->rcv_able = 0;
    }
  else
    {
      x->rcv = s;
      x->rcv_real = dollarzero2sym(s, x->dollarzero);
      x->rcv_able = 1;
    }
  // bind
  if (x->rcv_able)
    {
      pd_bind(&x->x_obj.ob_pd, x->rcv_real);
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_lab(t_n_knob* x, t_symbol *s)
{
  if (!(strcmp(s->s_name, "empty")))
    {
      x->lab = s;
      x->lab_vis = 0;
      n_knob_erase_label(x);
    }
  else
    {
      x->lab = s;
      x->lab_vis = 1;
      n_knob_erase_label(x);
      n_knob_draw_label(x);
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_labpar(t_n_knob* x, 
                   t_floatarg lab_fs, 
                   t_floatarg ldx,
                   t_floatarg ldy,
                   t_floatarg lcol)
{
  AF_CLIP_MINMAX(4, 256, lab_fs);
  x->lab_fs = lab_fs;
  x->ldx = ldx;
  x->ldy = ldy;
  x->lcol = pdcolor(lcol);
  if (x->lab_vis)
    {
      n_knob_erase_label(x);
      n_knob_draw_label(x);
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_numpar(t_n_knob* x, 
                   t_floatarg numw, 
                   t_floatarg num_fs, 
                   t_floatarg ndx,
                   t_floatarg ndy,
                   t_floatarg ncol)
{
  AF_CLIP_MINMAX(1, 8, numw);
  x->numw = numw;
  AF_CLIP_MINMAX(4, 256, num_fs);
  x->num_fs = num_fs;
  x->ndx = ndx;
  x->ndy = ndy;
  x->ncol = pdcolor(ncol);
  if (x->num_vis)
    {
      n_knob_erase_num(x);
      n_knob_draw_num(x);
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_numvis(t_n_knob* x, 
                   t_floatarg num_vis)
{
  x->num_vis = num_vis;
  if (x->num_vis)
    {
      n_knob_erase_num(x);
      n_knob_draw_num(x);
    }
  else
    {
      n_knob_erase_num(x);
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_set(t_n_knob* x, t_floatarg f)
{
  x->state = f;
  if (x->minmaxdir)
    {
      AF_CLIP_MINMAX(x->max, x->min, x->state);
    }
  else
    {
      AF_CLIP_MINMAX(x->min, x->max, x->state);
    }
  x->state = quantize(x->state, x->step);
  f = x->state - x->min;
  f = f / x->diff; 
  x->count = f * x->resolution;
  x->framepos = (x->count / x->resolution) * (x->frames - 1);
  if(glist_isvisible(x->x_glist) && gobj_shouldvis((t_gobj *)x, x->x_glist))
    {
      if (x->framepos != x->framepos_old)
        {
          n_knob_erase_image(x);
          n_knob_draw_image(x);
          n_knob_bind_image(x);
        }
      if (x->num_vis)
        {
          n_knob_erase_num(x);
          n_knob_draw_num(x);
        }
    }
  x->framepos_old = x->framepos;
}

// -------------------------------------------------------------------------- //
static void n_knob_init(t_n_knob* x, t_floatarg f)
{
  if (f > 0)  x->init = INIT;
  else        x->init = NOINIT;
}

// -------------------------------------------------------------------------- //
static void n_knob_bang(t_n_knob* x)
{
  n_knob_out(x);
}

// -------------------------------------------------------------------------- //
static void n_knob_float(t_n_knob* x, t_float f)
{
  x->state = f;
  if (x->minmaxdir)
    {
      AF_CLIP_MINMAX(x->max, x->min, x->state);
    }
  else
    {
      AF_CLIP_MINMAX(x->min, x->max, x->state);
    }
  x->state = quantize(x->state, x->step);
  f = x->state - x->min;
  f = f / x->diff; 
  x->count = f * x->resolution;
  x->framepos = (x->count / x->resolution) * (x->frames - 1);
  if(glist_isvisible(x->x_glist) && gobj_shouldvis((t_gobj *)x, x->x_glist))
    {
      if (x->framepos != x->framepos_old)
        {
          n_knob_erase_image(x);
          n_knob_draw_image(x);
          n_knob_bind_image(x);
        }
      if (x->num_vis)
        {
          n_knob_erase_num(x);
          n_knob_draw_num(x);
        }
    }
  x->framepos_old = x->framepos;
  n_knob_out(x);
}

// -------------------------------------------------------------------------- //
static void n_knob_default(t_n_knob* x)
{
  n_knob_float(x, x->default_state);
}

// -------------------------------------------------------------------------- //
static void n_knob_loadbang(t_n_knob *x, t_floatarg action)
{
  if(action == LB_LOAD && x->init)
    {
      n_knob_out(x);
    }
}

// -------------------------------------------------------------------------- //
// callback
// -------------------------------------------------------------------------- //
static void n_knob_size_callback(t_n_knob *x, t_float w, t_float h)
{
  if (w == -1)
    {
      error("n_knob: image loading error: %s",x->filename->s_name);
      return;
    }

  x->img_w = w;
  x->img_h = h;
  // only vertical
  x->frame_w = x->img_w;
  x->frame_h = x->img_h / x->frames;
  x->frame_w_2 = x->frame_w / 2;
  x->frame_h_2 = x->frame_h / 2;

  x->framepos = (x->count / x->resolution) * (x->frames - 1);
  if(glist_isvisible(x->x_glist) && gobj_shouldvis((t_gobj *)x, x->x_glist))
    {
      n_knob_erase_image(x);
      n_knob_draw_image(x);
      n_knob_bind_image(x);
      n_knob_erase_frame(x);
      if (x->sel)
        {
          n_knob_draw_frame(x);
        }
    }
}

// -------------------------------------------------------------------------- //
static void n_knob_double_callback(t_n_knob* x)
{
  if(!x->x_glist->gl_edit)
    {
      n_knob_float(x, x->default_state);
    }
}

// -------------------------------------------------------------------------- //
// save and properties
// -------------------------------------------------------------------------- //
static void n_knob_save(t_gobj *z, t_binbuf *b)
{
  t_n_knob *x = (t_n_knob *)z;
  char buf[128];
  t_symbol *snd;
  t_symbol *rcv;
  t_symbol *lab;
  t_symbol *lcol;
  t_symbol *ncol;
  sprintf(buf, "%s", x->snd->s_name); dollarinstring(buf); snd  = gensym(buf); 
  sprintf(buf, "%s", x->rcv->s_name); dollarinstring(buf); rcv  = gensym(buf); 
  sprintf(buf, "%s", x->lab->s_name); dollarinstring(buf); lab  = gensym(buf); 
  sprintf(buf, "%d", x->lcol);                             lcol = gensym(buf);
  sprintf(buf, "%d", x->ncol);                             ncol = gensym(buf);

  binbuf_addv(b, "ssiissiiffffisssiiisfiiiiiis", 
              gensym("#X"), 
              gensym("obj"),
              (int)x->x_obj.te_xpix, 
              (int)x->x_obj.te_ypix,   
              gensym("n_knob"),
              x->filename,
              (int)x->orientation,
              (int)x->frames,
              x->min,
              x->max,
              x->step,
              x->default_state,
              (int)x->resolution,
              snd,
              rcv,
              lab,
              (int)x->lab_fs,
              (int)x->ldx,
              (int)x->ldy,
              lcol,
              x->state,
              (int)x->init,
              (int)x->num_vis,
              (int)x->numw,
              (int)x->num_fs,
              (int)x->ndx,
              (int)x->ndy,
              ncol);
  binbuf_addv(b, ";");
}

//----------------------------------------------------------------------------//
static void n_knob_properties(t_gobj *z, t_glist *owner)
{
  t_n_knob *x = (t_n_knob *)z;
  char buf[1024];
  t_symbol *snd;
  t_symbol *rcv;
  t_symbol *lab;
  sprintf(buf, "%s", x->snd->s_name); dollarinstring(buf); snd  = gensym(buf); 
  sprintf(buf, "%s", x->rcv->s_name); dollarinstring(buf); rcv  = gensym(buf); 
  sprintf(buf, "%s", x->lab->s_name); dollarinstring(buf); lab  = gensym(buf); 
  
  sprintf(buf, "pdtk_n_knob_dialog %%s %s %d %d %g %g %g %g %d %s %s %s %d %d %d %d %d %d %d %d %d %d %d\n",
          x->filename->s_name,
          (int)x->orientation,
          (int)x->frames,
          x->min,
          x->max,
          x->step,
          x->default_state,
          (int)x->resolution,
          snd->s_name,
          rcv->s_name,
          lab->s_name,
          (int)x->lab_fs,
          (int)x->ldx,
          (int)x->ldy,
          x->lcol,
          (int)x->init,
          (int)x->num_vis,
          (int)x->numw,
          (int)x->num_fs,
          (int)x->ndx,
          (int)x->ndy,
          x->ncol);
  gfxstub_new(&x->x_obj.ob_pd, x, buf);
  if (owner) {};
}

//----------------------------------------------------------------------------//
static void n_knob_dialog(t_n_knob *x, t_symbol *s, int ac, t_atom *av)
{
  if (!x)
    {
      error("n_knob: dialog error:  unexisting object");
      return;
    }
  if (ac != 22)
    {
      error("n_knob: dialog error: number arguments");
      return;
    }
  t_symbol *filename    = atom_getsymbolarg(0,ac,av);
  int orientation       = atom_getfloatarg(1,ac,av);
  int frames            = atom_getfloatarg(2,ac,av);
  t_float min           = atom_getfloatarg(3,ac,av);
  t_float max           = atom_getfloatarg(4,ac,av);
  t_float step          = atom_getfloatarg(5,ac,av);
  t_float default_state = atom_getfloatarg(6,ac,av);
  int resolution        = atom_getfloatarg(7,ac,av);
  t_symbol *snd         = mygetsymbolarg(8,ac,av);
  t_symbol *rcv         = mygetsymbolarg(9,ac,av);
  t_symbol *lab         = mygetsymbolarg(10,ac,av);
  int lab_fs            = atom_getfloatarg(11,ac,av);
  int ldx               = atom_getfloatarg(12,ac,av);
  int ldy               = atom_getfloatarg(13,ac,av);
  int lcol              = mygetintarg(14,ac,av);
  int init              = atom_getfloatarg(15,ac,av);
  int num_vis           = atom_getfloatarg(16,ac,av);
  int numw              = atom_getfloatarg(17,ac,av);
  int num_fs            = atom_getfloatarg(18,ac,av);
  int ndx               = atom_getfloatarg(19,ac,av);
  int ndy               = atom_getfloatarg(20,ac,av);
  int ncol              = mygetintarg(21,ac,av);

  if ((strcmp(x->filename->s_name, filename->s_name))
      || x->orientation != orientation
      || x->frames != frames)
    {
      n_knob_loadimage(x, filename, orientation, frames);
    }

  if (x->min != min
      || x->max != max
      || x->step != step
      || x->default_state != default_state
      || x->resolution != resolution)
    {
      n_knob_knobpar(x, min, max, step, default_state, resolution);
    }

  if ((strcmp(x->snd->s_name, snd->s_name)))
    {
      n_knob_snd(x, snd);
    }
    
  if ((strcmp(x->rcv->s_name, rcv->s_name)))
    {
      n_knob_rcv(x, rcv);
    }
    
  if ((strcmp(x->lab->s_name, lab->s_name)))
    {
      n_knob_lab(x, lab);
    }

  if (x->lab_fs != lab_fs
      || x->ldx != ldx
      || x->ldy != ldy
      || x->lcol != lcol)
    {
      AF_CLIP_MINMAX(4, 256, lab_fs);
      x->lab_fs = lab_fs;
      x->ldx = ldx;
      x->ldy = ldy;
      x->lcol = lcol;
      if (x->lab_vis)
        {
          n_knob_erase_label(x);
          n_knob_draw_label(x);
        }
    }

  if (x->init != init)
    {
      n_knob_init(x, init);
    }

  if (x->numw != numw
      || x->num_fs != num_fs
      || x->ndx != ndx
      || x->ndy != ndy
      || x->ncol != ncol)
    {
      AF_CLIP_MINMAX(1, 8, numw);
      x->numw = numw;
      AF_CLIP_MINMAX(4, 256, num_fs);
      x->num_fs = num_fs;
      x->ndx = ndx;
      x->ndy = ndy;
      x->ncol = ncol;
      if (x->num_vis)
        {
          n_knob_erase_num(x);
          n_knob_draw_num(x);
        }
    }

  if (x->num_vis != num_vis)
    {
      n_knob_numvis(x, num_vis);
    }

  if (s) {} // disabled
}

// -------------------------------------------------------------------------- //
// setup
// -------------------------------------------------------------------------- //
static void *n_knob_new(t_symbol *s, t_int ac, t_atom *av)
{
  t_n_knob *x = (t_n_knob *)pd_new(n_knob_class);
  x->x_glist = (t_glist*) canvas_getcurrent();
  outlet_new(&x->x_obj, &s_float);
  char buf[MAXPDSTRING];
  
  // $0
  x->dollarzero = canvas_getdollarzero();
  x->curdir = canvas_getcurrentdir();
  
  // bind
  sprintf(buf, "#%lx", (long)x);
  pd_bind(&x->x_obj.ob_pd, x->x_bindname = gensym(buf));

  // image and frame
  x->img_w = -1;
  x->img_h = -1;
  x->sel = 0;
  x->state_old = NEVERST;
  x->framepos_old = NEVERST;
  
  t_symbol *filename;
  int orientation;
  int frames;
  t_float min;
  t_float max;
  t_float step;
  t_float default_state;
  int resolution;
  t_symbol *snd;
  t_symbol *rcv;
  t_symbol *lab;
  int lab_fs;
  int ldx;
  int ldy;
  int lcol;
  t_float state;
  int init;
  int num_vis;
  int numw;
  int num_fs;
  int ndx;
  int ndy;
  int ncol;

  
  // arguments
  if (ac == 23
      && IS_A_SYMBOL(av,0)
      && IS_A_FLOAT(av,1)
      && IS_A_FLOAT(av,2)
      && IS_A_FLOAT(av,3)
      && IS_A_FLOAT(av,4)
      && IS_A_FLOAT(av,5)
      && IS_A_FLOAT(av,6)
      && IS_A_FLOAT(av,7)
      && (IS_A_FLOAT(av, 8)  || IS_A_SYMBOL(av, 8))
      && (IS_A_FLOAT(av, 9)  || IS_A_SYMBOL(av, 9))
      && (IS_A_FLOAT(av, 10) || IS_A_SYMBOL(av, 10))
      && IS_A_FLOAT(av,11)
      && IS_A_FLOAT(av,12)
      && IS_A_FLOAT(av,13)
      && (IS_A_FLOAT(av, 14) || IS_A_SYMBOL(av, 14))
      && IS_A_FLOAT(av,15)
      && IS_A_FLOAT(av,16)
      && IS_A_FLOAT(av,17)
      && IS_A_FLOAT(av,18)
      && IS_A_FLOAT(av,19)
      && IS_A_FLOAT(av,20)
      && IS_A_FLOAT(av,21)
      && (IS_A_FLOAT(av, 22) || IS_A_SYMBOL(av, 22)))
    {
      filename      = atom_getsymbolarg(0,ac,av);
      orientation   = atom_getfloatarg(1,ac,av);
      frames        = atom_getfloatarg(2,ac,av);
      min           = atom_getfloatarg(3,ac,av);
      max           = atom_getfloatarg(4,ac,av);
      step          = atom_getfloatarg(5,ac,av);
      default_state = atom_getfloatarg(6,ac,av);
      resolution    = atom_getfloatarg(7,ac,av);
      snd           = mygetsymbolarg(8,ac,av);
      rcv           = mygetsymbolarg(9,ac,av);
      lab           = mygetsymbolarg(10,ac,av);
      lab_fs        = atom_getfloatarg(11,ac,av);
      ldx           = atom_getfloatarg(12,ac,av);
      ldy           = atom_getfloatarg(13,ac,av);
      lcol          = mygetintarg(14,ac,av);
      state         = atom_getfloatarg(15,ac,av);
      init          = atom_getfloatarg(16,ac,av);
      num_vis       = atom_getfloatarg(17,ac,av);
      numw          = atom_getfloatarg(18,ac,av);
      num_fs        = atom_getfloatarg(19,ac,av);
      ndx           = atom_getfloatarg(20,ac,av);
      ndy           = atom_getfloatarg(21,ac,av);
      ncol          = mygetintarg(22,ac,av);
    }
  else
    {
      filename      = gensym("default.gif");
      orientation   = VER;
      frames        = 33;
      min           = 0.0;
      max           = 1.0;
      step          = 0.01;
      default_state = 0.5;
      resolution    = 256;
      snd           = gensym("empty");
      rcv           = gensym("empty");
      lab           = gensym("empty");
      lab_fs        = 11;
      ldx           = 0;
      ldy           = -8;
      lcol          = pdcolor(22);
      state         = 0.0;
      init          = NOINIT;
      num_vis       = 0;
      numw          = 5;
      num_fs        = 11;
      ndx           = 0;
      ndy           = 42;
      ncol          = pdcolor(22);
    }
  
  n_knob_knobpar(x, min, max, step, default_state, resolution);

  if (!(strcmp(lab->s_name, "empty")))
    {
      x->lab = lab;
      x->lab_vis = 0;
    }
  else
    {
      x->lab = lab;
      x->lab_vis = 1;
    }
  AF_CLIP_MINMAX(4, 256, lab_fs);
  x->lab_fs = lab_fs;
  x->ldx = ldx;
  x->ldy = ldy;
  x->lcol = lcol;

  x->num_vis = num_vis;
  AF_CLIP_MINMAX(1, 8, numw);
  x->numw = numw;
  AF_CLIP_MINMAX(4, 256, num_fs);
  x->num_fs = num_fs;
  x->ndx = ndx;
  x->ndy = ndy;
  x->ncol = ncol;

  n_knob_set(x, state);
  n_knob_init(x, init);
  n_knob_loadimage(x, filename, orientation, frames);

  x->snd_able = 0;
  x->rcv_able = 0;
  n_knob_snd(x, snd);
  n_knob_rcv(x, rcv);

  return (x);
  if (s) {};
}


// -------------------------------------------------------------------------- //
static void n_knob_free(t_n_knob* x)
{
  pd_unbind(&x->x_obj.ob_pd, x->x_bindname);
  if (x->rcv_able)
    {
      pd_unbind(&x->x_obj.ob_pd, x->rcv_real);
    }
}

// -------------------------------------------------------------------------- //
void n_knob_setup(void)
{
  n_knob_class = class_new(gensym("n_knob"),(t_newmethod)n_knob_new,
                           (t_method)n_knob_free,sizeof(t_n_knob),0,A_GIMME,0);
  class_addmethod(n_knob_class,(t_method)n_knob_loadimage,gensym("loadimage"),
                  A_SYMBOL,A_FLOAT,A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_knobpar,gensym("knobpar"),
                  A_FLOAT,A_FLOAT,A_FLOAT,A_FLOAT,A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_snd,gensym("send"),
                  A_SYMBOL,0);
  class_addmethod(n_knob_class,(t_method)n_knob_rcv,gensym("receive"),
                  A_SYMBOL,0);
  class_addmethod(n_knob_class,(t_method)n_knob_lab,gensym("label"),
                  A_SYMBOL,0);
  class_addmethod(n_knob_class,(t_method)n_knob_labpar,gensym("labpar"),
                  A_FLOAT,A_FLOAT,A_FLOAT,A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_numpar,gensym("numpar"),
                  A_FLOAT,A_FLOAT,A_FLOAT,A_FLOAT,A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_numvis,gensym("numvis"),
                  A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_set,gensym("set"),
                  A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_init,gensym("init"),
                  A_FLOAT,0);
  class_addbang(n_knob_class, (t_method)n_knob_bang);
  class_addfloat(n_knob_class, (t_method)n_knob_float);
  class_addmethod(n_knob_class,(t_method)n_knob_default,gensym("default"),0);
  class_addmethod(n_knob_class,(t_method)n_knob_size_callback, gensym("_size"),
                  A_FLOAT,A_FLOAT,0);
  class_addmethod(n_knob_class,(t_method)n_knob_double_callback,gensym("_double"),
                  0);
  class_addmethod(n_knob_class, (t_method)n_knob_loadbang, gensym("loadbang"),
                  A_DEFFLOAT, 0);
  class_addmethod(n_knob_class, (t_method)n_knob_dialog, gensym("dialog"),
                  A_GIMME, 0);
  n_knob_widgetbehavior.w_getrectfn  = n_knob_getrect;
  n_knob_widgetbehavior.w_displacefn = n_knob_displace;
  n_knob_widgetbehavior.w_selectfn   = n_knob_select;
  n_knob_widgetbehavior.w_deletefn   = n_knob_delete;
  n_knob_widgetbehavior.w_visfn      = n_knob_vis;
  n_knob_widgetbehavior.w_clickfn    = n_knob_newclick;
  class_setsavefn(n_knob_class,&n_knob_save);
  class_setwidget(n_knob_class,&n_knob_widgetbehavior);
  class_setpropertiesfn(n_knob_class, n_knob_properties);
  sys_vgui("eval [read [open {%s/n_knob.tcl}]]\n",
           n_knob_class->c_externdir->s_name);
  extdir = n_knob_class->c_externdir;
}

