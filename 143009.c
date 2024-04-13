    **/
    CImg<T>& pow(const char *const expression) {
      return pow((+*this)._fill(expression,true,1,0,0,"pow",this));