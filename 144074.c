    template<typename t>
    void eval(CImg<t>& output, const char *const expression,
              const double x=0, const double y=0, const double z=0, const double c=0,
              const CImgList<T> *const list_inputs=0, CImgList<T> *const list_outputs=0) const {
      _eval(output,0,expression,x,y,z,c,list_inputs,list_outputs);