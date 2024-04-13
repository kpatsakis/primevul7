    **/
    T& at(const int offset, const T& out_value) {
      return (offset<0 || offset>=(int)size())?(cimg::temporary(out_value)=out_value):(*this)[offset];