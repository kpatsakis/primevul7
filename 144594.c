    template<typename t>
    CImg<T>& blur_guided(const CImg<t>& guide, const float radius, const float regularization) {
      return get_blur_guided(guide,radius,regularization).move_to(*this);