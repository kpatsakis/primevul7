		map<string, string> readAll() {
			map<string, string> result;
			DIR *dir = opendir(path.c_str());
			ScopeGuard guard(boost::bind(doClosedir, dir));
			struct dirent *ent;

			while ((ent = readdir(dir)) != NULL) {
				if (ent->d_name[0] != '.') {
					try {
						result.insert(make_pair<string, string>(
							ent->d_name,
							Passenger::readAll(path + "/" + ent->d_name)));
					} catch (const SystemException &) {
						// Do nothing.
					}
				}
			}
			return result;
		}