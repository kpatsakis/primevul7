    void Extract::writePreviewFile(const Exiv2::PreviewImage& pvImg, int num) const
    {
        std::string pvFile = newFilePath(path_, "-preview") + Exiv2::toString(num);
        std::string pvPath = pvFile + pvImg.extension();
        if (dontOverwrite(pvPath))
            return;
        if (Params::instance().verbose_) {
            std::cout << _("Writing preview") << " " << num << " (" << pvImg.mimeType() << ", ";
            if (pvImg.width() != 0 && pvImg.height() != 0) {
                std::cout << pvImg.width() << "x" << pvImg.height() << " " << _("pixels") << ", ";
            }
            std::cout << pvImg.size() << " " << _("bytes") << ") " << _("to file") << " " << pvPath << std::endl;
        }
        long rc = pvImg.writeFile(pvFile);
        if (rc == 0) {
            std::cerr << path_ << ": " << _("Image does not have preview") << " " << num << "\n";
        }
    }