    template<typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const float sigma,
                           const tc *const color, const float opacity=1) {
      return draw_gaussian(xc,yc,CImg<floatT>::diagonal(sigma,sigma),color,opacity);