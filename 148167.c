MagickExport MagickBooleanType NormalizeImage(Image *image,
  ExceptionInfo *exception)
{
  double
    black_point,
    white_point;

  black_point=(double) image->columns*image->rows*0.0015;
  white_point=(double) image->columns*image->rows*0.9995;
  return(ContrastStretchImage(image,black_point,white_point,exception));
}