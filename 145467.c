    static XMP_Status nsDumper
    ( void*           refCon
    , XMP_StringPtr   buffer
    , XMP_StringLen   bufferSize
    ) {
        XMP_Status result = 0 ;
        std::string out(buffer,bufferSize);

        // remove blanks
        // http://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
        std::string::iterator end_pos = std::remove(out.begin(), out.end(), ' ');
        out.erase(end_pos, out.end());

        bool bURI = out.find("http://") != std::string::npos   ;
        bool bNS  = out.find(':') != std::string::npos && !bURI;

        // pop trailing ':' on a namespace
        if ( bNS ) {
        std::size_t length = out.length();
            if ( out[length-1] == ':' ) out = out.substr(0,length-1);
        }

        if ( bURI || bNS ) {
            std::map<std::string,std::string>* p = (std::map<std::string,std::string>*) refCon;
            std::map<std::string,std::string>& m = *p;

            std::string b("");
            if ( bNS ) {  // store the NS in dict[""]
                m[b]=out;
            } else if ( m.find(b) != m.end() ) {  // store dict[uri] = dict[""]
                m[m[b]]=out;
                m.erase(b);
            }
        }
        return result;
    }