#ifndef GIT_TUTORIAL_H
#define GIT_TUTORIAL_H

#include <QMainWindow>
#include <QtXml>
#include <QMessageBox>

#include <QPlainTextEdit>

#define STEP_QUANT 2
#define SECT_QUANT 5

namespace Ui {
class git_tutorial;
}

class git_tutorial : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit git_tutorial(QWidget *parent = 0);
    virtual ~git_tutorial();
    
private slots:
    void on_Command_clicked();

public slots:
    void processCmd(const QString &cmd);

protected:
    Ui::git_tutorial *ui;
    QDomDocument domDoc;
    QFile file;
    QDomNode domNode;
    void ShowNextPage();
    int getTextsPage(QDomNode& domNode);
    QString *SectionNames;
    QString *SectionTexts;
    int CurStep;
};


class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    Console(QWidget *parent);
    virtual ~Console();
    void output(const QString &s);
    void writeCmd(const QString &cmd);

signals:
    void CmdInput(const QString &cmd);

protected:
    QString prompt;
    void insertPrompt(bool insertNewBlock);
    void mousePressEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *event);
    void onEnter();
    QString getLastLine(const QString &text);
    QString getCmdLine(const QString &text, const QString &prompt);
};

#endif // GIT_TUTORIAL_H
