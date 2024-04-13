static inline void GetMeanPixelList(PixelList *pixel_list,Quantum *pixel)
{
  double
    sum;

  register SkipList
    *p;

  size_t
    color;

  ssize_t
    count;

  /*
    Find the mean value for each of the color.
  */
  p=(&pixel_list->skip_list);
  color=65536L;
  count=0;
  sum=0.0;
  do
  {
    color=p->nodes[color].next[0];
    sum+=(double) p->nodes[color].count*color;
    count+=p->nodes[color].count;
  } while (count < (ssize_t) pixel_list->length);
  sum/=pixel_list->length;
  *pixel=ScaleShortToQuantum((unsigned short) sum);
}