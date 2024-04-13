    **/
    CImg<T>& dilate(const unsigned int s) {
      return dilate(s,s,s);