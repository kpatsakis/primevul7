MagickExport MagickBooleanType CLAHEImage(Image *image,const size_t width,
  const size_t height,const size_t number_bins,const double clip_limit,
  ExceptionInfo *exception)
{
#define CLAHEImageTag  "CLAHE/Image"

  CacheView
    *image_view;

  ColorspaceType
    colorspace;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MemoryInfo
    *pixel_cache;

  RangeInfo
    range_info;

  RectangleInfo
    clahe_info,
    tile_info;

  size_t
    n;

  ssize_t
    y;

  unsigned short
    *pixels;

  /*
    Configure CLAHE parameters.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  range_info.min=0;
  range_info.max=NumberCLAHEGrays-1;
  tile_info.width=width;
  if (tile_info.width == 0)
    tile_info.width=image->columns >> 3;
  tile_info.height=height;
  if (tile_info.height == 0)
    tile_info.height=image->rows >> 3;
  tile_info.x=0;
  if ((image->columns % tile_info.width) != 0)
    tile_info.x=(ssize_t) tile_info.width-(image->columns % tile_info.width);
  tile_info.y=0;
  if ((image->rows % tile_info.height) != 0)
    tile_info.y=(ssize_t) tile_info.height-(image->rows % tile_info.height);
  clahe_info.width=image->columns+tile_info.x;
  clahe_info.height=image->rows+tile_info.y;
  clahe_info.x=(ssize_t) clahe_info.width/tile_info.width;
  clahe_info.y=(ssize_t) clahe_info.height/tile_info.height;
  pixel_cache=AcquireVirtualMemory(clahe_info.width,clahe_info.height*
    sizeof(*pixels));
  if (pixel_cache == (MemoryInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  pixels=(unsigned short *) GetVirtualMemoryBlob(pixel_cache);
  colorspace=image->colorspace;
  if (TransformImageColorspace(image,LabColorspace,exception) == MagickFalse)
    {
      pixel_cache=RelinquishVirtualMemory(pixel_cache);
      return(MagickFalse);
    }
  /*
    Initialize CLAHE pixels.
  */
  image_view=AcquireVirtualCacheView(image,exception);
  progress=0;
  status=MagickTrue;
  n=0;
  for (y=0; y < (ssize_t) clahe_info.height; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-(tile_info.x >> 1),y-
      (tile_info.y >> 1),clahe_info.width,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) clahe_info.width; x++)
    {
      pixels[n++]=ScaleQuantumToShort(p[0]);
      p+=GetPixelChannels(image);
    }
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,CLAHEImageTag,progress,2*
          GetPixelChannels(image));
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  status=CLAHE(&clahe_info,&tile_info,&range_info,number_bins == 0 ?
    (size_t) 128 : MagickMin(number_bins,256),clip_limit,pixels);
  if (status == MagickFalse)
    (void) ThrowMagickException(exception,GetMagickModule(),
      ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
  /*
    Push CLAHE pixels to CLAHE image.
  */
  image_view=AcquireAuthenticCacheView(image,exception);
  n=clahe_info.width*(tile_info.y >> 1);
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
    n+=tile_info.x >> 1;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      q[0]=ScaleShortToQuantum(pixels[n++]);
      q+=GetPixelChannels(image);
    }
    n+=(clahe_info.width-image->columns-(tile_info.x >> 1));
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
        proceed=SetImageProgress(image,CLAHEImageTag,progress,2*
          GetPixelChannels(image));
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  pixel_cache=RelinquishVirtualMemory(pixel_cache);
  if (TransformImageColorspace(image,colorspace,exception) == MagickFalse)
    status=MagickFalse;
  return(status);
}