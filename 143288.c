    //! Sharpen image \newinstance.
    CImg<T> get_sharpen(const float amplitude, const bool sharpen_type=false, const float edge=1,
                        const float alpha=0, const float sigma=0) const {
      return (+*this).sharpen(amplitude,sharpen_type,edge,alpha,sigma);