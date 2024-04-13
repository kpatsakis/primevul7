DestroyJNG(unsigned char *chunk,Image **color_image,
  ImageInfo **color_image_info,Image **alpha_image,
  ImageInfo **alpha_image_info)
{
  (void) RelinquishMagickMemory(chunk);
  if (color_image_info && *color_image_info)
  {
    DestroyImageInfo(*color_image_info);
    *color_image_info = (ImageInfo *)NULL;
  }
  if (alpha_image_info && *alpha_image_info)
  {
    DestroyImageInfo(*alpha_image_info);
    *alpha_image_info = (ImageInfo *)NULL;
  }
  if (color_image && *color_image)
  {
    DestroyImage(*color_image);
    *color_image = (Image *)NULL;
  }
  if (alpha_image && *alpha_image)
  {
    DestroyImage(*alpha_image);
    *alpha_image = (Image *)NULL;
  }
}