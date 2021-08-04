#include "qgsattribute.h"
#include"qgsfields.h"
#include"qgsvectorlayer.h"
#include"qgsvectorlayerutils.h"
#include<qlabel.h>
#include<qboxlayout.h>
#include<qlineedit.h>
#include<qpushbutton.h>
#include<qdialogbuttonbox.h>
QgsAttribute::QgsAttribute(QgsVectorLayer* vlayer, QgsFeature& f)
	:m_vlayer(vlayer)
	,m_feature(&f)
{
	QgsFields fields = vlayer->fields();
	QgsAttributeMap initialAttributeValues;
	m_dialog = new QDialog();
	m_dialog->setWindowTitle(QStringLiteral("属性表"));
	m_dialog->setObjectName(QStringLiteral("属性表"));
	QGridLayout *gridLayout = new QGridLayout(m_dialog);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	QDialogButtonBox *buttonBox = new QDialogButtonBox(m_dialog);
	buttonBox->setObjectName("buttonBox");
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	gridLayout->addWidget(buttonBox, 1, 0, 1, 1);
	QVBoxLayout *verticalLayout = new QVBoxLayout();
	verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("确定"));
	buttonBox->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
	
	for (int i = 0; i < fields.size(); i++)
	{
		QHBoxLayout *horizontalLayout = new QHBoxLayout();
		horizontalLayout->setSpacing(6);
		horizontalLayout->setObjectName(QStringLiteral("水平布局") + i);
		QLabel *label = new QLabel(m_dialog);
		QString name = fields.at(i).name();
		label->setObjectName(name);
		label->setText(name);
		horizontalLayout->addWidget(label);
		QLineEdit *lineEdit = new QLineEdit(m_dialog);
		lineEdit->setObjectName(QStringLiteral("编辑") + i);
		m_edits.push_back(lineEdit);
		horizontalLayout->addWidget(lineEdit);
		verticalLayout->addLayout(horizontalLayout);
	}
	gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QgsAttribute::ok);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QgsAttribute::cancel);
}
void QgsAttribute::cancel()
{
	emit addFeatureFinished();
	m_dialog->reject();
}
void QgsAttribute::ok()
{
	QgsAttributeMap initialAttributeValues;
	for (int i = 0; i < m_edits.size(); i++)
	{
		initialAttributeValues.insert(i, m_edits.at(i)->text());
	}
	QgsExpressionContext context = m_vlayer->createExpressionContext();
	QgsFeature newFeature = QgsVectorLayerUtils::createFeature(m_vlayer, m_feature->geometry(), initialAttributeValues, &context);
	bool featureSaved = m_vlayer->addFeature(newFeature);
	if (featureSaved)
	{
		m_vlayer->endEditCommand();
		m_vlayer->triggerRepaint();
	}
	else
	{
		m_vlayer->destroyEditCommand();
	}
	emit addFeatureFinished();
	m_dialog->accept();
	
}
void QgsAttribute::showAttribute()
{
	if (m_dialog != NULL)
	{
		m_dialog->exec();
	}
}
