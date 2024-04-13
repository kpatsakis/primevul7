    void Print::printLabel(const std::string& label) const
    {
        std::cout << std::setfill(' ') << std::left;
        if (Params::instance().files_.size() > 1) {
            std::cout << std::setw(20) << path_ << " ";
        }
        std::cout << std::make_pair(label, align_) << ": ";
    }