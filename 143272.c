    template<typename t>
    CImg<T> get_fill(const CImg<t>& values, const bool repeat_values=true) const {
      return repeat_values?CImg<T>(_width,_height,_depth,_spectrum).fill(values,repeat_values):
        (+*this).fill(values,repeat_values);