int main(int argc, char* const argv[])
try {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " file\n";
        return 1;
    }

    const char* path = argv[1];
    mini1(path);
    mini9(path);

    return 0;
}