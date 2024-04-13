    **/
    bool operator==(const char *const expression) const {
      return *this==(+*this)._fill(expression,true,1,0,0,"operator==",this);