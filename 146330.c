    void Modify::regNamespace(const ModifyCmd& modifyCmd)
    {
        if (Params::instance().verbose_) {
            std::cout << _("Reg ") << modifyCmd.key_ << "=\"" << modifyCmd.value_ << "\"" << std::endl;
        }
        Exiv2::XmpProperties::registerNs(modifyCmd.value_, modifyCmd.key_);
    }