#pragma once
#include "qgsmaptooldigitizefeature.h"
#include<qdialog.h>
class QgsFeature;
class QgsMapToolAddFeature :
	public QgsMapToolDigitizeFeature
{
	Q_OBJECT
public:
	QgsMapToolAddFeature(QgsMapCanvas *canvas, QgsAdvancedDigitizingDockWidget *cadDockWidget, CaptureMode mode = QgsMapToolCapture::CaptureNone);
	virtual ~QgsMapToolAddFeature();
private:
	bool addFeature(QgsVectorLayer *vlayer, const QgsFeature &f, bool showModal = true);
	void digitized(const QgsFeature &f)override;
};

