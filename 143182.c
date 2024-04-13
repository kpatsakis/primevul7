    template<typename t>
    CImg<_cimg_Ttfloat> get_convolve(const CImg<t>& kernel, const bool boundary_conditions=true,
                                     const bool is_normalized=false, const int dilation=1) const {
      return _correlate(CImg<t>(kernel._data,kernel.size()/kernel._spectrum,1,1,kernel._spectrum,true).
                        get_mirror('x').resize(kernel,-1),boundary_conditions,is_normalized,dilation,true);