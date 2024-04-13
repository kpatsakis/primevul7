    **/
    inline void strunescape(char *const str) {
#define cimg_strunescape(ci,co) case ci : *nd = co; ++ns; break;
      unsigned int val = 0;
      for (char *ns = str, *nd = str; *ns || (bool)(*nd=0); ++nd) if (*ns=='\\') switch (*(++ns)) {
            cimg_strunescape('a','\a');
            cimg_strunescape('b','\b');
            cimg_strunescape('e',0x1B);
            cimg_strunescape('f','\f');
            cimg_strunescape('n','\n');
            cimg_strunescape('r','\r');
            cimg_strunescape('t','\t');
            cimg_strunescape('v','\v');
            cimg_strunescape('\\','\\');
            cimg_strunescape('\'','\'');
            cimg_strunescape('\"','\"');
            cimg_strunescape('\?','\?');
          case 0 : *nd = 0; break;
          case '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' :
            cimg_sscanf(ns,"%o",&val); while (*ns>='0' && *ns<='7') ++ns;
            *nd = (char)val; break;
          case 'x' :
            cimg_sscanf(++ns,"%x",&val);
            while ((*ns>='0' && *ns<='9') || (*ns>='a' && *ns<='f') || (*ns>='A' && *ns<='F')) ++ns;
            *nd = (char)val; break;
          default : *nd = *(ns++);
          } else *nd = *(ns++);