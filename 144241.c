    **/
    CImg<T>& rol(const char *const expression) {
      return rol((+*this)._fill(expression,true,1,0,0,"rol",this));