#include "fontoptionswidget.h"
#include "ui_fontoptionswidget.h"
#include "options.h"
#include <QStringList>
#include <QDebug>

FontOptionsWidget::FontOptionsWidget(QWidget *parent) :
	OptionsDialogPage(parent),
    ui(new Ui::fontoptionswidget)
{
    ui->setupUi(this);

	//
	//	Populate font families list
	//

	QStringList families = mFontDatabase.families();
	foreach (QString family, families)
		ui->fontCombo->addItem(family);

	QStringList fontFamilies = QFont::substitutes(Options::getEditorFont().family());
	fontFamilies.push_front(Options::getEditorFont().family());
	foreach (QString family, fontFamilies)
	{
		int index = ui->fontCombo->findText(family, Qt::MatchFixedString);
		qDebug() << "Index of " << family << " is " << index;
		if (index > 0)
		{
			ui->fontCombo->setCurrentIndex(index);
			break;
		}
	}

	//
	//	Populate standard sizes list
	//

	QList<int> sizes  = mFontDatabase.standardSizes();
	foreach (int size, sizes)
		ui->sizeCombo->addItem(QString::number(size));

	int index = ui->sizeCombo->findText(QString::number(Options::getEditorFont().pointSize()));
	ui->sizeCombo->setCurrentIndex(index);
}

FontOptionsWidget::~FontOptionsWidget()
{
    delete ui;
}

void FontOptionsWidget::apply()
{
	bool ok;
	int size = ui->sizeCombo->currentText().toInt(&ok);
	if (!ok)
		size = 12;

	Options::setEditorFont(mFontDatabase.font(ui->fontCombo->currentText(), "", size));
}
