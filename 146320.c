    bool cmpPreviewProperties(
        const PreviewProperties& lhs,
        const PreviewProperties& rhs
    )
    {
        uint32_t l = lhs.width_ * lhs.height_;
        uint32_t r = rhs.width_ * rhs.height_;

        return l < r;
    }