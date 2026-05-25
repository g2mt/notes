#include "notes/MainWindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("notes");
  app.setOrganizationName("QtNotes");
  app.setApplicationName("Notes");

  QCommandLineParser parser;
  parser.setApplicationDescription("Qt-based notes editor");
  parser.addHelpOption();
  parser.addOption(QCommandLineOption(
      QStringList() << "d" << "directory",
      "Open <directory> instead of the current working directory.",
      "directory"));
  parser.process(app);

  MainWindow window(parser.value("directory"));
  window.show();

  return app.exec();
}
