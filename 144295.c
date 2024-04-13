    template<typename t>
    CImg<Tfloat> get_blur_bilateral(const CImg<t>& guide,
                                    const float sigma_s, const float sigma_r,
                                    const float sampling_s=0, const float sampling_r=0) const {
      return CImg<Tfloat>(*this,false).blur_bilateral(guide,sigma_s,sigma_r,sampling_s,sampling_r);