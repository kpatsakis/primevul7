static inline void GetPixelInfoPixel(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel,PixelInfo *magick_restrict pixel_info)
{
  (void) ResetMagickMemory(pixel_info,0,sizeof(*pixel_info));
  pixel_info->storage_class=DirectClass;
  pixel_info->colorspace=sRGBColorspace;
  pixel_info->depth=MAGICKCORE_QUANTUM_DEPTH;
  pixel_info->alpha_trait=UndefinedPixelTrait;
  pixel_info->alpha=(MagickRealType) OpaqueAlpha;
  if (image != (Image *) NULL)
    {
      pixel_info->storage_class=image->storage_class;
      pixel_info->colorspace=image->colorspace;
      pixel_info->fuzz=image->fuzz;
      pixel_info->depth=image->depth;
      pixel_info->alpha_trait=image->alpha_trait;
      if (pixel != (Quantum *) NULL)
        {
          pixel_info->red=(MagickRealType)
            pixel[image->channel_map[RedPixelChannel].offset];
          pixel_info->green=(MagickRealType)
            pixel[image->channel_map[GreenPixelChannel].offset];
          pixel_info->blue=(MagickRealType)
            pixel[image->channel_map[BluePixelChannel].offset];
          if (image->channel_map[BlackPixelChannel].traits != UndefinedPixelTrait)
            pixel_info->black=(MagickRealType)
              pixel[image->channel_map[BlackPixelChannel].offset];
          if (image->channel_map[AlphaPixelChannel].traits != UndefinedPixelTrait)
            {
              pixel_info->alpha=(MagickRealType)
                pixel[image->channel_map[AlphaPixelChannel].offset];
              pixel_info->alpha_trait=BlendPixelTrait;
            }
          if (image->channel_map[IndexPixelChannel].traits != UndefinedPixelTrait)
            pixel_info->index=(MagickRealType)
          pixel[image->channel_map[IndexPixelChannel].offset];
        }
    }
}