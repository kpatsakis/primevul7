static void setup_user_data_dir()
{
#ifdef _WIN32
	_mkdir(user_data_dir.c_str());
	_mkdir((user_data_dir + "/editor").c_str());
	_mkdir((user_data_dir + "/editor/maps").c_str());
	_mkdir((user_data_dir + "/data").c_str());
	_mkdir((user_data_dir + "/data/add-ons").c_str());
	_mkdir((user_data_dir + "/saves").c_str());
	_mkdir((user_data_dir + "/persist").c_str());
#elif defined(__BEOS__)
	BPath tpath;
	#define BEOS_CREATE_PREFERENCES_SUBDIR(subdir) \
		tpath = be_path;                       \
		tpath.Append(subdir);                  \
		create_directory(tpath.Path(), 0775);

	BEOS_CREATE_PREFERENCES_SUBDIR("editor");
	BEOS_CREATE_PREFERENCES_SUBDIR("editor/maps");
	BEOS_CREATE_PREFERENCES_SUBDIR("data");
	BEOS_CREATE_PREFERENCES_SUBDIR("data/add-ons");
	BEOS_CREATE_PREFERENCES_SUBDIR("saves");
	BEOS_CREATE_PREFERENCES_SUBDIR("persist");
	#undef BEOS_CREATE_PREFERENCES_SUBDIR
#else
	const std::string& dir_path = user_data_dir;

	const bool res = create_directory_if_missing(dir_path);
	// probe read permissions (if we could make the directory)
	DIR* const dir = res ? opendir(dir_path.c_str()) : NULL;
	if(dir == NULL) {
		ERR_FS << "could not open or create preferences directory at " << dir_path << '\n';
		return;
	}
	closedir(dir);

	// Create user data and add-on directories
	create_directory_if_missing(dir_path + "/editor");
	create_directory_if_missing(dir_path + "/editor/maps");
	create_directory_if_missing(dir_path + "/data");
	create_directory_if_missing(dir_path + "/data/add-ons");
	create_directory_if_missing(dir_path + "/saves");
	create_directory_if_missing(dir_path + "/persist");
#endif
}