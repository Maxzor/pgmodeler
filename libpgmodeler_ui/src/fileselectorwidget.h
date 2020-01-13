#ifndef FILESELECTORWIDGET_H
#define FILESELECTORWIDGET_H

#include <QLineEdit>
#include <QToolButton>
#include <QFileDialog>
#include <QTimer>

const QString PgModelerCli("pgmodeler-cli");

//todo better use flags
static QList<QFileDialog::DialogLabel> FileSelectorDialogLabels{
		QFileDialog::LookIn,
		QFileDialog::FileName,
		QFileDialog::Accept,
		QFileDialog::Reject
};

static QList<QFileDialog::Option> FileSelectorOptions{
		QFileDialog::ShowDirsOnly,
		QFileDialog::DontConfirmOverwrite
};

struct FileSelector
{
	QFileDialog::AcceptMode accept_mode;
	QFileDialog::FileMode file_mode;
	QString dialog_title, default_suffix, named_filters;
	QStringList mime_type_filters, dialog_captions; //binds to MetaFileSelectorDialogLabels
	QList<bool>	options; //binds to MetaFileSelectorOptions
	bool modal;
};

class FileSelectorWidget : public QWidget
{
private:
	Q_OBJECT
	QLineEdit *file_edt;
	QToolButton *select_file_tb;

	QTimer *timer;

public:
	FileSelector *file_selector;
	FileSelectorWidget(QWidget *parent,
					   FileSelector *meta_file_selector=nullptr);
//	~FileSelectorWidget();

	QString currentFileString();

private slots:
	void handleTextChange();
public slots:
	void setCurrentFileString(QString path);
	void setupFileDialog();
	void handleTextChanged(const QString text);
signals:
	void s_fileSelected(QFileInfo *file_info);
};

#endif // FILESELECTORWIDGET_H
