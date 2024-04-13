    bool convertStringCharsetWindows(std::string& str, const char* from, const char* to)
    {
        bool ret = false;
        const ConvFctList* p = find(convFctList, std::make_pair(from, to));
        std::string tmpstr = str;
        if (p) ret = p->convFct_(tmpstr);
#ifndef SUPPRESS_WARNINGS
        else {
            EXV_WARNING << "No Windows function to map character string from " << from << " to " << to << " available.\n";
        }
#endif
        if (ret) str = tmpstr;
        return ret;
    }