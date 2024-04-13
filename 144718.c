    template<typename T>
    CImgDisplay& render(const CImg<T>& img) {
      return assign(img);