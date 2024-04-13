static void ClipCLAHEHistogram(const double clip_limit,const size_t number_bins,
  size_t *histogram)
{
#define NumberCLAHEGrays  (65536)

  register ssize_t
    i;

  size_t
    cumulative_excess,
    previous_excess,
    step;

  ssize_t
    excess;

  /*
    Compute total number of excess pixels.
  */
  cumulative_excess=0;
  for (i=0; i < (ssize_t) number_bins; i++)
  {
    excess=(ssize_t) histogram[i]-(ssize_t) clip_limit;
    if (excess > 0)
      cumulative_excess+=excess;
  }
  /*
    Clip histogram and redistribute excess pixels across all bins.
  */
  step=cumulative_excess/number_bins;
  excess=(ssize_t) (clip_limit-step);
  for (i=0; i < (ssize_t) number_bins; i++)
  {
    if ((double) histogram[i] > clip_limit)
      histogram[i]=(size_t) clip_limit;
    else
      if ((ssize_t) histogram[i] > excess)
        {
          cumulative_excess-=histogram[i]-excess;
          histogram[i]=(size_t) clip_limit;
        }
      else
        {
          cumulative_excess-=step;
          histogram[i]+=step;
        }
  }
  /*
    Redistribute remaining excess.
  */
  do
  {
    register size_t
      *p;

    size_t
      *q;

    previous_excess=cumulative_excess;
    p=histogram;
    q=histogram+number_bins;
    while ((cumulative_excess != 0) && (p < q))
    {
      step=number_bins/cumulative_excess;
      if (step < 1)
        step=1;
      for (p=histogram; (p < q) && (cumulative_excess != 0); p+=step)
        if ((double) *p < clip_limit)
          {
            (*p)++;
            cumulative_excess--;
          }
      p++;
    }
  } while ((cumulative_excess != 0) && (cumulative_excess < previous_excess));
}