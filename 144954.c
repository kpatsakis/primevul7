    **/
    CImg<T>& fill(const T& val) {
      if (is_empty()) return *this;
      if (val && sizeof(T)!=1) cimg_for(*this,ptrd,T) *ptrd = val;
      else std::memset(_data,(int)(ulongT)val,sizeof(T)*size()); // Double cast to allow val to be (void*)
      return *this;