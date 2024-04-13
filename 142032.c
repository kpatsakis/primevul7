static inline Quantum ClampPixel(const MagickRealType pixel)
{
  if (pixel < 0.0f)
    return((Quantum) 0);
  if (pixel >= (MagickRealType) QuantumRange)
    return((Quantum) QuantumRange);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (pixel+0.5f));
#else
  return((Quantum) pixel);
#endif
}