MagickExport MagickBooleanType ClipImage(Image *image,ExceptionInfo *exception)
{
  return(ClipImagePath(image,"#1",MagickTrue,exception));
}