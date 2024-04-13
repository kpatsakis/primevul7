static inline void GetStandardDeviationPixelList(PixelList *pixel_list,
  Quantum *pixel)
{
  double
    sum,
    sum_squared;

  register SkipList
    *p;

  size_t
    color;

  ssize_t
    count;

  /*
    Find the standard-deviation value for each of the color.
  */
  p=(&pixel_list->skip_list);
  color=65536L;
  count=0;
  sum=0.0;
  sum_squared=0.0;
  do
  {
    register ssize_t
      i;

    color=p->nodes[color].next[0];
    sum+=(double) p->nodes[color].count*color;
    for (i=0; i < (ssize_t) p->nodes[color].count; i++)
      sum_squared+=((double) color)*((double) color);
    count+=p->nodes[color].count;
  } while (count < (ssize_t) pixel_list->length);
  sum/=pixel_list->length;
  sum_squared/=pixel_list->length;
  *pixel=ScaleShortToQuantum((unsigned short) sqrt(sum_squared-(sum*sum)));
}