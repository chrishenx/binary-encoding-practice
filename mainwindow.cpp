#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "binaryencoder.h"

#include <QCheckBox>
#include <QDebug> // TODO Delete qDebug and its references when the project is ready

using namespace chrishenx;

static QString hex2bin(const QString& hex);

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  message = "";

  configureLineEditFonts();
  configureMethodCheckBoxes();
  configureCustomPlots();

#ifdef Q_OS_ANDROID

  configureForAndroid();

#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configureMethodCheckBoxes()
{
  methodCheckBoxes << ui->ttl_checkBox
                   << ui->nrzi_checkBox
                   << ui->nrzl_checkBox
                   << ui->bip_checkBox
                   << ui->pset_checkBox
                   << ui->manch_checkBox
                   << ui->manchd_checkBox
                   << ui->mlevel_checkBox;
  for (QCheckBox* checkBox : methodCheckBoxes)
  {
    connect(checkBox, &QCheckBox::toggled, [this, checkBox](bool toggled)
    {
      if (!toggled)
      {
          // Branch 1
          if (selectedCheckBoxes.contains(checkBox))
          {
              selectedCheckBoxes.removeOne(checkBox);
          }
      }
      else
      {
          // Branch 2
          if (selectedCheckBoxes.size() == ALLOWED_METHODS_CHECKED)
          {
              selectedCheckBoxes.front()->setChecked(false);
              // Executing Branch 1 concurrently on another QCheckBox
          }
          selectedCheckBoxes << checkBox;
      }
      if (checkBox == ui->mlevel_checkBox)
      {
        ui->levelsGroupBox->setEnabled(toggled);
      }
    });
  }
  ui->ttl_checkBox->click();
  ui->nrzi_checkBox->click();
  ui->nrzl_checkBox->click();
}

void MainWindow::configureLineEditFonts()
{
  QFont spacedFont = ui->messageLineEdit->font();
  spacedFont.setLetterSpacing(QFont::AbsoluteSpacing, 10);
  ui->messageLineEdit->setFont(spacedFont);
  spacedFont = ui->binaryMessageLineEdit->font();
  spacedFont.setLetterSpacing(QFont::AbsoluteSpacing, 8);
  ui->binaryMessageLineEdit->setFont(spacedFont);
}

void MainWindow::configureCustomPlots()
{
  ui->clockPlot->plotLayout()->insertRow(0);
  ui->clockPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->clockPlot, "Señal de reloj"));
  ui->clockPlot->setToolTip("Señal de reloj");
  QCPPlotTitle* plotTitle = (QCPPlotTitle*) ui->clockPlot->plotLayout()->element(0, 0);
  QFont titleFont = plotTitle->font();
  titleFont.setPointSize(11);
  plotTitle->setFont(titleFont);
  customPlots << ui->clockPlot
              << ui->cod1Plot
              << ui->cod2Plot
              << ui->cod3Plot;
  for (QCustomPlot* customPlot : customPlots)
  {
    QCPGraph* graph = customPlot->addGraph(0);
    QPen pen = graph->pen();
    pen.setWidthF(3.5);
    if (customPlot == ui->clockPlot)
    {
      pen.setColor(QColor(Qt::red));
    }
    else
    {
      customPlot->plotLayout()->insertRow(0);
      customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(customPlot, " "));
      plotTitle = (QCPPlotTitle*) customPlot->plotLayout()->element(0, 0);
      titleFont = plotTitle->font();
      titleFont.setPointSize(11);
      plotTitle->setFont(titleFont);
    }
    graph->setPen(pen);

#ifdef Q_OS_ANDROID
    QFont labelFont = customPlot->yAxis->tickLabelFont();
    labelFont.setPointSize(6);
    customPlot->yAxis->setTickLabelFont(labelFont);
    customPlot->xAxis->setTickLabelFont(labelFont);
#endif

  }
  customPlots.pop_front(); // Deleting clockPlot
}

static bool isHexadecimal(const QString& value)
{
  static const QString hexadecimalSymbols = "0123456789ABCDEF";
  for (const QChar& symbol : value)
  {
    if (!hexadecimalSymbols.contains(symbol, Qt::CaseInsensitive))
    {
      return false;
    }
  }
  return true;
}

void MainWindow::on_messageLineEdit_textEdited(const QString &input)
{
  if (input.isEmpty())
  {
    message = "";
    ui->binaryMessageLineEdit->setText(message);
  }
  else
  {
    if (!isHexadecimal(input))
    {
      QToolTip::showText(ui->messageLineEdit->mapToGlobal(QPoint(0, 0)),
                         "Solo digitos hexadecimales!");
      ui->messageLineEdit->setText(message);
    }
    else
    {
      message = input.toUpper();
      QToolTip::hideText();
    }
    ui->messageLineEdit->setText(message);
    ui->binaryMessageLineEdit->setText(hex2bin(message));
  }
}

void MainWindow::on_pushButton_clicked()
{
  if (selectedCheckBoxes.size() > ALLOWED_METHODS_CHECKED)
  { // TODO Delete this brach since never happens
    ui->statusBar->showMessage("Selecciona 3 métodos.", STATUS_BAR_MESSAGE_DURATION);
  }
  else
  {
    QString message = ui->binaryMessageLineEdit->text();
    if (message.isEmpty())
    {
      ui->statusBar->showMessage("Introduce tu mensage.", STATUS_BAR_MESSAGE_DURATION);
    }
    else
    {
      // All QCUstomPlots' graphs most exist at this point
      clearPlots();
      plotSelectedMethods();
    }
  }
}

