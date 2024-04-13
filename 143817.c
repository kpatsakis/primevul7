    template<typename t>
    CImg<T>& blur_bilateral(const CImg<t>& guide,
                            const float sigma_s, const float sigma_r,
                            const float sampling_s=0, const float sampling_r=0) {
      const float _sigma_s = sigma_s>=0?sigma_s:-sigma_s*cimg::max(_width,_height,_depth)/100;
      return blur_bilateral(guide,_sigma_s,_sigma_s,_sigma_s,sigma_r,sampling_s,sampling_s,sampling_s,sampling_r);