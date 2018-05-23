#include "QtdBase.h"
#include "QtdAppInfo.h"
#include "QtdApplication.h"

#include "QtdSync.BackupEngine.Rsync.h"
#include "QtdSync.Location.Rsync.h"

static QtdApplication::CommandLineArgumentConfig s_args[] = {
    { "-h", "--help", false, "show this information (no other options supported)" },
    { 0L, 0L, false, 0L } 
};

using namespace QtdSync;

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    int ret = 0;

    QCoreApplication app(argc, argv);
    RsyncBackupEngine rsync(&app);
    RsyncLocation loc = RsyncLocation::rsync("172.20.20.105", "rsync", "test", "test");

    QStringList modules = rsync.getRsyncModules(loc);
    //rsync.mkdir(loc, QString::fromLocal8Bit("testFölder"));
    rsync.rmdir(loc, QString::fromLocal8Bit("testFölder"));
    QtdFileEntryInfoList lst = rsync.dir(loc);

    return ret;
}
