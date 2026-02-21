#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <private/qzipwriter_p.h>


void ArchiveWorker::process() {
    QFile zipFile(m_destPath);

    //перевірка на існування старого файлу з таким же ім'ям
    if (zipFile.exists()) {
        if (!zipFile.remove()) {
            emit finished(false, "Не вдалося перезаписати існуючий файл.");
            return;
        }
    }

    //створюємо об'єкт для запису ZIP-архіву
    QZipWriter writer(m_destPath);
    if (writer.status() != QZipWriter::NoError) {
        emit finished(false, "Не вдалося створити файл архіву.");
        return;
    }

    int totalFiles = m_files.count();

    //цикл проходить по всіх обраних файлах
    //беремо шлях -> Відкриваємо файл -> Читаємо дані -> Додаємо в архів -> Рахуємо прогрес
    for (int i = 0; i < totalFiles; ++i) {
        QString filePath = m_files[i];
        QFile file(filePath);


        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            QString fileName = QFileInfo(filePath).fileName();

            writer.addFile(fileName, data);
            file.close();
        }

        //(Поточний індекс + 1) ділимо на загальну кількість, множимо на 100 і приводимо до цілого числа
        int percent = static_cast<int>(((double)(i + 1) / totalFiles) * 100);
        emit progressChanged(percent);
    }

    writer.close();
    emit finished(true, "Готово! Архів успішно створено.");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setupUi();
    applyStyles();
    updateButtonsState();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setMinimumSize(600, 500);
    setWindowTitle("Створення ZIP-архіву (Qt C++)");


    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // заголовки і кнопки
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *lblTitle = new QLabel("Ваші файли");
    lblTitle->setObjectName("HeaderTitle");

    btnClearFiles = new QPushButton("Очистити");
    btnClearFiles->setObjectName("ClearButton");

    btnSelectFiles = new QPushButton("Додати файли");

    headerLayout->addWidget(lblTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(btnClearFiles);
    headerLayout->addWidget(btnSelectFiles);
    mainLayout->addLayout(headerLayout);

    //Список файлів
    lbFiles = new QListWidget();
    mainLayout->addWidget(lbFiles);

    //назва архіву
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *lblName = new QLabel("Назва архіву:");
    lblName->setObjectName("LabelNormal");
    txtArchiveName = new QLineEdit();
    txtArchiveName->setPlaceholderText("Залиште пустим для назви за замовчуванням");
    txtArchiveName->setFixedWidth(250);

    nameLayout->addWidget(lblName);
    nameLayout->addWidget(txtArchiveName);
    nameLayout->addStretch();
    mainLayout->addLayout(nameLayout);

    //кнопки збереження і прогресбар
    QHBoxLayout *actionLayout = new QHBoxLayout();
    btnCreateArchive = new QPushButton("Зберегти архів...");
    btnCreateArchive->setObjectName("PrimaryButton");
    btnCreateArchive->setFixedWidth(180);

    pbProgress = new QProgressBar();
    pbProgress->setTextVisible(false);
    pbProgress->setFixedHeight(6);
    pbProgress->setRange(0, 100);
    pbProgress->setValue(0);
    pbProgress->hide(); //ховаємо прогрес-бар, поки не почалась архівація

    actionLayout->addWidget(btnCreateArchive);
    actionLayout->addWidget(pbProgress);
    mainLayout->addLayout(actionLayout);

    //статус
    txtStatus = new QLabel("");
    txtStatus->setObjectName("StatusText");
    mainLayout->addWidget(txtStatus);

    //Підключення кліків по кнопках до методів обробки
    connect(btnSelectFiles, &QPushButton::clicked, this, &MainWindow::onSelectFiles);
    connect(btnClearFiles, &QPushButton::clicked, this, &MainWindow::onClearFiles);
    connect(btnCreateArchive, &QPushButton::clicked, this, &MainWindow::onCreateArchive);
}


