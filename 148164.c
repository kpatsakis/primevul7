MagickExport MagickBooleanType LevelImageColors(Image *image,
  const PixelInfo *black_color,const PixelInfo *white_color,
  const MagickBooleanType invert,ExceptionInfo *exception)
{
  ChannelType
    channel_mask;

  MagickStatusType
    status;

  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((IsGrayColorspace(image->colorspace) != MagickFalse) &&
      ((IsGrayColorspace(black_color->colorspace) == MagickFalse) ||
       (IsGrayColorspace(white_color->colorspace) == MagickFalse)))
    (void) SetImageColorspace(image,sRGBColorspace,exception);
  status=MagickTrue;
  if (invert == MagickFalse)
    {
      if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
        {
          channel_mask=SetImageChannelMask(image,RedChannel);
          status&=LevelImage(image,black_color->red,white_color->red,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
        {
          channel_mask=SetImageChannelMask(image,GreenChannel);
          status&=LevelImage(image,black_color->green,white_color->green,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
        {
          channel_mask=SetImageChannelMask(image,BlueChannel);
          status&=LevelImage(image,black_color->blue,white_color->blue,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
          (image->colorspace == CMYKColorspace))
        {
          channel_mask=SetImageChannelMask(image,BlackChannel);
          status&=LevelImage(image,black_color->black,white_color->black,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
          (image->alpha_trait != UndefinedPixelTrait))
        {
          channel_mask=SetImageChannelMask(image,AlphaChannel);
          status&=LevelImage(image,black_color->alpha,white_color->alpha,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
    }
  else
    {
      if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
        {
          channel_mask=SetImageChannelMask(image,RedChannel);
          status&=LevelizeImage(image,black_color->red,white_color->red,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
        {
          channel_mask=SetImageChannelMask(image,GreenChannel);
          status&=LevelizeImage(image,black_color->green,white_color->green,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
        {
          channel_mask=SetImageChannelMask(image,BlueChannel);
          status&=LevelizeImage(image,black_color->blue,white_color->blue,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
          (image->colorspace == CMYKColorspace))
        {
          channel_mask=SetImageChannelMask(image,BlackChannel);
          status&=LevelizeImage(image,black_color->black,white_color->black,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
      if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
          (image->alpha_trait != UndefinedPixelTrait))
        {
          channel_mask=SetImageChannelMask(image,AlphaChannel);
          status&=LevelizeImage(image,black_color->alpha,white_color->alpha,1.0,
            exception);
          (void) SetImageChannelMask(image,channel_mask);
        }
    }
  return(status != 0 ? MagickTrue : MagickFalse);
}