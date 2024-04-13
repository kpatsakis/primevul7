    TaskFactory::TaskFactory()
        : registry_{
              {adjust, new Adjust}, {print, new Print},     {rename, new Rename},
              {erase, new Erase},   {extract, new Extract}, {insert, new Insert},
              {modify, new Modify}, {fixiso, new FixIso},   {fixcom, new FixCom},
          }
    {
    }