MagickExport MagickBooleanType BrightnessContrastImage(Image *image,
  const double brightness,const double contrast,ExceptionInfo *exception)
{
#define BrightnessContastImageTag  "BrightnessContast/Image"

  double
    alpha,
    coefficients[2],
    intercept,
    slope;

  MagickBooleanType
    status;

  /*
    Compute slope and intercept.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  alpha=contrast;
  slope=tan((double) (MagickPI*(alpha/100.0+1.0)/4.0));
  if (slope < 0.0)
    slope=0.0;
  intercept=brightness/100.0+((100-brightness)/200.0)*(1.0-slope);
  coefficients[0]=slope;
  coefficients[1]=intercept;
  status=FunctionImage(image,PolynomialFunction,2,coefficients,exception);
  return(status);
}