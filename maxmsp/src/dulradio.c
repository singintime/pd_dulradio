#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext.h"
#include "ext_obex.h"

static t_class *dulradio_class;

typedef struct _dulradio {
  t_object  x_obj;
  char buf[256], *toparse;
  int len, argc;
  t_atom argv[16];
  void *out;
} t_dulradio;

void dulradio_reset(t_dulradio *x)
{
  memset(x->buf, 0, x->len);
  x->toparse = &x->buf[0];
  x->len = 0;
  x->argc = 0;
}

void dulradio_addlong(t_dulradio *x, long l)
{
  x->argv[x->argc].a_type = A_LONG;
  x->argv[x->argc].a_w.w_long = l;
  x->argc++;
}

void dulradio_addsymbol(t_dulradio *x, t_symbol *s)
{
  x->argv[x->argc].a_type = A_SYM;
  x->argv[x->argc].a_w.w_sym = s;
  x->argc++;
}

int dulradio_consume(t_dulradio *x, const char *token)
{
  char *match = strstr(x->toparse, token);
  if (match == x->toparse) {
    x->toparse += strlen(token);
    return 1;
  }
  else return 0;
}

void dulradio_parse(t_dulradio *x)
{
  int i;
  for (i = 1; i <= 4; i++) {
    char s[2];
    sprintf(s, "%d", i);
    if (dulradio_consume(x, s)) {
      dulradio_addlong(x, (long)i);
      break;
    }
  }
  int n_words;
  if (x->argc != 1) return;
  if (dulradio_consume(x, ":ADC ")) {
    dulradio_addsymbol(x, gensym("ADC"));
    n_words = 2;
  }
  else if (dulradio_consume(x, ":Acc ")) {
    dulradio_addsymbol(x, gensym("Acc"));
    n_words = 3;
  }
  else return;
  int16_t value;
  if (strcspn(x->toparse, "\a") <= 2 * n_words) {
    for (i = 0; i < n_words; i++) {
      memcpy(&value, x->toparse, 2);
      dulradio_addlong(x, (long)value);
      x->toparse += 2;
    }
  }
  else if (dulradio_consume(x, "(")) {
    for (i = 0; i < n_words; i++) {
      char sep = i == n_words - 1 ? ')' : ',';
      value = (int16_t)strtol(x->toparse, &x->toparse, 16);
      if (x->toparse[0] != sep) return;
      dulradio_addlong(x, (long)value);
      x->toparse++;
    }
  }
  else return;
  while (x->toparse[0] == '\a') {
    x->toparse += 1;
    size_t offset = strcspn(x->toparse, "\a");
    char token[offset+1];
    strncpy(token, x->toparse, offset);
    token[offset] = '\0';
    dulradio_addsymbol(x, gensym(token));
    x->toparse += offset;
  }
  outlet_list(x->out, NULL, x->argc, &x->argv[0]);
}

void dulradio_append(t_dulradio *x, long l)
{
  if (x->len > 255) dulradio_reset(x);
  char c = (char)l;
  if ((c == '\n' || c == '\r') && (x->len >= 12 || (strstr(x->buf, ":ADC ") && x->len == 10))) {
    dulradio_parse(x);
    dulradio_reset(x);
    return;
  }
  x->buf[x->len] = c;
  x->len++;
}

void *dulradio_new(void)
{
  t_dulradio *x = (t_dulradio *)object_alloc(dulradio_class);
  x->out = outlet_new(&x->x_obj, "list");
  dulradio_reset(x);
  return x;
}

int main(void) {
  t_class *c;
  c = class_new("dulradio", (method)dulradio_new, (method)NULL, sizeof(t_dulradio), 0L, 0);
  class_addmethod(c, (method)dulradio_reset, "bang", 0);
  class_addmethod(c, (method)dulradio_append, "int", A_LONG, 0);
  class_register(CLASS_BOX, c);
  dulradio_class = c;
  return 0;
}
