    template<typename t>
    CImg<T>& solve_tridiagonal(const CImg<t>& A) {
      const unsigned int siz = (unsigned int)size();
      if (A._width!=3 || A._height!=siz)
        throw CImgArgumentException(_cimg_instance
                                    "solve_tridiagonal(): Instance and tridiagonal matrix "
                                    "(%u,%u,%u,%u,%p) have incompatible dimensions.",
                                    cimg_instance,
                                    A._width,A._height,A._depth,A._spectrum,A._data);
      typedef _cimg_Ttfloat Ttfloat;
      const Ttfloat epsilon = 1e-4f;
      CImg<Ttfloat> B = A.get_column(1), V(*this,false);
      for (int i = 1; i<(int)siz; ++i) {
        const Ttfloat m = A(0,i)/(B[i - 1]?B[i - 1]:epsilon);
        B[i] -= m*A(2,i - 1);
        V[i] -= m*V[i - 1];
      }
      (*this)[siz - 1] = (T)(V[siz - 1]/(B[siz - 1]?B[siz - 1]:epsilon));
      for (int i = (int)siz - 2; i>=0; --i) (*this)[i] = (T)((V[i] - A(2,i)*(*this)[i + 1])/(B[i]?B[i]:epsilon));
      return *this;