#if cimg_use_cpp11==1
    CImg(CImg<T>&& img):_width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      swap(img);