void MainWindow::plotSelectedMethods()
{
  BinaryEncoder binaryEncoder(ui->binaryMessageLineEdit->text());

  static const double ZERO_LOWER = -0.09;
  const double SIGNAL_AMPLITUDE = binaryEncoder.amplitude() * 1.08;
  const int MSG_LENGHT = binaryEncoder.messageLength();

  // Ploting the reference clock signal
  ui->clockPlot->graph(0)->setData(binaryEncoder.generateClock());
  ui->clockPlot->xAxis->setRange(0, binaryEncoder.timeMax());
  ui->clockPlot->yAxis->setRange(ZERO_LOWER, SIGNAL_AMPLITUDE);
  ui->clockPlot->xAxis->setAutoTickCount(MSG_LENGHT - 1);
  ui->clockPlot->replot();
  auto customPlotIt = customPlots.begin();
  for (const QCheckBox* selectedCheckBox : selectedCheckBoxes)
  {
    QCustomPlot* customPlot = *customPlotIt;
    QCPPlotTitle* plotTitle = (QCPPlotTitle*) customPlot->plotLayout()->element(0, 0);
    if (selectedCheckBox == ui->ttl_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateTTL());
      customPlot->yAxis->setRange(ZERO_LOWER, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación TTL");
      plotTitle->setText("Codificación TTL");
    }
    else if (selectedCheckBox == ui->nrzl_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateNRZL());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación NRZ-L");
      plotTitle->setText("Codificación NRZ-L");
    }
    else if (selectedCheckBox == ui->nrzi_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateNRZI());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación NRZ-I");
      plotTitle->setText("Codificación NRZ-I");
    }
    else if (selectedCheckBox == ui->bip_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateBipolar());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación Bipolar");
      plotTitle->setText("Codificación Bipolar");
    }
    else if (selectedCheckBox == ui->pset_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generatePseudoternary());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación Pseudo-ternaria");
      plotTitle->setText("Codificación Pseudo-ternaria");
    }
    else if (selectedCheckBox == ui->manch_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateManchester());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación Manchester");
      plotTitle->setText("Codificación Manchester");
    }
    else if (selectedCheckBox == ui->manchd_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateDManchester());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación Manchester diferencial");
      plotTitle->setText("Codificación Manchester diferencial");
    }
    else if (selectedCheckBox == ui->mlevel_checkBox)
    {
      const int levels = ui->l2radioButton->isChecked() ? 2 :
                  ui->l4radioButton->isChecked() ? 4 : 8;
      customPlot->graph(0)->setData(binaryEncoder.generateMultilevel(levels));
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip(QString("Codificación de %1 niveles").arg(levels));
      plotTitle->setText(QString("Codificación de %1 niveles").arg(levels));
    }
    customPlot->xAxis->setRange(0, binaryEncoder.timeMax());
    customPlot->xAxis->setAutoTickCount(MSG_LENGHT - 1);
    customPlot->replot();
    customPlotIt++;
  }
}

void MainWindow::clearPlots()
{
  for (QCustomPlot* customPlot : customPlots) {
    customPlot->graph(0)->clearData();
    customPlot->replot();
    QCPPlotTitle* plotTitle = (QCPPlotTitle*) customPlot->plotLayout()->element(0, 0);
    plotTitle->setText("");
  }
}

#ifdef Q_OS_ANDROID

void MainWindow::configureForAndroid()
{
  ui->statusBar->setStyleSheet("QScrollBar:horizontal {height: 60px;}");
  ui->verticalSpacer->changeSize(20, 50);
  ui->verticalSpacer->invalidate();

  ui->centralWidget->layout()->setSpacing(2);
  ui->centralWidget->layout()->invalidate();


  // QRadioButtons configuration
  QList<QRadioButton*> radioButtons = {
    ui->l2radioButton, ui->l4radioButton, ui->l8radioButton
  };

  for (QRadioButton* radioButton : radioButtons) {
    QFont radioButtonFont = radioButton->font();
    radioButtonFont.setPointSizeF(7.6);
    radioButton->setFont(radioButtonFont);
    radioButton->setStyleSheet("QRadioButton::indicator { width:31px; height: 31px; }");
  }

  // QCheckBoxes configuration
  for (QCheckBox* checkBox : methodCheckBoxes) {
    checkBox->setStyleSheet("QCheckBox::indicator { width:31px; height: 31px; }");
    QFont checkBoxFont = checkBox->font();
    checkBoxFont.setPointSize(7);
    checkBox->setFont(checkBoxFont);
  }

  QGridLayout* groupBoxLayout = (QGridLayout*) ui->groupBox->layout();
  groupBoxLayout->setVerticalSpacing(6);
  groupBoxLayout->invalidate();

  ui->messageLineEdit->setMinimumHeight(30);
  ui->binaryMessageLineEdit->setMinimumHeight(30);

  ui->groupBox->setTitle("Métodos de codificación:");

}

#endif // Q_OS_ANDROID

// Local functions

QString hex2bin(const QString& hex)
{
  qlonglong binary = hex.toLongLong(nullptr, 16);
  QString strBinary = QString::number(binary, 2);
  int necesaryBIts = hex.length() * 4;
  while (strBinary.length() < necesaryBIts) {
    strBinary = "0" + strBinary;
  }
  return strBinary;
}
