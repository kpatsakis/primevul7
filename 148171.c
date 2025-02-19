MagickExport MagickBooleanType ContrastStretchImage(Image *image,
  const double black_point,const double white_point,ExceptionInfo *exception)
{
#define MaxRange(color)  ((double) ScaleQuantumToMap((Quantum) (color)))
#define ContrastStretchImageTag  "ContrastStretch/Image"

  CacheView
    *image_view;

  double
    *black,
    *histogram,
    *stretch_map,
    *white;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  register ssize_t
    i;

  ssize_t
    y;

  /*
    Allocate histogram and stretch map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (SetImageGray(image,exception) != MagickFalse)
    (void) SetImageColorspace(image,GRAYColorspace,exception);
  black=(double *) AcquireQuantumMemory(MaxPixelChannels,sizeof(*black));
  white=(double *) AcquireQuantumMemory(MaxPixelChannels,sizeof(*white));
  histogram=(double *) AcquireQuantumMemory(MaxMap+1UL,MaxPixelChannels*
    sizeof(*histogram));
  stretch_map=(double *) AcquireQuantumMemory(MaxMap+1UL,MaxPixelChannels*
    sizeof(*stretch_map));
  if ((black == (double *) NULL) || (white == (double *) NULL) ||
      (histogram == (double *) NULL) || (stretch_map == (double *) NULL))
    {
      if (stretch_map != (double *) NULL)
        stretch_map=(double *) RelinquishMagickMemory(stretch_map);
      if (histogram != (double *) NULL)
        histogram=(double *) RelinquishMagickMemory(histogram);
      if (white != (double *) NULL)
        white=(double *) RelinquishMagickMemory(white);
      if (black != (double *) NULL)
        black=(double *) RelinquishMagickMemory(black);
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
      double
        pixel;

      pixel=GetPixelIntensity(image,p);
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        if (image->channel_mask != DefaultChannels)
          pixel=(double) p[i];
        histogram[GetPixelChannels(image)*ScaleQuantumToMap(
          ClampToQuantum(pixel))+i]++;
      }
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  /*
    Find the histogram boundaries by locating the black/white levels.
  */
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    double
      intensity;

    register ssize_t
      j;

    black[i]=0.0;
    white[i]=MaxRange(QuantumRange);
    intensity=0.0;
    for (j=0; j <= (ssize_t) MaxMap; j++)
    {
      intensity+=histogram[GetPixelChannels(image)*j+i];
      if (intensity > black_point)
        break;
    }
    black[i]=(double) j;
    intensity=0.0;
    for (j=(ssize_t) MaxMap; j != 0; j--)
    {
      intensity+=histogram[GetPixelChannels(image)*j+i];
      if (intensity > ((double) image->columns*image->rows-white_point))
        break;
    }
    white[i]=(double) j;
  }
  histogram=(double *) RelinquishMagickMemory(histogram);
  /*
    Stretch the histogram to create the stretched image mapping.
  */
  (void) memset(stretch_map,0,(MaxMap+1)*GetPixelChannels(image)*
    sizeof(*stretch_map));
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    register ssize_t
      j;

    for (j=0; j <= (ssize_t) MaxMap; j++)
    {
      double
        gamma;

      gamma=PerceptibleReciprocal(white[i]-black[i]);
      if (j < (ssize_t) black[i])
        stretch_map[GetPixelChannels(image)*j+i]=0.0;
      else
        if (j > (ssize_t) white[i])
          stretch_map[GetPixelChannels(image)*j+i]=(double) QuantumRange;
        else
          if (black[i] != white[i])
            stretch_map[GetPixelChannels(image)*j+i]=(double) ScaleMapToQuantum(
              (double) (MaxMap*gamma*(j-black[i])));
    }
  }
  if (image->storage_class == PseudoClass)
    {
      register ssize_t
        j;

      /*
        Stretch-contrast colormap.
      */
      for (j=0; j < (ssize_t) image->colors; j++)
      {
        if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
          {
            i=GetPixelChannelOffset(image,RedPixelChannel);
            image->colormap[j].red=stretch_map[GetPixelChannels(image)*
              ScaleQuantumToMap(ClampToQuantum(image->colormap[j].red))+i];
          }
        if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
          {
            i=GetPixelChannelOffset(image,GreenPixelChannel);
            image->colormap[j].green=stretch_map[GetPixelChannels(image)*
              ScaleQuantumToMap(ClampToQuantum(image->colormap[j].green))+i];
          }
        if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
          {
            i=GetPixelChannelOffset(image,BluePixelChannel);
            image->colormap[j].blue=stretch_map[GetPixelChannels(image)*
              ScaleQuantumToMap(ClampToQuantum(image->colormap[j].blue))+i];
          }
        if ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0)
          {
            i=GetPixelChannelOffset(image,AlphaPixelChannel);
            image->colormap[j].alpha=stretch_map[GetPixelChannels(image)*
              ScaleQuantumToMap(ClampToQuantum(image->colormap[j].alpha))+i];
          }
      }
    }
  /*
    Stretch-contrast image.
  */
  status=MagickTrue;
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
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        if (black[j] == white[j])
          continue;
        q[j]=ClampToQuantum(stretch_map[GetPixelChannels(image)*
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
        proceed=SetImageProgress(image,ContrastStretchImageTag,progress,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  stretch_map=(double *) RelinquishMagickMemory(stretch_map);
  white=(double *) RelinquishMagickMemory(white);
  black=(double *) RelinquishMagickMemory(black);
  return(status);
}