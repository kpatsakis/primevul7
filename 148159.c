static void GenerateCLAHEHistogram(const RectangleInfo *clahe_info,
  const RectangleInfo *tile_info,const size_t number_bins,
  const unsigned short *lut,const unsigned short *pixels,size_t *histogram)
{
  register const unsigned short
    *p;

  register ssize_t
    i;

  /*
    Classify the pixels into a gray histogram.
  */
  for (i=0; i < (ssize_t) number_bins; i++)
    histogram[i]=0L;
  p=pixels;
  for (i=0; i < (ssize_t) tile_info->height; i++)
  {
    const unsigned short
      *q;

    q=p+tile_info->width;
    while (p < q)
      histogram[lut[*p++]]++;
    q+=clahe_info->width;
    p=q-tile_info->width;
  }
}