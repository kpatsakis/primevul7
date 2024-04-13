aspath_filter_exclude (struct aspath * source, struct aspath * exclude_list)
{
  struct assegment * srcseg, * exclseg, * lastseg;
  struct aspath * newpath;

  newpath = aspath_new();
  lastseg = NULL;

  for (srcseg = source->segments; srcseg; srcseg = srcseg->next)
  {
    unsigned i, y, newlen = 0, done = 0, skip_as;
    struct assegment * newseg;

    /* Find out, how much ASns are we going to pick from this segment.
     * We can't perform filtering right inline, because the size of
     * the new segment isn't known at the moment yet.
     */
    for (i = 0; i < srcseg->length; i++)
    {
      skip_as = 0;
      for (exclseg = exclude_list->segments; exclseg && !skip_as; exclseg = exclseg->next)
        for (y = 0; y < exclseg->length; y++)
          if (srcseg->as[i] == exclseg->as[y])
          {
            skip_as = 1;
            // There's no sense in testing the rest of exclusion list, bail out.
            break;
          }
      if (!skip_as)
        newlen++;
    }
    /* newlen is now the number of ASns to copy */
    if (!newlen)
      continue;

    /* Actual copying. Allocate memory and iterate once more, performing filtering. */
    newseg = assegment_new (srcseg->type, newlen);
    for (i = 0; i < srcseg->length; i++)
    {
      skip_as = 0;
      for (exclseg = exclude_list->segments; exclseg && !skip_as; exclseg = exclseg->next)
        for (y = 0; y < exclseg->length; y++)
          if (srcseg->as[i] == exclseg->as[y])
          {
            skip_as = 1;
            break;
          }
      if (skip_as)
        continue;
      newseg->as[done++] = srcseg->as[i];
    }
    /* At his point newlen must be equal to done, and both must be positive. Append
     * the filtered segment to the gross result. */
    if (!lastseg)
      newpath->segments = newseg;
    else
      lastseg->next = newseg;
    lastseg = newseg;
  }
  aspath_str_update (newpath);
  /* We are happy returning even an empty AS_PATH, because the administrator
   * might expect this very behaviour. There's a mean to avoid this, if necessary,
   * by having a match rule against certain AS_PATH regexps in the route-map index.
   */
  aspath_free (source);
  return newpath;
}