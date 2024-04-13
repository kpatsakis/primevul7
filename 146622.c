    } catch (const Exiv2::AnyError& e) {
        std::cerr << "Exiv2 exception in insert action for file " << path << ":\n" << e << "\n";
        return 1;
    }