     **/
    CImg<T>& blur_anisotropic(const float amplitude, const float sharpness=0.7f, const float anisotropy=0.6f,
                              const float alpha=0.6f, const float sigma=1.1f, const float dl=0.8f, const float da=30,
                              const float gauss_prec=2, const unsigned int interpolation_type=0,
                              const bool is_fast_approx=true) {
      const float nalpha = alpha>=0?alpha:-alpha*cimg::max(_width,_height,_depth)/100;
      const float nsigma = sigma>=0?sigma:-sigma*cimg::max(_width,_height,_depth)/100;
      return blur_anisotropic(get_diffusion_tensors(sharpness,anisotropy,nalpha,nsigma,interpolation_type!=3),
                              amplitude,dl,da,gauss_prec,interpolation_type,is_fast_approx);