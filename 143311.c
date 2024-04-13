      // Return a string that defines the calling function + the user-defined function scope.
      CImg<charT> calling_function_s() const {
        CImg<charT> res;
        const unsigned int
          l1 = calling_function?(unsigned int)std::strlen(calling_function):0U,
          l2 = user_macro?(unsigned int)std::strlen(user_macro):0U;
        if (l2) {
          res.assign(l1 + l2 + 48);
          cimg_snprintf(res,res._width,"%s(): When substituting function '%s()'",calling_function,user_macro);
        } else {
          res.assign(l1 + l2 + 4);
          cimg_snprintf(res,res._width,"%s()",calling_function);
        }
        return res;