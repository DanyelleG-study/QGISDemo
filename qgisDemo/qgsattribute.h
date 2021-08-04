#pragma once
#include "qdialog.h"
#include"qgsfeature.h"
class QgsVectorLayer;
class QLineEdit;
class QgsAttribute :
	public QDialog
{
	Q_OBJECT
public:
	QgsAttribute(QgsVectorLayer* vlayer, QgsFeature& f);
	void showAttribute();
private:
	QgsVectorLayer* m_vlayer;//ͼ��
	QgsFeature* m_feature;//Ҫ��
	QDialog *m_dialog;
	QVector<QLineEdit*> m_edits;
signals:
	void addFeatureFinished();
private slots:
	void ok();
	void cancel();
};

