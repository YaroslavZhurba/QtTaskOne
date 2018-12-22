#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <algorithm>

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QtWidgets>
#include <iostream>
#include <QCryptographicHash>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);

    QString a = "239";
    QString b = "228";
    QString c = "239";
    if (a > b) {
        std::cout << 1 << std::endl;
    } else {
        std::cout << 0 << std::endl;
    }

    if (a == c) {
        std::cout << 1 << std::endl;
    } else {
        std::cout << 0 << std::endl;
    }

//    scan_directory(QDir::homePath());
}

main_window::~main_window()
{}

void main_window::select_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    scan_directory(dir);
}

void main_window::recursive_scan(QString const& dir, long long *p, std::vector<MyFile > & arrayOfHashes) {
    QDir d(dir);
    QFileInfoList list = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFile file;
    for (QFileInfo file_info : list) {
//        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
//        item->setText(0, file_info.fileName());
//        item->setText(1, QString::number(file_info.size()));
//        ui->treeWidget->addTopLevelItem(item);
        QString Path = file_info.absoluteFilePath();  //Gets the absolute file path
                if(file_info.isDir()) {
//                    item->setText(1, dir);
                    recursive_scan(Path, p, arrayOfHashes);
                } else {
                    //Do stuff with the found file.
                    *p = *p + 1;
                    file.setFileName(file_info.absoluteFilePath());
                    if (file.open(QIODevice::ReadOnly)) {
                         QCryptographicHash hash(QCryptographicHash::Sha3_256);

//                         QByteArray hashByte;
                         hash.addData(&file);
//                         QString hashString(hash.result());
                         MyFile myFile;
                         myFile.absolutePath = file_info.absoluteFilePath();
                         myFile.size = file_info.size();
                         myFile.name = file_info.fileName();
                         myFile.hash = hash.result();
                         arrayOfHashes.push_back(myFile);
//                         item->setText(1, hashString);

                    } else {
//                         item->setText(1,"Error");
                    }
                    file.close();

                }
    }
}

void main_window::scan_directory(QString const& dir)
{
    long long *count = new long long;
    *count = 0;
    std::vector<MyFile> arrayOfHashes;
    std::vector<std::vector<MyFile>> sizeBuckets;
    ui->treeWidget->clear();
    setWindowTitle(QString("Directory Content - %1").arg(dir));
    recursive_scan(dir,count,arrayOfHashes);
    QString message = QString::number(*count);
    ui->statusBar->showMessage(message);
    sort(arrayOfHashes.begin(), arrayOfHashes.end(), [](MyFile a, MyFile b) {
        return a.size < b.size;
    });
    int j = 0;
    std::vector<MyFile> tmp;
    sizeBuckets.push_back(tmp);
    sizeBuckets[j].push_back(arrayOfHashes[0]);
    for (int i = 1; i < arrayOfHashes.size(); i++) {
        if (arrayOfHashes[i].size != arrayOfHashes[i-1].size) {
            j++;
            sizeBuckets.push_back(tmp);
        }
        sizeBuckets[j].push_back(arrayOfHashes[i]);
    }

    for (int i = 0; i < sizeBuckets.size(); i++) {
        int l,r;
        l = 0;
        r = 1;
        while (l < sizeBuckets[i].size()) {
            int cnt = 1;
            while (r < sizeBuckets[i].size() && sizeBuckets[i][r].hash == sizeBuckets[i][r-1].hash) {
                r++;
                cnt++;
            }
            for (int j = l; j < r; j++) { // copies name path size
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
                int ind = j - l + 1;
                QString text = QString::number(ind) + "/" + QString::number(cnt);
                MyFile myFile = sizeBuckets[i][j];
                item->setText(0, text);
                item->setText(1, myFile.name);
                item->setText(2, myFile.absolutePath);
                item->setText(3, QString::number(myFile.size));
            }
            l = r;
            r++;
        }
    }

}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}
