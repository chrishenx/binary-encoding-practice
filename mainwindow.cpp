#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>

#include <QDebug>

static QString hex2bin(const QString& hex);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    for (QCheckBox* checkBox : methodCheckBoxes) {
        connect(checkBox, &QCheckBox::toggled, [this, checkBox](bool toggled) {
            if (!toggled) {
                // Branch 1
                if (selectedCheckBoxes.contains(checkBox)) {
                    selectedCheckBoxes.removeOne(checkBox);
                }
            } else {
                // Branch 2
                if (selectedCheckBoxes.size() == allowedMethodsChecked) {
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
    spacedFont.setLetterSpacing(QFont::AbsoluteSpacing, 10);
    ui->binaryMessageLineEdit->setFont(spacedFont);
}

void MainWindow::configureCustomPlots()
{
    customPlots << ui->clockPlot
                << ui->cod1Plot
                << ui->cod2Plot
                << ui->cod3Plot;
}

void MainWindow::on_messageLineEdit_textEdited(const QString &input)
{
    static const QString hexadecimalSymbols = "0123456789ABCDEF";
    if (input.isEmpty()) {
        ui->binaryMessageLineEdit->setText("");
    } else {
        const int lastIndex = input.length() - 1;
        if (!hexadecimalSymbols.contains(input[lastIndex], Qt::CaseInsensitive)) {
            ui->messageLineEdit->setText(input.left(lastIndex));
        } else if (input[lastIndex].isLower()) {
            ui->messageLineEdit->setText(input.toUpper());
        }
        ui->binaryMessageLineEdit->setText(hex2bin(ui->messageLineEdit->text()));
    }
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << selectedCheckBoxes.size();
    if (selectedCheckBoxes.size() != allowedMethodsChecked) {
        ui->statusBar->showMessage("Selecciona 3 métodos por lo menos", statusBarMessageDuration);
    }
}

// Local functions

QString hex2bin(const QString& hex) {
    int binary = hex.toInt(nullptr, 16);
    return QString::number(binary, 2);
}