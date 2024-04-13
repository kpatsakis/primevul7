static inline void GetMinimumPixelList(PixelList *pixel_list,Quantum *pixel)
{
  register SkipList
    *p;

  size_t
    color,
    minimum;

  ssize_t
    count;

  /*
    Find the minimum value for each of the color.
  */
  p=(&pixel_list->skip_list);
  count=0;
  color=65536UL;
  minimum=p->nodes[color].next[0];
  do
  {
    color=p->nodes[color].next[0];
    if (color < minimum)
      minimum=color;
    count+=p->nodes[color].count;
  } while (count < (ssize_t) pixel_list->length);
  *pixel=ScaleShortToQuantum((unsigned short) minimum);
}