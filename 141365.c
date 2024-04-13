	bool isAbsolutePath(const StaticString &path) const {
		if (path.empty() || path[0] != '/') {
			return false;
		} else {
			vector<string> components;
			string component;

			split(path, '/', components);
			components.erase(components.begin());
			foreach (component, components) {
				if (component.empty() || component == "." || component == "..") {
					return false;
				}
			}
			return true;
		}
	}