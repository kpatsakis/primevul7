    LoaderNative::LoaderNative(PreviewId id, const Image &image, int parIdx)
        : Loader(id, image)
    {
        if (!(0 <= parIdx && static_cast<size_t>(parIdx) < image.nativePreviews().size())) return;
        nativePreview_ = image.nativePreviews()[parIdx];
        width_ = nativePreview_.width_;
        height_ = nativePreview_.height_;
        valid_ = true;
        if (nativePreview_.filter_ == "") {
            size_ = nativePreview_.size_;
        } else {
            size_ = getData().size_;
        }
    }