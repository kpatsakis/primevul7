     **/
    CImg<T>& operator/=(const char *const expression) {
      return div((+*this)._fill(expression,true,1,0,0,"operator/=",this));