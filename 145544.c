    TiffBinaryArray::TiffBinaryArray(const TiffBinaryArray& rhs)
        : TiffEntryBase(rhs),
          cfgSelFct_(rhs.cfgSelFct_),
          arraySet_(rhs.arraySet_),
          arrayCfg_(rhs.arrayCfg_),
          arrayDef_(rhs.arrayDef_),
          defSize_(rhs.defSize_),
          setSize_(rhs.setSize_),
          origData_(rhs.origData_),
          origSize_(rhs.origSize_),
          pRoot_(rhs.pRoot_),
          decoded_(false)
    {
    }