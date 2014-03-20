#include "git_tutorial.h"
#include "ui_git_tutorial.h"

void InfoMsg(const QString& msg)
{
    QMessageBox::information(NULL, "Info",
                    msg,
                    QMessageBox::Ok | QMessageBox::Default);
}

void ErrorMsg(const QString& msg)
{
    QMessageBox::warning(NULL, "Error",
                    msg,
                    QMessageBox::Ok | QMessageBox::Default);
}

git_tutorial::git_tutorial(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::git_tutorial)
{
    ui->setupUi(this);

    connect(ui->Shell, SIGNAL(CmdInput(const QString&)),
               this, SLOT(processCmd(const QString&)));

    SectionNames = new QString[SECT_QUANT];
    SectionNames[0] = "headline";
    SectionNames[1] = "description";
    SectionNames[2] = "command";
    SectionNames[3] = "shell";
    SectionNames[4] = "repository";

    SectionTexts = new QString[SECT_QUANT];

    CurStep = 0;

    file.setFileName("data.xml");
    if(file.open(QIODevice::ReadOnly))
    {
        if(domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            domNode = domElement.firstChild();
        }
        else
            ErrorMsg("Can't get file with XML structure");
    }
    else
        ErrorMsg("Can't open file with data");

    ShowNextPage();
}

git_tutorial::~git_tutorial()
{
    file.close();
    delete [] SectionNames;
    delete [] SectionTexts;
    delete ui;
}

int git_tutorial::getTextsPage(QDomNode& domNode)
{
    int ret = 0;
    for(int i = 0; i < SECT_QUANT; i++)
    {
        SectionTexts[i] = "Error: Section is broken";
        if(!domNode.isNull())
        {
            if(domNode.isElement())
            {
                QDomElement domElement = domNode.toElement();
                if(!domElement.isNull())
                {
                    if(domElement.tagName() == SectionNames[i])
                        SectionTexts[i] = domElement.text();
                    else
                        ret = 4;
                }
                else
                    ret = 3;
            }
            else
                ret = 2;
        }
        else
            ret = 1;

        domNode = domNode.nextSibling();
    }

    return ret;
}

void git_tutorial::ShowNextPage()
{
    if(CurStep < STEP_QUANT)
    {
        if(getTextsPage(domNode) != 0)
            ErrorMsg("XML file is broken or has wrong format");

        ui->Headline->setText(SectionTexts[0]);
        ui->Description->setText(SectionTexts[1]);
        ui->Command->setText(SectionTexts[2]);
        ui->Shell->output(SectionTexts[3]);
        ui->Repository->setText(SectionTexts[4]);

        CurStep++;
    }
    else
        InfoMsg("The tutorial is over!");
}

void git_tutorial::on_Command_clicked()
{
    ui->Shell->writeCmd(ui->Command->text());
}

void git_tutorial::processCmd(const QString &cmd)
{
    if(cmd == ui->Command->text())
        ShowNextPage();
    else
        ui->Shell->output(cmd + ": command not found");
}


////////////////////////////////////////////////////////////////////////////////////////

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    prompt = "$ ";
}

Console::~Console()
{

}

void Console::insertPrompt(bool insertNewBlock)
{
    if(insertNewBlock)
        textCursor().insertBlock();
    textCursor().insertText(prompt);
}

void Console::mousePressEvent(QMouseEvent *)
{
    setFocus();
}

void Console::keyPressEvent(QKeyEvent *event)
{
    //ban input if we are waiting a answer
    if(isReadOnly())
            return;

    //for adding new chars to the end of line
    if(event->key() >= 0x20 && event->key() <= 0x7e
       && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
        QPlainTextEdit::keyPressEvent(event);

    //the rule of erasing chars
    if(event->key() == Qt::Key_Backspace
       && event->modifiers() == Qt::NoModifier
       && textCursor().positionInBlock() > prompt.length())
        QPlainTextEdit::keyPressEvent(event);

    //if <Enter> was pressed
    if(event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier)
        onEnter();

}

QString Console::getLastLine(const QString &text)
{
    QString line;
    int posN = 0;
    for(int i=text.size()-1; i >= 0; i--)
        if(text[i] == '\n')
        {
            posN = i;
            break;
        }

    int LineBegin = 0;
    if(posN != 0)
        LineBegin = posN+1;

    for(int i=LineBegin; i < text.size(); i++)
        line += text[i];

    return line;
}

QString Console::getCmdLine(const QString &text, const QString &prompt)
{
    QString line = getLastLine(text);

    QString cmdLine = line.remove(0,prompt.length());

    return cmdLine;
}

void Console::onEnter()
{
    if(textCursor().positionInBlock() == prompt.length())
    {
        insertPrompt(false);
        return;
    }

    setReadOnly(true);

    emit CmdInput(getCmdLine(toPlainText(), prompt));
}

void Console::output(const QString &s)
{
    textCursor().insertBlock();
    textCursor().insertText(s);
    insertPrompt(true);
    setReadOnly(false);
}

void Console::writeCmd(const QString &cmd)
{
    textCursor().insertText(cmd);
}

