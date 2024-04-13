    template<typename t>
    const CImg<T>& symmetric_eigen(CImg<t>& val, CImg<t>& vec) const {
      if (is_empty()) { val.assign(); vec.assign(); return *this; }
      if (_width!=_height || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "eigen(): Instance is not a square matrix.",
                                    cimg_instance);
      val.assign(1,_width);
      vec.assign(_width,_width);

      if (_width==1) { val[0] = cimg::abs((*this)[0]); vec[0] = 1; return *this; }
      if (_width==2) {
        const double
          a = (*this)[0], b = (*this)[1], c = (*this)[2], d = (*this)[3],
          e = a + d, f = std::sqrt(std::max(e*e - 4*(a*d - b*c),0.0)),
          l1 = 0.5*(e - f), l2 = 0.5*(e + f),
          n = std::sqrt(cimg::sqr(l2 - a) + b*b);
        val[0] = (t)l2;
        val[1] = (t)l1;
        if (n>0) { vec[0] = (t)(b/n); vec[2] = (t)((l2 - a)/n); } else { vec[0] = 1; vec[2] = 0; }
        vec[1] = -vec[2];
        vec[3] = vec[0];
        return *this;
      }

#ifdef cimg_use_lapack
      char JOB = 'V', UPLO = 'U';
      int N = _width, LWORK = 4*N, INFO;
      Tfloat
        *const lapA = new Tfloat[N*N],
        *const lapW = new Tfloat[N],
        *const WORK = new Tfloat[LWORK];
      cimg_forXY(*this,k,l) lapA[k*N + l] = (Tfloat)((*this)(k,l));
      cimg::syev(JOB,UPLO,N,lapA,lapW,WORK,LWORK,INFO);
      if (INFO)
        cimg::warn(_cimg_instance
                   "symmetric_eigen(): LAPACK library function dsyev_() returned error code %d.",
                   cimg_instance,
                   INFO);
      if (!INFO) {
        cimg_forY(val,i) val(i) = (T)lapW[N - 1 -i];
        cimg_forXY(vec,k,l) vec(k,l) = (T)(lapA[(N - 1 - k)*N + l]);
      } else { val.fill(0); vec.fill(0); }
      delete[] lapA; delete[] lapW; delete[] WORK;

#else
      CImg<t> V(_width,_width);
      Tfloat M = 0, m = (Tfloat)min_max(M), maxabs = cimg::max((Tfloat)1,cimg::abs(m),cimg::abs(M));
      (CImg<Tfloat>(*this,false)/=maxabs).SVD(vec,val,V,false);
      if (maxabs!=1) val*=maxabs;

      bool is_ambiguous = false;
      float eig = 0;
      cimg_forY(val,p) { // Check for ambiguous cases
        if (val[p]>eig) eig = (float)val[p];
        t scal = 0;
        cimg_forY(vec,y) scal+=vec(p,y)*V(p,y);
        if (cimg::abs(scal)<0.9f) is_ambiguous = true;
        if (scal<0) val[p] = -val[p];
      }
      if (is_ambiguous) {
        ++(eig*=2);
        SVD(vec,val,V,false,40,eig);
        val-=eig;
      }

      CImg<intT> permutations; // Sort eigenvalues in decreasing order
      CImg<t> tmp(_width);
      val.sort(permutations,false);
      cimg_forY(vec,k) {
        cimg_forY(permutations,y) tmp(y) = vec(permutations(y),k);
        std::memcpy(vec.data(0,k),tmp._data,sizeof(t)*_width);
      }
#endif
      return *this;