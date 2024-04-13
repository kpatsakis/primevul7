static void MapCLAHEHistogram(const RangeInfo *range_info,
  const size_t number_bins,const size_t number_pixels,size_t *histogram)
{
  double
    scale,
    sum;

  register ssize_t
    i;

  /*
    Rescale histogram to range [min-intensity .. max-intensity].
  */
  scale=(double) (range_info->max-range_info->min)/number_pixels;
  sum=0.0;
  for (i=0; i < (ssize_t) number_bins; i++)
  {
    sum+=histogram[i];
    histogram[i]=(size_t) (range_info->min+scale*sum);
    if (histogram[i] > range_info->max)
      histogram[i]=range_info->max;
  }
}