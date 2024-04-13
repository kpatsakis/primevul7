    template<typename t>
    CImgDisplay& operator=(const CImg<t>& img) {
      return display(img);