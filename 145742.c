    TiffEncoder::TiffEncoder(
            const ExifData&      exifData,
            const IptcData&      iptcData,
            const XmpData&       xmpData,
                  TiffComponent* pRoot,
            const bool           isNewImage,
            const PrimaryGroups* pPrimaryGroups,
            const TiffHeaderBase* pHeader,
                  FindEncoderFct findEncoderFct
    )
        : exifData_(exifData),
          iptcData_(iptcData),
          xmpData_(xmpData),
          del_(true),
          pHeader_(pHeader),
          pRoot_(pRoot),
          isNewImage_(isNewImage),
          pPrimaryGroups_(pPrimaryGroups),
          pSourceTree_(0),
          findEncoderFct_(findEncoderFct),
          dirty_(false),
          writeMethod_(wmNonIntrusive)
    {
        assert(pRoot != 0);
        assert(pPrimaryGroups != 0);
        assert(pHeader != 0);

        byteOrder_ = pHeader->byteOrder();
        origByteOrder_ = byteOrder_;

        encodeIptc();
        encodeXmp();

        // Find camera make
        ExifKey key("Exif.Image.Make");
        ExifData::const_iterator pos = exifData_.findKey(key);
        if (pos != exifData_.end()) {
            make_ = pos->toString();
        }
        if (make_.empty() && pRoot_) {
            TiffFinder finder(0x010f, ifd0Id);
            pRoot_->accept(finder);
            TiffEntryBase* te = dynamic_cast<TiffEntryBase*>(finder.result());
            if (te && te->pValue()) {
                make_ = te->pValue()->toString();
            }
        }
    }