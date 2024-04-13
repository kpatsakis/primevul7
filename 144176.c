    //! Compute field of diffusion tensors for edge-preserving smoothing \newinstance.
    CImg<Tfloat> get_diffusion_tensors(const float sharpness=0.7f, const float anisotropy=0.6f,
                                       const float alpha=0.6f, const float sigma=1.1f, const bool is_sqrt=false) const {
      return CImg<Tfloat>(*this,false).diffusion_tensors(sharpness,anisotropy,alpha,sigma,is_sqrt);