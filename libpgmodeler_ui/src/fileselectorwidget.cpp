#include "fileselectorwidget.h"
#include <QHBoxLayout>


// A few FileSelectors, place the ones that you use often here.
FileSelector FSel_DatabaseModelOutput{
	QFileDialog::AcceptSave, QFileDialog::AnyFile,
	"Save database model file :","dbm", "PgModeler-db-model file (*.dbm);;All files (*.*)", {""},
	{"","","Set","Cancel"}, {false, false},	false};

FileSelector FSel_DatabaseModelInput{
	QFileDialog::AcceptOpen, QFileDialog::ExistingFile,
	"Open database model file :","dbm", "PgModeler-db-model file (*.dbm);;All files (*.*)", {""},
	{"","","Set","Cancel"}, {false, true},	false};

FileSelector FSel_SQLOutput{
	QFileDialog::AcceptSave, QFileDialog::AnyFile,
	"Save sql file :","sql", "", {"application/sql","application/octet-stream"},
	{"","","Set","Cancel"},	{false, false},	false};

FileSelector FSel_ExecutableInput{
	QFileDialog::AcceptOpen, QFileDialog::ExistingFile,
	"Executable input :",
#ifdef Q_OS_WIN
	"exe", "", {"application/x-ms-dos-executable","application/octet-stream"},
#else
	"",	"Executable binary", {""},
#endif
	{"","","Set","Cancel"},
	//{GlobalAttributes::getPgModelerCLIPath(),PgModelerCli,"Set","Cancel"},
	{false, true}, false};

FileSelector FSel_DirectoryInput{
	QFileDialog::AcceptOpen, QFileDialog::DirectoryOnly,
	"Choose input directory :","", "", {"application/octet-stream"},
	{"","","Set","Cancel"},	{false, true},	false};

FileSelector FSel_DirectoryOutput{
	QFileDialog::AcceptSave, QFileDialog::DirectoryOnly,
	"Choose output directory :","", "", {"application/octet-stream"},
	{"","","Set","Cancel"},	{false, false},	false};



FileSelectorWidget::FileSelectorWidget(QWidget *parent,
									   FileSelector *meta_fsel) : QWidget(parent)
{
	//File selection init
	if(meta_fsel==nullptr)
		file_selector=&FSel_SQLOutput;
	else
		file_selector=meta_fsel;

	//UI init
	QHBoxLayout *horizontalLayout;

	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	horizontalLayout->setMargin(0);
//	horizontalLayout->setContentsMargins(2,2,2,2);
	horizontalLayout->setContentsMargins(0,0,0,0);

	file_edt = new QLineEdit(this);
	file_edt->setObjectName(QString::fromUtf8("file_edt"));
	file_edt->setEnabled(true);
	file_edt->setDragEnabled(false);
	file_edt->setClearButtonEnabled(true);

	horizontalLayout->addWidget(file_edt);

	select_file_tb = new QToolButton(this);
	select_file_tb->setObjectName(QString::fromUtf8("select_file_tb"));
	select_file_tb->setEnabled(true);
	select_file_tb->setMinimumSize(QSize(0, 0));
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/icones/icones/abrir.png"), QSize(), QIcon::Normal, QIcon::Off);
	select_file_tb->setIcon(icon1);
	select_file_tb->setIconSize(QSize(22, 22));

	horizontalLayout->addWidget(select_file_tb);

	this->setLayout(horizontalLayout);
//	horizontalLayout->setMargin(10);
//	this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	//Signalling init
	connect(select_file_tb, SIGNAL(clicked()), this, SLOT(setupFileDialog()));
	connect(file_edt, SIGNAL(textChanged(QString)),
			this, SLOT(handleTextChanged(QString)));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(handleTextChange()));

}

//FileSelectorWidget::~FileSelectorWidget()
//{
//	if(timer)
//	{
//		timer->stop();
//		disconnect(timer,SIGNAL(timeout()), nullptr, nullptr);
//	}
////	delete file_selector;
//	delete file_edt;
//	delete select_file_tb;
//}

