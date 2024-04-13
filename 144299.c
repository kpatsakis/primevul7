    **/
    CImg<T>& blur_median(const unsigned int n, const float threshold=0) {
      if (!n) return *this;
      return get_blur_median(n,threshold).move_to(*this);