void MainWindow::applyStyles() {
    //Dark Mode
    this->setStyleSheet(R"(
        QMainWindow { background-color: #131314; }
        QLabel { color: #E3E3E3; font-family: 'Segoe UI'; }
        QLabel#HeaderTitle { font-size: 20px; font-weight: bold; }
        QLabel#LabelNormal { color: #C4C7C5; font-size: 14px; }
        QLabel#StatusText { color: #C4C7C5; font-size: 14px; }
        QListWidget { background-color: #1E1F22; border-radius: 8px; color: #C4C7C5; font-size: 14px; border: none; padding: 5px; }
        QLineEdit { background-color: #1E1F22; color: #E3E3E3; border-radius: 6px; padding: 8px; font-size: 14px; border: none; }
        QPushButton { background-color: #1E1F22; color: #E3E3E3; border-radius: 10px; padding: 10px 16px; font-size: 14px; font-weight: bold; border: none; }
        QPushButton:hover { background-color: #333538; }
        QPushButton:disabled { opacity: 100; color: #555555; background-color: #18191b; }
        QPushButton#PrimaryButton { background-color: #A8C7FA; color: #041E49; }
        QPushButton#PrimaryButton:hover { background-color: #D3E3FD; }
        QPushButton#ClearButton { background-color: #1E1F22; color: #F28B82; }
        QPushButton#ClearButton:hover { background-color: #5C2B29; }
        QProgressBar { background-color: #1E1F22; border-radius: 3px; }
        QProgressBar::chunk { background-color: #A8C7FA; border-radius: 3px; }
    )");
}

void MainWindow::onSelectFiles() {
    //стандартне вікно вибору файлів ОС
    QStringList files = QFileDialog::getOpenFileNames(this, "Оберіть файли", QDir::homePath());

    if (!files.isEmpty()) {
        for (const QString &path : files) {
            //перевіряємо, чи немає вже цього файлу в списку, щоб уникнути дублікатів
            if (!selectedFilePaths.contains(path)) {
                selectedFilePaths.append(path);
                lbFiles->addItem(QFileInfo(path).fileName());
            }
        }
        txtStatus->setText(QString("Обрано файлів: %1").arg(selectedFilePaths.count()));
        updateButtonsState(); //оновлюємо стан кнопок
    }
}

void MainWindow::onClearFiles() {
    //повністю очищає списки як у пам'яті (selectedFilePaths), так і візуальні
    selectedFilePaths.clear();
    lbFiles->clear();
    txtStatus->setText("Список файлів очищено");
    updateButtonsState();
}

void MainWindow::updateButtonsState() {
    //робимо кнопки неактивними, якщо список файлів порожній
    bool hasFiles = !selectedFilePaths.isEmpty();
    btnCreateArchive->setEnabled(hasFiles);
    btnClearFiles->setEnabled(hasFiles);
}

void MainWindow::onCreateArchive() {
    QString archiveName = txtArchiveName->text().trimmed();
    if (archiveName.isEmpty()) archiveName = "DefaultArchive";

    //pабезпечуємо наявність розширення .zip, незалежно від регістру
    if (!archiveName.endsWith(".zip", Qt::CaseInsensitive)) archiveName += ".zip";

    //запитуємо у користувача місце для збереження
    QString savePath = QFileDialog::getSaveFileName(this, "Зберегти архів як...", archiveName, "ZIP Archive (*.zip)");
    if (savePath.isEmpty()) return; //якщо користувач натиснув "Скасувати"

    //не можна дозволяти користувачу додавати нові файли або знову натискати "Зберегти", поки йде процес
    btnSelectFiles->setEnabled(false);
    btnCreateArchive->setEnabled(false);
    btnClearFiles->setEnabled(false);

    pbProgress->setValue(0);
    pbProgress->setVisible(true);
    txtStatus->setText("Архівація в процесі...");

    //багатопотоковість
    QThread* thread = new QThread;
    ArchiveWorker* worker = new ArchiveWorker(selectedFilePaths, savePath);

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &ArchiveWorker::process);

    connect(worker, &ArchiveWorker::progressChanged, this, &MainWindow::onArchiveProgress);
    connect(worker, &ArchiveWorker::finished, this, &MainWindow::onArchiveFinished);

    connect(worker, &ArchiveWorker::finished, thread, &QThread::quit);

    connect(worker, &ArchiveWorker::finished, worker, &ArchiveWorker::deleteLater);

    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

void MainWindow::onArchiveProgress(int percent) {
    pbProgress->setValue(percent);
}

void MainWindow::onArchiveFinished(bool success, QString message) {
    if (success) {
        txtStatus->setText(message);
        txtStatus->setStyleSheet("color: #81C995");

        selectedFilePaths.clear();
        lbFiles->clear();
        txtArchiveName->clear();

        QTimer::singleShot(1500, [this](){ pbProgress->setVisible(false); });
    } else {
        txtStatus->setText("Помилка: " + message);
        txtStatus->setStyleSheet("color: #F28B82");
    }

    btnSelectFiles->setEnabled(true);
    updateButtonsState();
}
