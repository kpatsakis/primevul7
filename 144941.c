    **/
    CImg<T>& erode(const unsigned int s) {
      return erode(s,s,s);