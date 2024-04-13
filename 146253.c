    TiffBinaryArray::TiffBinaryArray(uint16_t tag,
                                     IfdId group,
                                     const ArrayCfg* arrayCfg,
                                     const ArrayDef* arrayDef,
                                     int defSize)
        : TiffEntryBase(tag, group, arrayCfg->elTiffType_),
          cfgSelFct_(0),
          arraySet_(0),
          arrayCfg_(arrayCfg),
          arrayDef_(arrayDef),
          defSize_(defSize),
          setSize_(0),
          origData_(0),
          origSize_(0),
          pRoot_(0),
          decoded_(false)
    {
        assert(arrayCfg != 0);
    }