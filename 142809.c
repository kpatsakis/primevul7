     **/
    static CImg<T>& empty() {
      static CImg<T> _empty;
      return _empty.assign();