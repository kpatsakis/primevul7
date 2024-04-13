    template<typename T>
    CImgDisplay& resize(const CImg<T>& img, const bool force_redraw=true) {
      return resize(img._width,img._height,force_redraw);