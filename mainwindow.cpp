#include "mainwindow.h"
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>

#define INDEX_FILE QString("%1/note_index.txt").arg(SAVE_PATH)

// --- StickyNote ---

StickyNote::StickyNote(const QString &name, QWidget *parent) : QWidget(parent) {
    setWindowTitle(name);
    resize(250, 250);

    QMenuBar *menuBar = new QMenuBar(this);
    QAction *saveAction = new QAction("Save", this);
    QAction *deleteAction = new QAction("Delete", this);
    QAction *renameAction = new QAction("Rename", this);

    QMenu *fileMenu = menuBar->addMenu("File");
    fileMenu->addAction(saveAction);
    fileMenu->addAction(deleteAction);
    fileMenu->addAction(renameAction);

    editor = new QTextEdit(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMenuBar(menuBar);
    layout->addWidget(editor);
    setLayout(layout);

    connect(saveAction, &QAction::triggered, this, &StickyNote::saveNote);
    connect(deleteAction, &QAction::triggered, this, &StickyNote::deleteNote);
    connect(renameAction, &QAction::triggered, this, [this]() {
        bool ok;
        QString text = QInputDialog::getText(this, "Rename Note", "New name:", QLineEdit::Normal, windowTitle(), &ok);
        if(ok && !text.isEmpty()) renameNote(text);
    });
}

QString StickyNote::currentFileName() const {
    QString safeTitle = windowTitle();
    safeTitle.replace(" ", "_").replace("#", "_");
    return QString("%1/%2.txt").arg(SAVE_PATH).arg(safeTitle);
}

void StickyNote::renameNote(const QString &newName) {
    QString oldFile = currentFileName();
    setWindowTitle(newName);
    QString newFile = currentFileName();

    if(QFile::exists(oldFile)) {
        QFile::rename(oldFile, newFile);
    }
}

void StickyNote::saveNote() {
    QDir dir(SAVE_PATH);
    if (!dir.exists()) dir.mkpath(".");

    QFile file(currentFileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not save note to:\n" + currentFileName());
        return;
    }

    QTextStream out(&file);
    out << editor->toPlainText();
    file.close();
}

void StickyNote::deleteNote() {
    QString filePath = currentFileName();
    if(QFile::exists(filePath)) QFile::remove(filePath);

    emit noteDeleted(this);
    close();
}

// --- MainWindow ---

MainWindow::MainWindow() {
    setWindowTitle("Sticky Notes");

    notesMenu = menuBar()->addMenu("Menu");

    QAction *createNoteAction = new QAction("Create Note", this);
    notesMenu->addAction(createNoteAction);

    QAction *viewNotesAction = new QAction("View Notes", this);
    notesMenu->addAction(viewNotesAction);

    connect(createNoteAction, &QAction::triggered, this, &MainWindow::createNote);
    connect(viewNotesAction, &QAction::triggered, this, &MainWindow::updateNotesMenu);

    // Load last note index
    QFile idxFile(INDEX_FILE);
    if(idxFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&idxFile);
        in >> noteCounter;
        idxFile.close();
    } else {
        noteCounter = 0;
    }
}

void MainWindow::createNote() {
    if(notes.size() >= MAX_NOTES) {
        QMessageBox::warning(this, "Notes full", "Notes full. Delete a note to create new ones.");
        return;
    }

    noteCounter++;
    QString name = QString("Note #%1").arg(noteCounter);
    StickyNote *note = new StickyNote(name);
    note->setAttribute(Qt::WA_DeleteOnClose);
    notes.append(note);
    connect(note, &StickyNote::noteDeleted, this, &MainWindow::handleNoteDeleted);
    note->show();

    saveNoteCounter();
    note->saveNote();
}

void MainWindow::updateNotesMenu() {
    QMenu *oldViewMenu = notesMenu->findChild<QMenu*>("ViewNotesMenu");
    if(oldViewMenu) {
        notesMenu->removeAction(oldViewMenu->menuAction());
        delete oldViewMenu;
    }

    QMenu *viewMenu = notesMenu->addMenu("View Notes");
    viewMenu->setObjectName("ViewNotesMenu");

    QDir dir(SAVE_PATH);
    QStringList noteFiles = dir.entryList(QStringList() << "*.txt", QDir::Files);

    for(const QString &fileName : noteFiles) {
        QString noteName = fileName;
        noteName.chop(4); // remove .txt
        QAction *noteAction = new QAction(noteName, this);
        viewMenu->addAction(noteAction);
        connect(noteAction, &QAction::triggered, this, [this, fileName]() {
            QFile file(QDir(SAVE_PATH).filePath(fileName));
            if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();
                file.close();

                QString noteName = fileName;
                noteName.chop(4);
                StickyNote *note = new StickyNote(noteName);
                note->getEditor()->setPlainText(content);
                note->setAttribute(Qt::WA_DeleteOnClose);
                notes.append(note);
                connect(note, &StickyNote::noteDeleted, this, &MainWindow::handleNoteDeleted);
                note->show();
            }
        });
    }
}

void MainWindow::handleNoteDeleted(StickyNote* note) {
    notes.removeOne(note);
    reindexNotes();
}

void MainWindow::reindexNotes() {
    noteCounter = 0;
    for(int i = 0; i < notes.size(); ++i) {
        noteCounter++;
        QString newTitle = QString("Note #%1").arg(noteCounter);
        notes[i]->renameNote(newTitle); // now properly renames file too
        notes[i]->saveNote();
    }
    saveNoteCounter();
}

void MainWindow::saveNoteCounter() {
    QDir dir(SAVE_PATH);
    if(!dir.exists()) dir.mkpath(".");

    QFile idxFile(INDEX_FILE);
    if(idxFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&idxFile);
        out << noteCounter;
        idxFile.close();
    }
}
