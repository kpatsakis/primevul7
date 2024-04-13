find_all_by_keyword(smartlist_t *s, directory_keyword k)
{
  smartlist_t *out = NULL;
  SMARTLIST_FOREACH(s, directory_token_t *, t,
                    if (t->tp == k) {
                      if (!out)
                        out = smartlist_new();
                      smartlist_add(out, t);
                    });
  return out;
}