static inline double gamma_pow(const double value,const double gamma)
{
  return(value < 0.0 ? value : pow(value,gamma));
}