#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QVector>

#define SAVE_PATH "/media/paul/D/QT_Projects/New_Series/Sticky_Notes/Notes"
#define MAX_NOTES 20

class StickyNote : public QWidget {
    Q_OBJECT
public:
    StickyNote(const QString &name, QWidget *parent = nullptr);
    QString getNoteName() const { return windowTitle(); }
    void renameNote(const QString &newName);
    QTextEdit* getEditor() const { return editor; }

public slots:
    void saveNote();
    void deleteNote();

signals:
    void noteDeleted(StickyNote* note); // notify MainWindow

private:
    QTextEdit *editor;
    QString currentFileName() const; // current file in SAVE_PATH
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void createNote();
    void updateNotesMenu();
    void handleNoteDeleted(StickyNote* note); // remove deleted note

private:
    QVector<StickyNote*> notes;
    QMenu *notesMenu;
    QMenu *alwaysOnTop;
    int noteCounter;
    void saveNoteCounter();
    void reindexNotes(); // adjust note numbers after deletion
};
