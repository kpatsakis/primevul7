qemuProcessCreatePretendCmdBuild(virQEMUDriverPtr driver,
                                 virDomainObjPtr vm,
                                 const char *migrateURI,
                                 bool enableFips,
                                 bool standalone,
                                 bool jsonPropsValidation)
{
    unsigned int buildflags = 0;

    if (jsonPropsValidation)
        buildflags = QEMU_BUILD_COMMANDLINE_VALIDATE_KEEP_JSON;

    VIR_DEBUG("Building emulator command line");
    return qemuBuildCommandLine(driver,
                                NULL,
                                driver->securityManager,
                                vm,
                                migrateURI,
                                NULL,
                                VIR_NETDEV_VPORT_PROFILE_OP_NO_OP,
                                standalone,
                                enableFips,
                                NULL,
                                NULL,
                                buildflags);
}