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
                   << ui->mniv_checkBox;
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
  customPlots << ui->clockPlot
              << ui->cod1Plot
              << ui->cod2Plot
              << ui->cod3Plot;
  for (QCustomPlot* customPlot : customPlots)
  {
    auto graph = customPlot->addGraph(0);
    QPen pen = graph->pen();
    pen.setWidthF(3.5);
    if (customPlot == ui->clockPlot)
    {
      pen.setColor(QColor(Qt::red));
    }
    graph->setPen(pen);
  }

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
      BinaryEncoder binaryEncoder(ui->binaryMessageLineEdit->text());
      ui->clockPlot->graph(0)->setData(binaryEncoder.generateClock());
      ui->clockPlot->xAxis->setRange(0, binaryEncoder.timeMax());
      ui->clockPlot->yAxis->setRange(0, BinaryEncoder::DEFAULT_AMPLITUDE);
      ui->clockPlot->replot();

      ui->cod1Plot->graph(0)->setData(binaryEncoder.generateManchester());
      ui->cod1Plot->xAxis->setRange(0, binaryEncoder.timeMax());
      ui->cod1Plot->yAxis->setRange(-BinaryEncoder::DEFAULT_AMPLITUDE,
                                     BinaryEncoder::DEFAULT_AMPLITUDE);
      ui->cod1Plot->replot();

      ui->cod2Plot->graph(0)->setData(binaryEncoder.generateDManchester());
      ui->cod2Plot->xAxis->setRange(0, binaryEncoder.timeMax());
      ui->cod2Plot->yAxis->setRange(-BinaryEncoder::DEFAULT_AMPLITUDE,
                                     BinaryEncoder::DEFAULT_AMPLITUDE);
      ui->cod2Plot->replot();
    }
  }
}

// Local functions

QString hex2bin(const QString& hex)
{
  int binary = hex.toInt(nullptr, 16);
  return QString::number(binary, 2);
}
