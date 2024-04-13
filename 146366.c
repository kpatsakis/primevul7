    void printNode(const std::string& schemaNs,
                   const std::string& propPath,
                   const std::string& propValue,
                   const XMP_OptionBits& opt)
    {
        static bool first = true;
        if (first) {
            first = false;
            std::cout << "ashisabsals\n"
                      << "lcqqtrgqlai\n";
        }
        enum { alia=0, sche, hasq, isqu, stru, arra,
               abag, aseq, aalt, lang, simp, len };

        std::string opts(len, '.');
        if (XMP_PropIsAlias(opt))       opts[alia] = 'X';
        if (XMP_NodeIsSchema(opt))      opts[sche] = 'X';
        if (XMP_PropHasQualifiers(opt)) opts[hasq] = 'X';
        if (XMP_PropIsQualifier(opt))   opts[isqu] = 'X';
        if (XMP_PropIsStruct(opt))      opts[stru] = 'X';
        if (XMP_PropIsArray(opt))       opts[arra] = 'X';
        if (XMP_ArrayIsUnordered(opt))  opts[abag] = 'X';
        if (XMP_ArrayIsOrdered(opt))    opts[aseq] = 'X';
        if (XMP_ArrayIsAlternate(opt))  opts[aalt] = 'X';
        if (XMP_ArrayIsAltText(opt))    opts[lang] = 'X';
        if (XMP_PropIsSimple(opt))      opts[simp] = 'X';

        std::cout << opts << " ";
        if (opts[sche] == 'X') {
            std::cout << "ns=" << schemaNs;
        }
        else {
            std::cout << propPath << " = " << propValue;
        }
        std::cout << std::endl;
    }