onig_region_resize(OnigRegion* region, int n)
{
  region->num_regs = n;

  if (n < ONIG_NREGION)
    n = ONIG_NREGION;

  if (region->allocated == 0) {
    region->beg = (OnigPosition* )xmalloc(n * sizeof(OnigPosition));
    if (region->beg == 0)
      return ONIGERR_MEMORY;

    region->end = (OnigPosition* )xmalloc(n * sizeof(OnigPosition));
    if (region->end == 0) {
      xfree(region->beg);
      return ONIGERR_MEMORY;
    }

    region->allocated = n;
  }
  else if (region->allocated < n) {
    OnigPosition *tmp;

    region->allocated = 0;
    tmp = (OnigPosition* )xrealloc(region->beg, n * sizeof(OnigPosition));
    if (tmp == 0) {
      xfree(region->beg);
      xfree(region->end);
      return ONIGERR_MEMORY;
    }
    region->beg = tmp;
    tmp = (OnigPosition* )xrealloc(region->end, n * sizeof(OnigPosition));
    if (tmp == 0) {
      xfree(region->beg);
      xfree(region->end);
      return ONIGERR_MEMORY;
    }
    region->end = tmp;

    region->allocated = n;
  }

  return 0;
}