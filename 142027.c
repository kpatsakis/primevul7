static inline void SetPixelBackgoundColor(const Image *magick_restrict image,
  Quantum *magick_restrict pixel)
{
  ssize_t
    i;

  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
    pixel[i]=(Quantum) 0;
  pixel[image->channel_map[RedPixelChannel].offset]=
    ClampToQuantum(image->background_color.red);
  pixel[image->channel_map[GreenPixelChannel].offset]=
    ClampToQuantum(image->background_color.green);
  pixel[image->channel_map[BluePixelChannel].offset]=
    ClampToQuantum(image->background_color.blue);
  if (image->channel_map[BlackPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[BlackPixelChannel].offset]=
      ClampToQuantum(image->background_color.black);
  if (image->channel_map[AlphaPixelChannel].traits != UndefinedPixelTrait)
    pixel[image->channel_map[AlphaPixelChannel].offset]=
      image->background_color.alpha_trait == UndefinedPixelTrait ? OpaqueAlpha :
      ClampToQuantum(image->background_color.alpha);
}