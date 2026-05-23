#include "notes/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("notes");
  app.setOrganizationName("QtNotes");
  app.setApplicationName("Notes");

  MainWindow window;
  window.show();

  return app.exec();
}
