    bool XmpParser::initialize(XmpParser::XmpLockFct xmpLockFct, void* pLockData)
    {
        if (!initialized_) {
            xmpLockFct_ = xmpLockFct;
            pLockData_ = pLockData;
            initialized_ = SXMPMeta::Initialize();
#ifdef EXV_ADOBE_XMPSDK
            SXMPMeta::RegisterNamespace("http://ns.adobe.com/lightroom/1.0/", "lr",nullptr);
            SXMPMeta::RegisterNamespace("http://rs.tdwg.org/dwc/index.htm", "dwc",nullptr);
            SXMPMeta::RegisterNamespace("http://purl.org/dc/terms/", "dcterms",nullptr);
            SXMPMeta::RegisterNamespace("http://www.digikam.org/ns/1.0/", "digiKam",nullptr);
            SXMPMeta::RegisterNamespace("http://www.digikam.org/ns/kipi/1.0/", "kipi",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.0/", "MicrosoftPhoto",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.acdsee.com/iptc/1.0/", "acdsee",nullptr);
            SXMPMeta::RegisterNamespace("http://iptc.org/std/Iptc4xmpExt/2008-02-29/", "iptcExt",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.useplus.org/ldf/xmp/1.0/", "plus",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.iview-multimedia.com/mediapro/1.0/", "mediapro",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/expressionmedia/1.0/", "expressionmedia",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.2/", "MP",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.2/t/RegionInfo#", "MPRI",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.2/t/Region#", "MPReg",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.google.com/photos/1.0/panorama/", "GPano",nullptr);
            SXMPMeta::RegisterNamespace("http://www.metadataworkinggroup.com/schemas/regions/", "mwg-rs",nullptr);
            SXMPMeta::RegisterNamespace("http://www.metadataworkinggroup.com/schemas/keywords/", "mwg-kw",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.adobe.com/xmp/sType/Area#", "stArea",nullptr);
            SXMPMeta::RegisterNamespace("http://cipa.jp/exif/1.0/", "exifEX",nullptr);
            SXMPMeta::RegisterNamespace("http://ns.adobe.com/camera-raw-saved-settings/1.0/", "crss",nullptr);
            SXMPMeta::RegisterNamespace("http://www.audio/", "audio",nullptr);
            SXMPMeta::RegisterNamespace("http://www.video/", "video",nullptr);
#else
            SXMPMeta::RegisterNamespace("http://ns.adobe.com/lightroom/1.0/", "lr");
            SXMPMeta::RegisterNamespace("http://rs.tdwg.org/dwc/index.htm", "dwc");
            SXMPMeta::RegisterNamespace("http://purl.org/dc/terms/", "dcterms");
            SXMPMeta::RegisterNamespace("http://www.digikam.org/ns/1.0/", "digiKam");
            SXMPMeta::RegisterNamespace("http://www.digikam.org/ns/kipi/1.0/", "kipi");
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.0/", "MicrosoftPhoto");
            SXMPMeta::RegisterNamespace("http://ns.acdsee.com/iptc/1.0/", "acdsee");
            SXMPMeta::RegisterNamespace("http://iptc.org/std/Iptc4xmpExt/2008-02-29/", "iptcExt");
            SXMPMeta::RegisterNamespace("http://ns.useplus.org/ldf/xmp/1.0/", "plus");
            SXMPMeta::RegisterNamespace("http://ns.iview-multimedia.com/mediapro/1.0/", "mediapro");
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/expressionmedia/1.0/", "expressionmedia");
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.2/", "MP");
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.2/t/RegionInfo#", "MPRI");
            SXMPMeta::RegisterNamespace("http://ns.microsoft.com/photo/1.2/t/Region#", "MPReg");
            SXMPMeta::RegisterNamespace("http://ns.google.com/photos/1.0/panorama/", "GPano");
            SXMPMeta::RegisterNamespace("http://www.metadataworkinggroup.com/schemas/regions/", "mwg-rs");
            SXMPMeta::RegisterNamespace("http://www.metadataworkinggroup.com/schemas/keywords/", "mwg-kw");
            SXMPMeta::RegisterNamespace("http://ns.adobe.com/xmp/sType/Area#", "stArea");
            SXMPMeta::RegisterNamespace("http://cipa.jp/exif/1.0/", "exifEX");
            SXMPMeta::RegisterNamespace("http://ns.adobe.com/camera-raw-saved-settings/1.0/", "crss");
            SXMPMeta::RegisterNamespace("http://www.audio/", "audio");
            SXMPMeta::RegisterNamespace("http://www.video/", "video");
#endif
        }
        return initialized_;
    }