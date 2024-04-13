qemuProcessReconnectAll(virQEMUDriverPtr driver)
{
    struct qemuProcessReconnectData data = {.driver = driver};
    virDomainObjListForEach(driver->domains, true,
                            qemuProcessReconnectHelper, &data);
}