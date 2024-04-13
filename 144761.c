    **/
    CImg<T>& operator&=(const char *const expression) {
      return *this&=(+*this)._fill(expression,true,1,0,0,"operator&=",this);