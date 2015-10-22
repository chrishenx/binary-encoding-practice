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
  ui->clockPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->clockPlot, "Señal de relog"));
  ui->clockPlot->setToolTip("Señal de relog");
  QCPPlotTitle* plotTitle = (QCPPlotTitle*) ui->clockPlot->plotLayout()->element(0, 0);
  QFont titleFont = plotTitle->font();
  titleFont.setPointSize(12);
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
      titleFont.setPointSize(12);
      plotTitle->setFont(titleFont);
    }
    graph->setPen(pen);
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
      ui->messageLineEdit->setText(message);
    }
    else
    {
      message = input.toUpper();
    }
    ui->messageLineEdit->setText(message);
    ui->binaryMessageLineEdit->setText(hex2bin(message));
  }
}

void MainWindow::on_pushButton_clicked()
{
  // TODO Allow user to select less than ALLOWED_METHODS_CHECKED
  if (selectedCheckBoxes.size() != ALLOWED_METHODS_CHECKED)
  {
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
      plotSelectedMethods();
    }
  }
}

void MainWindow::plotSelectedMethods()
{
  BinaryEncoder binaryEncoder(ui->binaryMessageLineEdit->text());

  const double SIGNAL_AMPLITUDE = binaryEncoder.amplitude();
  const int MSG_LENGHT = binaryEncoder.messageLength();

  // Ploting the reference clock signal
  ui->clockPlot->graph(0)->setData(binaryEncoder.generateClock());
  ui->clockPlot->xAxis->setRange(0, binaryEncoder.timeMax());
  ui->clockPlot->yAxis->setRange(0, SIGNAL_AMPLITUDE);
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
      customPlot->yAxis->setRange(0, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación TTL");
      plotTitle->setText("Codificación TTL");
    }
    else if (selectedCheckBox == ui->nrzl_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateNRZL());
      customPlot->yAxis->setRange(0, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación NRZ-L");
      plotTitle->setText("Codificación NRZ-L");
    }
    else if (selectedCheckBox == ui->nrzi_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateNRZI());
      customPlot->yAxis->setRange(0, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación NRZ-I");
      plotTitle->setText("Codificación NRZ-I");
    }
    else if (selectedCheckBox == ui->bip_checkBox)
    {
      customPlot->graph(0)->setData(binaryEncoder.generateBipolar());
      customPlot->yAxis->setRange(-SIGNAL_AMPLITUDE, SIGNAL_AMPLITUDE);
      customPlot->setToolTip("Codificación Bilineal");
      plotTitle->setText("Codificación Bilineal");
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

// Local functions

QString hex2bin(const QString& hex)
{
  int binary = hex.toInt(nullptr, 16);
  return QString::number(binary, 2);
}
