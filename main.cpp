#include <QApplication>

#include "cal.h"


// Crete calculator applicaton and make it visible.

int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     Cal *calculator = new Cal;
     calculator->show();
     return app.exec();
}
