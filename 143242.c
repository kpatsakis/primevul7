    **/
    CImg<T>& blur(const float sigma, const bool boundary_conditions=true, const bool is_gaussian=false) {
      const float nsigma = sigma>=0?sigma:-sigma*cimg::max(_width,_height,_depth)/100;
      return blur(nsigma,nsigma,nsigma,boundary_conditions,is_gaussian);