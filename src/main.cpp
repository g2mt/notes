#include "notes/MainWindow.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

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
  parser.addPositionalArgument("file", "File(s) to open", "[file...]");
  parser.process(app);

  const QStringList files = parser.positionalArguments();

  MainWindow window(parser.value("directory"), files);
  window.show();

  return app.exec();
}
