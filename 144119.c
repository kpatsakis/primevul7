    **/
    CImg<T>& shift(const int delta_x, const int delta_y=0, const int delta_z=0, const int delta_c=0,
                   const unsigned int boundary_conditions=0) {
      if (is_empty()) return *this;
      if (boundary_conditions==3)
        return get_crop(-delta_x,-delta_y,-delta_z,-delta_c,
                        width() - delta_x - 1,
                        height() - delta_y - 1,
                        depth() - delta_z - 1,
                        spectrum() - delta_c - 1,3).move_to(*this);
      if (delta_x) // Shift along X-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_x,width()), ndelta_x = (ml<=width()/2)?ml:(ml-width());
          if (!ndelta_x) return *this;
          CImg<T> buf(cimg::abs(ndelta_x));
          if (ndelta_x>0) cimg_forYZC(*this,y,z,c) {
              std::memcpy(buf,data(0,y,z,c),ndelta_x*sizeof(T));
              std::memmove(data(0,y,z,c),data(ndelta_x,y,z,c),(_width-ndelta_x)*sizeof(T));
              std::memcpy(data(_width-ndelta_x,y,z,c),buf,ndelta_x*sizeof(T));
            } else cimg_forYZC(*this,y,z,c) {
              std::memcpy(buf,data(_width + ndelta_x,y,z,c),-ndelta_x*sizeof(T));
              std::memmove(data(-ndelta_x,y,z,c),data(0,y,z,c),(_width + ndelta_x)*sizeof(T));
              std::memcpy(data(0,y,z,c),buf,-ndelta_x*sizeof(T));
            }
        } break;
        case 1 : // Neumann
          if (delta_x<0) {
            const int ndelta_x = (-delta_x>=width())?width() - 1:-delta_x;
            if (!ndelta_x) return *this;
            cimg_forYZC(*this,y,z,c) {
              std::memmove(data(0,y,z,c),data(ndelta_x,y,z,c),(_width-ndelta_x)*sizeof(T));
              T *ptrd = data(_width - 1,y,z,c);
              const T val = *ptrd;
              for (int l = 0; l<ndelta_x - 1; ++l) *(--ptrd) = val;
            }
          } else {
            const int ndelta_x = (delta_x>=width())?width() - 1:delta_x;
            if (!ndelta_x) return *this;
            cimg_forYZC(*this,y,z,c) {
              std::memmove(data(ndelta_x,y,z,c),data(0,y,z,c),(_width-ndelta_x)*sizeof(T));
              T *ptrd = data(0,y,z,c);
              const T val = *ptrd;
              for (int l = 0; l<ndelta_x - 1; ++l) *(++ptrd) = val;
            }
          }
          break;
        default : // Dirichlet
          if (delta_x<=-width() || delta_x>=width()) return fill((T)0);
          if (delta_x<0) cimg_forYZC(*this,y,z,c) {
              std::memmove(data(0,y,z,c),data(-delta_x,y,z,c),(_width + delta_x)*sizeof(T));
              std::memset(data(_width + delta_x,y,z,c),0,-delta_x*sizeof(T));
            } else cimg_forYZC(*this,y,z,c) {
              std::memmove(data(delta_x,y,z,c),data(0,y,z,c),(_width-delta_x)*sizeof(T));
              std::memset(data(0,y,z,c),0,delta_x*sizeof(T));
            }
        }

      if (delta_y) // Shift along Y-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_y,height()), ndelta_y = (ml<=height()/2)?ml:(ml-height());
          if (!ndelta_y) return *this;
          CImg<T> buf(width(),cimg::abs(ndelta_y));
          if (ndelta_y>0) cimg_forZC(*this,z,c) {
              std::memcpy(buf,data(0,0,z,c),_width*ndelta_y*sizeof(T));
              std::memmove(data(0,0,z,c),data(0,ndelta_y,z,c),_width*(_height-ndelta_y)*sizeof(T));
              std::memcpy(data(0,_height-ndelta_y,z,c),buf,_width*ndelta_y*sizeof(T));
            } else cimg_forZC(*this,z,c) {
              std::memcpy(buf,data(0,_height + ndelta_y,z,c),-ndelta_y*_width*sizeof(T));
              std::memmove(data(0,-ndelta_y,z,c),data(0,0,z,c),_width*(_height + ndelta_y)*sizeof(T));
              std::memcpy(data(0,0,z,c),buf,-ndelta_y*_width*sizeof(T));
            }
        } break;
        case 1 : // Neumann
          if (delta_y<0) {
            const int ndelta_y = (-delta_y>=height())?height() - 1:-delta_y;
            if (!ndelta_y) return *this;
            cimg_forZC(*this,z,c) {
              std::memmove(data(0,0,z,c),data(0,ndelta_y,z,c),_width*(_height-ndelta_y)*sizeof(T));
              T *ptrd = data(0,_height-ndelta_y,z,c), *ptrs = data(0,_height - 1,z,c);
              for (int l = 0; l<ndelta_y - 1; ++l) { std::memcpy(ptrd,ptrs,_width*sizeof(T)); ptrd+=_width; }
            }
          } else {
            const int ndelta_y = (delta_y>=height())?height() - 1:delta_y;
            if (!ndelta_y) return *this;
            cimg_forZC(*this,z,c) {
              std::memmove(data(0,ndelta_y,z,c),data(0,0,z,c),_width*(_height-ndelta_y)*sizeof(T));
              T *ptrd = data(0,1,z,c), *ptrs = data(0,0,z,c);
              for (int l = 0; l<ndelta_y - 1; ++l) { std::memcpy(ptrd,ptrs,_width*sizeof(T)); ptrd+=_width; }
            }
          }
          break;
        default : // Dirichlet
          if (delta_y<=-height() || delta_y>=height()) return fill((T)0);
          if (delta_y<0) cimg_forZC(*this,z,c) {
              std::memmove(data(0,0,z,c),data(0,-delta_y,z,c),_width*(_height + delta_y)*sizeof(T));
              std::memset(data(0,_height + delta_y,z,c),0,-delta_y*_width*sizeof(T));
            } else cimg_forZC(*this,z,c) {
              std::memmove(data(0,delta_y,z,c),data(0,0,z,c),_width*(_height-delta_y)*sizeof(T));
              std::memset(data(0,0,z,c),0,delta_y*_width*sizeof(T));
            }
        }

      if (delta_z) // Shift along Z-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_z,depth()), ndelta_z = (ml<=depth()/2)?ml:(ml-depth());
          if (!ndelta_z) return *this;
          CImg<T> buf(width(),height(),cimg::abs(ndelta_z));
          if (ndelta_z>0) cimg_forC(*this,c) {
              std::memcpy(buf,data(0,0,0,c),_width*_height*ndelta_z*sizeof(T));
              std::memmove(data(0,0,0,c),data(0,0,ndelta_z,c),_width*_height*(_depth-ndelta_z)*sizeof(T));
              std::memcpy(data(0,0,_depth-ndelta_z,c),buf,_width*_height*ndelta_z*sizeof(T));
            } else cimg_forC(*this,c) {
              std::memcpy(buf,data(0,0,_depth + ndelta_z,c),-ndelta_z*_width*_height*sizeof(T));
              std::memmove(data(0,0,-ndelta_z,c),data(0,0,0,c),_width*_height*(_depth + ndelta_z)*sizeof(T));
              std::memcpy(data(0,0,0,c),buf,-ndelta_z*_width*_height*sizeof(T));
            }
        } break;
        case 1 : // Neumann
          if (delta_z<0) {
            const int ndelta_z = (-delta_z>=depth())?depth() - 1:-delta_z;
            if (!ndelta_z) return *this;
            cimg_forC(*this,c) {
              std::memmove(data(0,0,0,c),data(0,0,ndelta_z,c),_width*_height*(_depth-ndelta_z)*sizeof(T));
              T *ptrd = data(0,0,_depth-ndelta_z,c), *ptrs = data(0,0,_depth - 1,c);
              for (int l = 0; l<ndelta_z - 1; ++l) {
                std::memcpy(ptrd,ptrs,_width*_height*sizeof(T)); ptrd+=(ulongT)_width*_height;
              }
            }
          } else {
            const int ndelta_z = (delta_z>=depth())?depth() - 1:delta_z;
            if (!ndelta_z) return *this;
            cimg_forC(*this,c) {
              std::memmove(data(0,0,ndelta_z,c),data(0,0,0,c),_width*_height*(_depth-ndelta_z)*sizeof(T));
              T *ptrd = data(0,0,1,c), *ptrs = data(0,0,0,c);
              for (int l = 0; l<ndelta_z - 1; ++l) {
                std::memcpy(ptrd,ptrs,_width*_height*sizeof(T)); ptrd+=(ulongT)_width*_height;
              }
            }
          }
          break;
        default : // Dirichlet
          if (delta_z<=-depth() || delta_z>=depth()) return fill((T)0);
          if (delta_z<0) cimg_forC(*this,c) {
              std::memmove(data(0,0,0,c),data(0,0,-delta_z,c),_width*_height*(_depth + delta_z)*sizeof(T));
              std::memset(data(0,0,_depth + delta_z,c),0,_width*_height*(-delta_z)*sizeof(T));
            } else cimg_forC(*this,c) {
              std::memmove(data(0,0,delta_z,c),data(0,0,0,c),_width*_height*(_depth-delta_z)*sizeof(T));
              std::memset(data(0,0,0,c),0,delta_z*_width*_height*sizeof(T));
            }
        }

      if (delta_c) // Shift along C-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_c,spectrum()), ndelta_c = (ml<=spectrum()/2)?ml:(ml-spectrum());
          if (!ndelta_c) return *this;
          CImg<T> buf(width(),height(),depth(),cimg::abs(ndelta_c));
          if (ndelta_c>0) {
            std::memcpy(buf,_data,_width*_height*_depth*ndelta_c*sizeof(T));
            std::memmove(_data,data(0,0,0,ndelta_c),_width*_height*_depth*(_spectrum-ndelta_c)*sizeof(T));
            std::memcpy(data(0,0,0,_spectrum-ndelta_c),buf,_width*_height*_depth*ndelta_c*sizeof(T));
          } else {
            std::memcpy(buf,data(0,0,0,_spectrum + ndelta_c),-ndelta_c*_width*_height*_depth*sizeof(T));
            std::memmove(data(0,0,0,-ndelta_c),_data,_width*_height*_depth*(_spectrum + ndelta_c)*sizeof(T));
            std::memcpy(_data,buf,-ndelta_c*_width*_height*_depth*sizeof(T));
          }
        } break;
        case 1 : // Neumann
          if (delta_c<0) {
            const int ndelta_c = (-delta_c>=spectrum())?spectrum() - 1:-delta_c;
            if (!ndelta_c) return *this;
            std::memmove(_data,data(0,0,0,ndelta_c),_width*_height*_depth*(_spectrum-ndelta_c)*sizeof(T));
            T *ptrd = data(0,0,0,_spectrum-ndelta_c), *ptrs = data(0,0,0,_spectrum - 1);
            for (int l = 0; l<ndelta_c - 1; ++l) {
              std::memcpy(ptrd,ptrs,_width*_height*_depth*sizeof(T)); ptrd+=(ulongT)_width*_height*_depth;
            }
          } else {
            const int ndelta_c = (delta_c>=spectrum())?spectrum() - 1:delta_c;
            if (!ndelta_c) return *this;
            std::memmove(data(0,0,0,ndelta_c),_data,_width*_height*_depth*(_spectrum-ndelta_c)*sizeof(T));
            T *ptrd = data(0,0,0,1);
            for (int l = 0; l<ndelta_c - 1; ++l) {
              std::memcpy(ptrd,_data,_width*_height*_depth*sizeof(T)); ptrd+=(ulongT)_width*_height*_depth;
            }
          }
          break;
        default : // Dirichlet
          if (delta_c<=-spectrum() || delta_c>=spectrum()) return fill((T)0);
          if (delta_c<0) {
            std::memmove(_data,data(0,0,0,-delta_c),_width*_height*_depth*(_spectrum + delta_c)*sizeof(T));
            std::memset(data(0,0,0,_spectrum + delta_c),0,_width*_height*_depth*(-delta_c)*sizeof(T));
          } else {
            std::memmove(data(0,0,0,delta_c),_data,_width*_height*_depth*(_spectrum-delta_c)*sizeof(T));
            std::memset(_data,0,delta_c*_width*_height*_depth*sizeof(T));
          }
        }
      return *this;