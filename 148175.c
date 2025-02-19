MagickExport Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
{
#define EnhanceImageTag  "Enhance/Image"
#define EnhancePixel(weight) \
  mean=QuantumScale*((double) GetPixelRed(image,r)+pixel.red)/2.0; \
  distance=QuantumScale*((double) GetPixelRed(image,r)-pixel.red); \
  distance_squared=(4.0+mean)*distance*distance; \
  mean=QuantumScale*((double) GetPixelGreen(image,r)+pixel.green)/2.0; \
  distance=QuantumScale*((double) GetPixelGreen(image,r)-pixel.green); \
  distance_squared+=(7.0-mean)*distance*distance; \
  mean=QuantumScale*((double) GetPixelBlue(image,r)+pixel.blue)/2.0; \
  distance=QuantumScale*((double) GetPixelBlue(image,r)-pixel.blue); \
  distance_squared+=(5.0-mean)*distance*distance; \
  mean=QuantumScale*((double) GetPixelBlack(image,r)+pixel.black)/2.0; \
  distance=QuantumScale*((double) GetPixelBlack(image,r)-pixel.black); \
  distance_squared+=(5.0-mean)*distance*distance; \
  mean=QuantumScale*((double) GetPixelAlpha(image,r)+pixel.alpha)/2.0; \
  distance=QuantumScale*((double) GetPixelAlpha(image,r)-pixel.alpha); \
  distance_squared+=(5.0-mean)*distance*distance; \
  if (distance_squared < 0.069) \
    { \
      aggregate.red+=(weight)*GetPixelRed(image,r); \
      aggregate.green+=(weight)*GetPixelGreen(image,r); \
      aggregate.blue+=(weight)*GetPixelBlue(image,r); \
      aggregate.black+=(weight)*GetPixelBlack(image,r); \
      aggregate.alpha+=(weight)*GetPixelAlpha(image,r); \
      total_weight+=(weight); \
    } \
  r+=GetPixelChannels(image);

  CacheView
    *enhance_view,
    *image_view;

  Image
    *enhance_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Initialize enhanced image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  enhance_image=CloneImage(image,0,0,MagickTrue,
    exception);
  if (enhance_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(enhance_image,DirectClass,exception) == MagickFalse)
    {
      enhance_image=DestroyImage(enhance_image);
      return((Image *) NULL);
    }
  /*
    Enhance image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  enhance_view=AcquireAuthenticCacheView(enhance_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,enhance_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    PixelInfo
      pixel;

    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    ssize_t
      center;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-2,y-2,image->columns+4,5,exception);
    q=QueueCacheViewAuthenticPixels(enhance_view,0,y,enhance_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    center=(ssize_t) GetPixelChannels(image)*(2*(image->columns+4)+2);
    GetPixelInfo(image,&pixel);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      double
        distance,
        distance_squared,
        mean,
        total_weight;

      PixelInfo
        aggregate;

      register const Quantum
        *magick_restrict r;

      GetPixelInfo(image,&aggregate);
      total_weight=0.0;
      GetPixelInfoPixel(image,p+center,&pixel);
      r=p;
      EnhancePixel(5.0); EnhancePixel(8.0); EnhancePixel(10.0);
        EnhancePixel(8.0); EnhancePixel(5.0);
      r=p+GetPixelChannels(image)*(image->columns+4);
      EnhancePixel(8.0); EnhancePixel(20.0); EnhancePixel(40.0);
        EnhancePixel(20.0); EnhancePixel(8.0);
      r=p+2*GetPixelChannels(image)*(image->columns+4);
      EnhancePixel(10.0); EnhancePixel(40.0); EnhancePixel(80.0);
        EnhancePixel(40.0); EnhancePixel(10.0);
      r=p+3*GetPixelChannels(image)*(image->columns+4);
      EnhancePixel(8.0); EnhancePixel(20.0); EnhancePixel(40.0);
        EnhancePixel(20.0); EnhancePixel(8.0);
      r=p+4*GetPixelChannels(image)*(image->columns+4);
      EnhancePixel(5.0); EnhancePixel(8.0); EnhancePixel(10.0);
        EnhancePixel(8.0); EnhancePixel(5.0);
      if (total_weight > MagickEpsilon)
        {
          pixel.red=((aggregate.red+total_weight/2.0)/total_weight);
          pixel.green=((aggregate.green+total_weight/2.0)/total_weight);
          pixel.blue=((aggregate.blue+total_weight/2.0)/total_weight);
          pixel.black=((aggregate.black+total_weight/2.0)/total_weight);
          pixel.alpha=((aggregate.alpha+total_weight/2.0)/total_weight);
        }
      SetPixelViaPixelInfo(enhance_image,&pixel,q);
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(enhance_image);
    }
    if (SyncCacheViewAuthenticPixels(enhance_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,EnhanceImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  enhance_view=DestroyCacheView(enhance_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    enhance_image=DestroyImage(enhance_image);
  return(enhance_image);
}