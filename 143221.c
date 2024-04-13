    template<typename t>
    CImg<_cimg_Ttfloat> get_correlate(const CImg<t>& kernel, const bool boundary_conditions=true,
                                      const bool is_normalized=false, const int dilation=1) const {
      return _correlate(kernel,boundary_conditions,is_normalized,dilation,false);