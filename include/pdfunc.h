#include <string.h>
#include <stdlib.h>
#include "m_pd.h"
#include "g_all_guis.h"

//----------------------------------------------------------------------------//
t_float mygetfloatarg(int n, int ac, t_atom *av)
{
  t_symbol *buf;
  if (IS_A_FLOAT(av, n))
	{
      return (atom_getfloatarg(n, ac, av));
	}
  else
	{
      buf = (t_symbol *)atom_getsymbolarg(n, ac, av);
      return (atof(buf->s_name));
	}
}

//----------------------------------------------------------------------------//
t_int mygetintarg(int n, int ac, t_atom *av)
{
  t_symbol *buf;
  if (IS_A_FLOAT(av, n))
	{
      return ((int)atom_getfloatarg(n, ac, av));
	}
  else
	{
      buf = (t_symbol *)atom_getsymbolarg(n, ac, av);
      return (atoi(buf->s_name));
	}
}

//----------------------------------------------------------------------------//
t_symbol *mygetsymbolarg(int n, int ac, t_atom *av)
{
  char buf[20];
  if (IS_A_FLOAT(av, n))
	{
      sprintf(buf, "%d", (int)atom_getfloatarg(n, ac, av));
      return ((t_symbol *)gensym(buf));
	}
  else
	{
      return ((t_symbol *)atom_getsymbolarg(n, ac, av));
	}
}

//----------------------------------------------------------------------------//
void dollarinstring(char *str)
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
t_symbol *dollarzero2sym(t_symbol *s, int id)
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
  return (gensym(buf));
}

//----------------------------------------------------------------------------//
int pdcolor30[] =
  {
    16579836, 10526880, 4210752, 16572640, 16572608,
    16579784, 14220504, 14220540, 14476540, 16308476,
    14737632, 8158332, 2105376, 16525352, 16559172,
    15263784, 1370132, 2684148, 3952892, 16003312,
    12369084, 6316128, 0, 9177096, 5779456,
    7874580, 2641940, 17488, 5256, 5767248
  };

//----------------------------------------------------------------------------//
int pdcolor(int col)
{
  if (col >= 0)
    {
      col = col % 30;
      col = pdcolor30[col];
    }
  else
    {
      col = (0 - col) & 0x00ffffff;
    }
  return (col);
}
