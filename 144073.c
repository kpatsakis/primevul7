    template<typename t>
    CImg<Tfloat> get_blur_bilateral(const CImg<t>& guide,
                                    const float sigma_x, const float sigma_y,
                                    const float sigma_z, const float sigma_r,
                                    const float sampling_x, const float sampling_y,
                                    const float sampling_z, const float sampling_r) const {
      return CImg<Tfloat>(*this,false).blur_bilateral(guide,sigma_x,sigma_y,sigma_z,sigma_r,
                                                      sampling_x,sampling_y,sampling_z,sampling_r);