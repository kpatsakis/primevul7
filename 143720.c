
      unsigned int vector(const unsigned int siz, const double value) { // Insert new initialized vector
        const unsigned int pos = vector(siz);
        double *ptr = &mem[pos] + 1;
        for (unsigned int i = 0; i<siz; ++i) *(ptr++) = value;
        return pos;