void FileSelectorWidget::setupFileDialog()
{

	QFileDialog file_dlg;

	file_dlg.setWindowTitle(file_selector->dialog_title);
	file_dlg.setAcceptMode(file_selector->accept_mode);
	file_dlg.setFileMode(file_selector->file_mode);
	file_dlg.setDefaultSuffix(file_selector->default_suffix);
	file_dlg.setDefaultSuffix(file_selector->default_suffix);
	if(file_selector->mime_type_filters.first().length()>1)
		file_dlg.setMimeTypeFilters((file_selector->mime_type_filters));
	else
		file_dlg.setNameFilter(file_selector->named_filters);
	for (short unsigned i=0;i<FileSelectorOptions.size();i++)
		 file_dlg.setOption(FileSelectorOptions[i], file_selector->options[i]);
	for (short unsigned i=0;i<FileSelectorDialogLabels.size();i++)
		 file_dlg.setLabelText(FileSelectorDialogLabels[i], file_selector->dialog_captions[i]);
	if(file_selector->dialog_captions[0]!="")
		file_dlg.setDirectory(file_selector->dialog_captions[0]);
	if(file_selector->dialog_captions[1]!="")
		file_dlg.selectFile(file_selector->dialog_captions[1]);
	file_dlg.setModal(file_selector->modal);

	connect(&file_dlg, SIGNAL(fileSelected(QString)), file_edt, SLOT(setText(QString)));
	file_dlg.exec();
}

void FileSelectorWidget::handleTextChange()
{
	if(this->isEnabled() && file_edt->text()!="")
		handleTextChanged(file_edt->text());
	else
		timer->stop();
}


void FileSelectorWidget::handleTextChanged(const QString text)
{
	//todo Imagining (|=widget border) : |/this/is/quite/a/very/long|/path/to/a/file turning into :
	// |/this/is[...]/path/to/a/file|
	// lineEdit->text().count() - qlineedit.width/ QFontMetrics::averageCharWidth > 0 ?
	// would have to dig into qt sources around QString::left/rightJustified for border & clear button

	if(text==nullptr)
	{
		emit s_fileSelected(nullptr);
		return;
	}

	//todo Would be better to run an xterm/shell than all this (NIH-ing)?
	QPalette pal=file_edt->palette();

	QFileInfo fi(text);
	if(!fi.exists())
	{
		if(file_selector->file_mode==QFileDialog::ExistingFile ||
		   file_selector->file_mode==QFileDialog::DirectoryOnly)
			pal.setColor(QPalette::Text, Qt::red);
		else
			pal.setColor(QPalette::Text, Qt::darkGreen);
	}
	else
	{
		if(file_selector->file_mode==QFileDialog::ExistingFile ||
		   file_selector->file_mode==QFileDialog::DirectoryOnly)
			pal.setColor(QPalette::Text, Qt::black);
//		else if(file_selector->file_mode==QFileDialog::DirectoryOnly) //todo combixplosion with dir mode and stuff?
//			pal.setColor(QPalette::Text, Qt::darkYellow);
		else
			pal.setColor(QPalette::Text, QColor(240,121,23));
	}

	file_edt->setPalette(pal);

	//poll disk for changes every 3s, which qtcreator does not currently.
	timer->start(3000);

	if(fi.exists() || file_selector->file_mode!=QFileDialog::ExistingFile)
	{
		emit s_fileSelected(&fi);
		if(fi.isFile())
		{
			file_selector->dialog_captions[0]=fi.absoluteDir().path();
			file_selector->dialog_captions[1]=fi.fileName();
		}
		else if(fi.isDir())
		{
			file_selector->dialog_captions[0]=fi.absoluteFilePath();
			file_selector->dialog_captions[1]="";
		}
	}
}

QString FileSelectorWidget::currentFileString()
{
	return file_edt->text();
}

void FileSelectorWidget::setCurrentFileString(QString path)
{
	file_edt->setText(path);
}
