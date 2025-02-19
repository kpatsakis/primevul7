static inline void ModulateHCLp(const double percent_hue,
  const double percent_chroma,const double percent_luma,double *red,
  double *green,double *blue)
{
  double
    hue,
    luma,
    chroma;

  /*
    Increase or decrease color luma, chroma, or hue.
  */
  ConvertRGBToHCLp(*red,*green,*blue,&hue,&chroma,&luma);
  hue+=fmod((percent_hue-100.0),200.0)/200.0;
  chroma*=0.01*percent_chroma;
  luma*=0.01*percent_luma;
  ConvertHCLpToRGB(hue,chroma,luma,red,green,blue);
}