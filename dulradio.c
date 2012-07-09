#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "m_pd.h"

static t_class *dulradio_class;

typedef struct _dulradio {
  t_object  x_obj;
  char buf[1024];
  t_outlet *out;
} t_dulradio;

void dulradio_reset(t_dulradio *x)
{
  strcpy(x->buf,"");
}

void dulradio_parse(t_dulradio *x)
{
  char *token;
  char *end_ptr;
  t_atom argv[100];
  int argc = 0;
  token = strtok(x->buf, ":");
  if (token == NULL) return;
  argv[argc].a_type = A_FLOAT;
  argv[argc].a_w.w_float = (t_float)atoi(token);
  argc++;
  token = strtok(NULL, " ");
  if (token == NULL) return;
  argv[argc].a_type = A_SYMBOL;
  argv[argc].a_w.w_symbol = gensym(token);
  argc++;
  while (1)
  {
    token = strtok(NULL, " (,)");
    if (token == NULL) break;
    t_float f = (t_float)strtol(token, &end_ptr, 16);
    if (strcmp(end_ptr,"") == 0) {
      argv[argc].a_type = A_FLOAT;
      argv[argc].a_w.w_float = f;
      argc++;
    }
    else {
      argv[argc].a_type = A_SYMBOL;
      argv[argc].a_w.w_symbol = gensym(token);
      argc++;
    }
  }
  outlet_list(x->out, &s_list, argc, &argv[0]);
}

void dulradio_append(t_dulradio *x, t_floatarg f)
{
  char c = (char)f;
  if (c == '\n') {
    dulradio_parse(x);
    strcpy(x->buf,"");
    return;
  }
  strcat(x->buf, &c);
}

void *dulradio_new(void)
{
  t_dulradio *x = (t_dulradio *)pd_new(dulradio_class);
  dulradio_reset(x);
  x->out = outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void dulradio_setup(void) {
  dulradio_class = class_new(gensym("dulradio"),
        (t_newmethod)dulradio_new,
        0, sizeof(t_dulradio),
        CLASS_DEFAULT, 0);

  class_addbang  (dulradio_class, dulradio_reset);
  class_doaddfloat(dulradio_class, (t_method)dulradio_append);
}
