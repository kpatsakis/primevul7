    LoaderXmpJpeg::LoaderXmpJpeg(PreviewId id, const Image &image, int parIdx)
        : Loader(id, image)
    {
        (void)parIdx;

        const XmpData &xmpData = image_.xmpData();

        std::string prefix = "xmpGImg";
        if (xmpData.findKey(XmpKey("Xmp.xmp.Thumbnails[1]/xapGImg:image")) != xmpData.end()) {
            prefix = "xapGImg";
        }

        XmpData::const_iterator imageDatum = xmpData.findKey(XmpKey("Xmp.xmp.Thumbnails[1]/" + prefix + ":image"));
        if (imageDatum == xmpData.end()) return;
        XmpData::const_iterator formatDatum = xmpData.findKey(XmpKey("Xmp.xmp.Thumbnails[1]/" + prefix + ":format"));
        if (formatDatum == xmpData.end()) return;
        XmpData::const_iterator widthDatum = xmpData.findKey(XmpKey("Xmp.xmp.Thumbnails[1]/" + prefix + ":width"));
        if (widthDatum == xmpData.end()) return;
        XmpData::const_iterator heightDatum = xmpData.findKey(XmpKey("Xmp.xmp.Thumbnails[1]/" + prefix + ":height"));
        if (heightDatum == xmpData.end()) return;

        if (formatDatum->toString() != "JPEG") return;

        width_ = widthDatum->toLong();
        height_ = heightDatum->toLong();
        preview_ = decodeBase64(imageDatum->toString());
        size_ = static_cast<uint32_t>(preview_.size_);
        valid_ = true;
    }