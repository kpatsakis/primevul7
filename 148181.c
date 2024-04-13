static inline double LevelPixel(const double black_point,
  const double white_point,const double gamma,const double pixel)
{
  double
    level_pixel,
    scale;

  scale=PerceptibleReciprocal(white_point-black_point);
  level_pixel=QuantumRange*gamma_pow(scale*((double) pixel-black_point),
    PerceptibleReciprocal(gamma));
  return(level_pixel);
}