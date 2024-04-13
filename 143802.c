
      static double mp_vector_eq(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2) + 1,
          *ptr2 = &_mp_arg(4) + 1;
        unsigned int p1 = (unsigned int)mp.opcode[3], p2 = (unsigned int)mp.opcode[5], n;
        const int N = (int)_mp_arg(6);
        const bool case_sensitive = (bool)_mp_arg(7);
        bool still_equal = true;
        double value;
        if (!N) return true;

        // Compare all values.
        if (N<0) {
          if (p1>0 && p2>0) { // Vector == vector
            if (p1!=p2) return false;
            if (case_sensitive)
              while (still_equal && p1--) still_equal = *(ptr1++)==*(ptr2++);
            else
              while (still_equal && p1--)
                still_equal = cimg::lowercase(*(ptr1++))==cimg::lowercase(*(ptr2++));
            return still_equal;
          } else if (p1>0 && !p2) { // Vector == scalar
            value = _mp_arg(4);
            if (!case_sensitive) value = cimg::lowercase(value);
            while (still_equal && p1--) still_equal = *(ptr1++)==value;
            return still_equal;
          } else if (!p1 && p2>0) { // Scalar == vector
            value = _mp_arg(2);
            if (!case_sensitive) value = cimg::lowercase(value);
            while (still_equal && p2--) still_equal = *(ptr2++)==value;
            return still_equal;
          } else { // Scalar == scalar
            if (case_sensitive) return _mp_arg(2)==_mp_arg(4);
            else return cimg::lowercase(_mp_arg(2))==cimg::lowercase(_mp_arg(4));
          }
        }

        // Compare only first N values.
        if (p1>0 && p2>0) { // Vector == vector
          n = cimg::min((unsigned int)N,p1,p2);
          if (case_sensitive)
            while (still_equal && n--) still_equal = *(ptr1++)==(*ptr2++);
          else
            while (still_equal && n--) still_equal = cimg::lowercase(*(ptr1++))==cimg::lowercase(*(ptr2++));
          return still_equal;
        } else if (p1>0 && !p2) { // Vector == scalar
          n = std::min((unsigned int)N,p1);
          value = _mp_arg(4);
          if (!case_sensitive) value = cimg::lowercase(value);
          while (still_equal && n--) still_equal = *(ptr1++)==value;
          return still_equal;
        } else if (!p1 && p2>0) { // Scalar == vector
          n = std::min((unsigned int)N,p2);
          value = _mp_arg(2);
          if (!case_sensitive) value = cimg::lowercase(value);
          while (still_equal && n--) still_equal = *(ptr2++)==value;
          return still_equal;
        }  // Scalar == scalar
        if (case_sensitive) return _mp_arg(2)==_mp_arg(4);
        return cimg::lowercase(_mp_arg(2))==cimg::lowercase(_mp_arg(4));