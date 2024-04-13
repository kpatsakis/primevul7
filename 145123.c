static MagickBooleanType ReadPixmap(Image *image,PICTPixmap *pixmap)
{
  pixmap->version=(short) ReadBlobMSBShort(image);
  pixmap->pack_type=(short) ReadBlobMSBShort(image);
  pixmap->pack_size=ReadBlobMSBLong(image);
  pixmap->horizontal_resolution=1UL*ReadBlobMSBShort(image);
  (void) ReadBlobMSBShort(image);
  pixmap->vertical_resolution=1UL*ReadBlobMSBShort(image);
  (void) ReadBlobMSBShort(image);
  pixmap->pixel_type=(short) ReadBlobMSBShort(image);
  pixmap->bits_per_pixel=(short) ReadBlobMSBShort(image);
  pixmap->component_count=(short) ReadBlobMSBShort(image);
  pixmap->component_size=(short) ReadBlobMSBShort(image);
  pixmap->plane_bytes=ReadBlobMSBLong(image);
  pixmap->table=ReadBlobMSBLong(image);
  pixmap->reserved=ReadBlobMSBLong(image);
  if ((EOFBlob(image) != MagickFalse) || (pixmap->bits_per_pixel <= 0) ||
      (pixmap->bits_per_pixel > 32) || (pixmap->component_count <= 0) ||
      (pixmap->component_count > 4) || (pixmap->component_size <= 0))
    return(MagickFalse);
  return(MagickTrue);
}