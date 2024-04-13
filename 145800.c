    int Print::run(const std::string& path)
    {
        try {
            path_ = path;
            int rc = 0;
            Exiv2::PrintStructureOption option = Exiv2::kpsNone;
            switch (Params::instance().printMode_) {
                case Params::pmSummary:
                    rc = Params::instance().greps_.empty() ? printSummary() : printList();
                    break;
                case Params::pmList:
                    rc = printList();
                    break;
                case Params::pmComment:
                    rc = printComment();
                    break;
                case Params::pmPreview:
                    rc = printPreviewList();
                    break;
                case Params::pmStructure:
                    rc = printStructure(std::cout, Exiv2::kpsBasic, path_);
                    break;
                case Params::pmRecursive:
                    rc = printStructure(std::cout, Exiv2::kpsRecursive, path_);
                    break;
                case Params::pmXMP:
                    if (option == Exiv2::kpsNone)
                        option = Exiv2::kpsXMP;
                    rc = setModeAndPrintStructure(option, path_);
                    break;
                case Params::pmIccProfile:
                    if (option == Exiv2::kpsNone)
                        option = Exiv2::kpsIccProfile;
                    rc = setModeAndPrintStructure(option, path_);
                    break;
            }
            return rc;
        } catch (const Exiv2::AnyError& e) {
            std::cerr << "Exiv2 exception in print action for file " << path << ":\n" << e << "\n";
            return 1;
        } catch (const std::overflow_error& e) {
            std::cerr << "std::overflow_error exception in print action for file " << path << ":\n" << e.what() << "\n";
            return 1;
        }
    }