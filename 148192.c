MagickExport MagickBooleanType EqualizeImage(Image *image,
  ExceptionInfo *exception)
{
#define EqualizeImageTag  "Equalize/Image"

  CacheView
    *image_view;

  double
    black[CompositePixelChannel+1],
    *equalize_map,
    *histogram,
    *map,
    white[CompositePixelChannel+1];

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  register ssize_t
    i;

  ssize_t
    y;

  /*
    Allocate and initialize histogram arrays.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  if (AccelerateEqualizeImage(image,exception) != MagickFalse)
    return(MagickTrue);
#endif
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  equalize_map=(double *) AcquireQuantumMemory(MaxMap+1UL,MaxPixelChannels*
    sizeof(*equalize_map));
  histogram=(double *) AcquireQuantumMemory(MaxMap+1UL,MaxPixelChannels*
    sizeof(*histogram));
  map=(double *) AcquireQuantumMemory(MaxMap+1UL,MaxPixelChannels*sizeof(*map));
  if ((equalize_map == (double *) NULL) || (histogram == (double *) NULL) ||
      (map == (double *) NULL))
    {
      if (map != (double *) NULL)
        map=(double *) RelinquishMagickMemory(map);
      if (histogram != (double *) NULL)
        histogram=(double *) RelinquishMagickMemory(histogram);
      if (equalize_map != (double *) NULL)
        equalize_map=(double *) RelinquishMagickMemory(equalize_map);
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        image->filename);
    }
  /*
    Form histogram.
  */
  status=MagickTrue;
  (void) memset(histogram,0,(MaxMap+1)*GetPixelChannels(image)*
    sizeof(*histogram));
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        double
          intensity;

        intensity=(double) p[i];
        if ((image->channel_mask & SyncChannels) != 0)
          intensity=GetPixelIntensity(image,p);
        histogram[GetPixelChannels(image)*ScaleQuantumToMap(
          ClampToQuantum(intensity))+i]++;
      }
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  /*
    Integrate the histogram to get the equalization map.
  */
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    double
      intensity;

    register ssize_t
      j;

    intensity=0.0;
    for (j=0; j <= (ssize_t) MaxMap; j++)
    {
      intensity+=histogram[GetPixelChannels(image)*j+i];
      map[GetPixelChannels(image)*j+i]=intensity;
    }
  }
  (void) memset(equalize_map,0,(MaxMap+1)*GetPixelChannels(image)*
    sizeof(*equalize_map));
  (void) memset(black,0,sizeof(*black));
  (void) memset(white,0,sizeof(*white));
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    register ssize_t
      j;

    black[i]=map[i];
    white[i]=map[GetPixelChannels(image)*MaxMap+i];
    if (black[i] != white[i])
      for (j=0; j <= (ssize_t) MaxMap; j++)
        equalize_map[GetPixelChannels(image)*j+i]=(double)
          ScaleMapToQuantum((double) ((MaxMap*(map[
          GetPixelChannels(image)*j+i]-black[i]))/(white[i]-black[i])));
  }
  histogram=(double *) RelinquishMagickMemory(histogram);
  map=(double *) RelinquishMagickMemory(map);
  if (image->storage_class == PseudoClass)
    {
      register ssize_t
        j;

      /*
        Equalize colormap.
      */
      for (j=0; j < (ssize_t) image->colors; j++)
      {
        if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
          {
            PixelChannel channel = GetPixelChannelChannel(image,
              RedPixelChannel);
            if (black[channel] != white[channel])
              image->colormap[j].red=equalize_map[GetPixelChannels(image)*
                ScaleQuantumToMap(ClampToQuantum(image->colormap[j].red))+
                channel];
          }
        if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
          {
            PixelChannel channel = GetPixelChannelChannel(image,
              GreenPixelChannel);
            if (black[channel] != white[channel])
              image->colormap[j].green=equalize_map[GetPixelChannels(image)*
                ScaleQuantumToMap(ClampToQuantum(image->colormap[j].green))+
                channel];
          }
        if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
          {
            PixelChannel channel = GetPixelChannelChannel(image,
              BluePixelChannel);
            if (black[channel] != white[channel])
              image->colormap[j].blue=equalize_map[GetPixelChannels(image)*
                ScaleQuantumToMap(ClampToQuantum(image->colormap[j].blue))+
                channel];
          }
        if ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0)
          {
            PixelChannel channel = GetPixelChannelChannel(image,
              AlphaPixelChannel);
            if (black[channel] != white[channel])
              image->colormap[j].alpha=equalize_map[GetPixelChannels(image)*
                ScaleQuantumToMap(ClampToQuantum(image->colormap[j].alpha))+
                channel];
          }
      }
    }
  /*
    Equalize image.
  */
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        j;

      for (j=0; j < (ssize_t) GetPixelChannels(image); j++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,j);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (((traits & UpdatePixelTrait) == 0) || (black[j] == white[j]))
          continue;
        q[j]=ClampToQuantum(equalize_map[GetPixelChannels(image)*
          ScaleQuantumToMap(q[j])+j]);
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,EqualizeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  equalize_map=(double *) RelinquishMagickMemory(equalize_map);
  return(status);
}