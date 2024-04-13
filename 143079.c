    template<typename t>
    CImg<Tfloat> get_blur_anisotropic(const CImg<t>& G,
                                      const float amplitude=60, const float dl=0.8f, const float da=30,
                                      const float gauss_prec=2, const unsigned int interpolation_type=0,
                                      const bool is_fast_approx=true) const {
      return CImg<Tfloat>(*this,false).blur_anisotropic(G,amplitude,dl,da,gauss_prec,interpolation_type,is_fast_approx);