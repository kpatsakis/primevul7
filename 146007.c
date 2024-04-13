    TiffDecoder::TiffDecoder(
        ExifData&            exifData,
        IptcData&            iptcData,
        XmpData&             xmpData,
        TiffComponent* const pRoot,
        FindDecoderFct       findDecoderFct
    )
        : exifData_(exifData),
          iptcData_(iptcData),
          xmpData_(xmpData),
          pRoot_(pRoot),
          findDecoderFct_(findDecoderFct),
          decodedIptc_(false)
    {
        assert(pRoot != 0);

        exifData_.clear();
        iptcData_.clear();
        xmpData_.clear();

        // Find camera make
        TiffFinder finder(0x010f, ifd0Id);
        pRoot_->accept(finder);
        TiffEntryBase* te = dynamic_cast<TiffEntryBase*>(finder.result());
        if (te && te->pValue()) {
            make_ = te->pValue()->toString();
        }
    }