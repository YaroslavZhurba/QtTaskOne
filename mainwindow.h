#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <memory>

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);
    ~main_window();

private:
    struct MyFile {
        QString absolutePath, name;
        QByteArray hash;
        long long size;
    };
private slots:
    void select_directory();
    void recursive_scan(QString const& dir, long long *p, std::vector<MyFile> & arrayOfHashes);
    void scan_directory(QString const& dir);
    void show_about_dialog();

private:
    std::unique_ptr<Ui::MainWindow> ui;

};

#endif // MAINWINDOW_H
