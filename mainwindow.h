#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLinkedList>

class QCustomPlot;
class QCheckBox;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_messageLineEdit_textEdited(const QString &input);
  void on_pushButton_clicked();


private:
  Ui::MainWindow *ui;

  static const int ALLOWED_METHODS_CHECKED = 3; // Equal to the number of QCustomPlots
  static const int STATUS_BAR_MESSAGE_DURATION = 4000; // ms

  QLinkedList<QCheckBox*> methodCheckBoxes;
  QLinkedList<QCheckBox*> selectedCheckBoxes;
  QList<QCustomPlot*> customPlots;

  QString message;

  void configureMethodCheckBoxes();
  void configureLineEditFonts();
  void configureCustomPlots();
  void plotSelectedMethods();
  void clearPlots();
#ifdef Q_OS_ANDROID
  void configureForAndroid();
#endif
};

#endif // MAINWINDOW_H
