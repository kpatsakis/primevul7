    //! Compute the Moore-Penrose pseudo-inverse of the instance image, viewed as a matrix \newinstance.
    CImg<Tfloat> get_pseudoinvert() const {
      CImg<Tfloat> U, S, V;
      SVD(U,S,V);
      const Tfloat tolerance = (sizeof(Tfloat)<=4?5.96e-8f:1.11e-16f)*std::max(_width,_height)*S.max();
      cimg_forX(V,x) {
	const Tfloat s = S(x), invs = s>tolerance?1/s:0;
	cimg_forY(V,y) V(x,y)*=invs;
      }
      return V*U.transpose();