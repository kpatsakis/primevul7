    **/
    CImg<T>& threshold(const T& value, const bool soft_threshold=false, const bool strict_threshold=false) {
      if (is_empty()) return *this;
      if (strict_threshold) {
        if (soft_threshold)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32768))
          cimg_rofoff(*this,off) {
            const T v = _data[off];
            _data[off] = v>value?(T)(v-value):v<-(float)value?(T)(v + value):(T)0;
          }
        else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),65536))
          cimg_rofoff(*this,off) _data[off] = _data[off]>value?(T)1:(T)0;
      } else {
        if (soft_threshold)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32768))
          cimg_rofoff(*this,off) {
            const T v = _data[off];
            _data[off] = v>=value?(T)(v-value):v<=-(float)value?(T)(v + value):(T)0;
          }
        else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),65536))
          cimg_rofoff(*this,off) _data[off] = _data[off]>=value?(T)1:(T)0;
      }
      return *this;