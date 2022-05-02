#include "string.h"
#include "m_pd.h"

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
