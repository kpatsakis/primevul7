    **/
    CImg<T>& min(const char *const expression) {
      return min((+*this)._fill(expression,true,1,0,0,"min",this));