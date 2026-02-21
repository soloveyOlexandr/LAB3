#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QThread>

//Клас для фонової архівації
class ArchiveWorker : public QObject {
    Q_OBJECT
public:
    //конструктор приймає список файлів та шлях куди зберегти архів.
    ArchiveWorker(const QStringList &files, const QString &destPath)
        : m_files(files), m_destPath(destPath) {}

public slots:
    //Головний метод, який виконуватиме архівацію
    void process();

signals:
    // progressChanged передає відсоток виконання, а finished повідомляє про кінець та передає текст результату
    void progressChanged(int percent);
    void finished(bool success, QString message);

private:
    QStringList m_files;
    QString m_destPath;
};

//Головне вікно
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //реакції на натискання кнопок користувачем
    void onSelectFiles();
    void onClearFiles();
    void onCreateArchive();

    //прийом сигналів від об'єкта ArchiveWorker
    void onArchiveProgress(int percent);
    void onArchiveFinished(bool success, QString message);

private:
    void setupUi();             //створення кнопок/полів
    void applyStyles();         //темна тема
    void updateButtonsState();  //увімкнення/вимкнення кнопок залежно від наявності обраних файлів

    //Елементи інтерфейсу
    QListWidget *lbFiles;
    QPushButton *btnSelectFiles;
    QPushButton *btnClearFiles;
    QPushButton *btnCreateArchive;
    QLineEdit *txtArchiveName;
    QProgressBar *pbProgress;
    QLabel *txtStatus;

    //зберігає повні шляхи до обраних користувачем файлів
    QStringList selectedFilePaths;
};

#endif
