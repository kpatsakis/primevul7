    //! Evaluate math formula \const.
    double eval(const char *const expression,
                const double x=0, const double y=0, const double z=0, const double c=0,
                const CImgList<T> *const list_inputs=0, CImgList<T> *const list_outputs=0) const {
      return _eval(0,expression,x,y,z,c,list_inputs,list_outputs);