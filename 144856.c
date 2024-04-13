
      unsigned int vector(const unsigned int siz) { // Insert new vector of specified size in memory
        if (mempos + siz>=mem._width) {
          mem.resize(2*mem._width + siz,1,1,1,0);
          memtype.resize(mem._width,1,1,1,0);
        }
        const unsigned int pos = mempos++;
        mem[pos] = cimg::type<double>::nan();
        memtype[pos] = siz + 1;
        mempos+=siz;
        return pos;