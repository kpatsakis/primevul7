	Spawner(const ConfigPtr &_config)
		: config(_config),
		  creationTime(SystemTime::getUsec())
		{ }