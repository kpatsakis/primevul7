static inline void EMFSetDimensions(Image * image,Gdiplus::Image *source)
{
  if ((image->x_resolution <= 0.0) || (image->y_resolution <= 0.0))
    return;

  image->columns=(size_t) floor((Gdiplus::REAL) source->GetWidth()/
    source->GetHorizontalResolution()*image->x_resolution+0.5);
  image->rows=(size_t)floor((Gdiplus::REAL) source->GetHeight()/
    source->GetVerticalResolution()*image->y_resolution+0.5);
}