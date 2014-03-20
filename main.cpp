#include <QtGui/QApplication>
#include "git_tutorial.h"
//#include "console.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    git_tutorial w;
    w.show();
    

    return a.exec();
}
