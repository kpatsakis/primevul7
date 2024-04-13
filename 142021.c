static inline Quantum GetPixelOpacity(const Image *magick_restrict image,
  const Quantum *magick_restrict pixel)
{
  if (image->channel_map[AlphaPixelChannel].traits != BlendPixelTrait)
    return(QuantumRange-OpaqueAlpha);
  return(QuantumRange-pixel[image->channel_map[AlphaPixelChannel].offset]);
}