    template<typename t>
    CImg<_cimg_Ttfloat> get_solve(const CImg<t>& A) const {
      typedef _cimg_Ttfloat Ttfloat;
      return CImg<Ttfloat>(*this,false).solve(A);