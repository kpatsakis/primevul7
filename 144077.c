    template<typename T>
    CImgDisplay& display(const CImg<T>& img) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "display(): Empty specified image.",
                                    cimgdisplay_instance);
      if (is_empty()) return assign(img);
      return render(img).paint(false);