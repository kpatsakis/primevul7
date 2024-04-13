    **/
    CImg<T>& max(const char *const expression) {
      return max((+*this)._fill(expression,true,1,0,0,"max",this));