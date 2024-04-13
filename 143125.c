    template<typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const float zc, const float sigma,
                           const tc *const color, const float opacity=1) {
      return draw_gaussian(xc,yc,zc,CImg<floatT>::diagonal(sigma,sigma,sigma),color,opacity);