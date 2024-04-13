    template<typename t>
    CImg<_cimg_Ttfloat> get_solve_tridiagonal(const CImg<t>& A) const {
      return CImg<_cimg_Ttfloat>(*this,false).solve_tridiagonal(A);