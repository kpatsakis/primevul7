    template<typename tc>
    CImg<T>& draw_gaussian(const int xc, const int yc, const float r1, const float r2, const float ru, const float rv,
                           const tc *const color, const float opacity=1) {
      const double
        a = r1*ru*ru + r2*rv*rv,
        b = (r1-r2)*ru*rv,
        c = r1*rv*rv + r2*ru*ru;
      const CImg<Tfloat> tensor(2,2,1,1, a,b,b,c);
      return draw_gaussian(xc,yc,tensor,color,opacity);