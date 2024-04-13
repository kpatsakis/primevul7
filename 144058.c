    **/
    CImg<T>& blur_patch(const float sigma_s, const float sigma_p, const unsigned int patch_size=3,
                        const unsigned int lookup_size=4, const float smoothness=0, const bool is_fast_approx=true) {
      if (is_empty() || !patch_size || !lookup_size) return *this;
      return get_blur_patch(sigma_s,sigma_p,patch_size,lookup_size,smoothness,is_fast_approx).move_to(*this);