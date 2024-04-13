static inline void ModulateHSL(const double percent_hue,
  const double percent_saturation,const double percent_lightness,double *red,
  double *green,double *blue)
{
  double
    hue,
    lightness,
    saturation;

  /*
    Increase or decrease color lightness, saturation, or hue.
  */
  ConvertRGBToHSL(*red,*green,*blue,&hue,&saturation,&lightness);
  hue+=fmod((percent_hue-100.0),200.0)/200.0;
  saturation*=0.01*percent_saturation;
  lightness*=0.01*percent_lightness;
  ConvertHSLToRGB(hue,saturation,lightness,red,green,blue);
}