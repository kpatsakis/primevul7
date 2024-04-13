        std::string readExiv2Config(const std::string& section,const std::string& value,const std::string& def)
        {
            std::string result = def;
            Exiv2::INIReader reader(Exiv2::Internal::getExiv2ConfigPath());

            if (reader.ParseError() == 0) {
                result = reader.Get(section,value,def);
            }
            return result;
        }