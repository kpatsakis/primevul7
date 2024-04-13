    Image::UniquePtr newWebPInstance(BasicIo::UniquePtr io, bool /*create*/)
    {
        Image::UniquePtr image(new WebPImage(std::move(io)));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }