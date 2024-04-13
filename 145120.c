static MagickBooleanType ReadRectangle(Image *image,PICTRectangle *rectangle)
{
  rectangle->top=(short) ReadBlobMSBShort(image);
  rectangle->left=(short) ReadBlobMSBShort(image);
  rectangle->bottom=(short) ReadBlobMSBShort(image);
  rectangle->right=(short) ReadBlobMSBShort(image);
  if ((EOFBlob(image) != MagickFalse) ||
      ((rectangle->bottom-rectangle->top) <= 0) ||
      ((rectangle->right-rectangle->left) <= 0))
    return(MagickFalse);
  return(MagickTrue);
}