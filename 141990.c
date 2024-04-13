static inline MagickBooleanType IsPixelAtDepth(const Quantum pixel,
  const QuantumAny range)
{
  Quantum
    quantum;

  if (range == 0)
    return(MagickTrue);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  quantum=(Quantum) (((MagickRealType) QuantumRange*((QuantumAny)
    (((MagickRealType) range*pixel)/QuantumRange+0.5)))/range+0.5);
#else
  quantum=(Quantum) (((MagickRealType) QuantumRange*((QuantumAny)
    (((MagickRealType) range*pixel)/QuantumRange+0.5)))/range);
#endif
  return(pixel == quantum ? MagickTrue : MagickFalse);
}