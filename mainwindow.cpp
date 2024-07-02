#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    process = new QProcess(this);

    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_searchButton_clicked()
{
    QString command = ui->commandLineEdit->text();
    QString keyword = ui->keywordLineEdit->text();

    if (command.isEmpty() || keyword.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Por favor, rellena ambos campos");
        return;
    }

    // Configura el proceso para ejecutar el buscador
    QStringList arguments;
    arguments << command << keyword;
    process->start("buscador", arguments);
    process->waitForFinished();

    // Maneja la salida del proceso y muestra los resultados
    if (process->exitStatus() == QProcess::NormalExit) {
        QByteArray result = process->readAllStandardOutput();
        QString outputText = QString::fromLocal8Bit(result);
        // Puedes mostrar los resultados en un QTextEdit, por ejemplo
        ui->outputTextEdit->setText(outputText);
    } else {
        QMessageBox::warning(this, "Error", "Hubo un error al ejecutar el comando buscador");
    }
}