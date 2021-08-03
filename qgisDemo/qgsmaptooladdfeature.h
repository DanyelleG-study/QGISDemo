#pragma once
#include "qgsmaptooldigitizefeature.h"
class QgsFeature;
class QgsMapToolAddFeature :
	public QgsMapToolDigitizeFeature
{
public:
	QgsMapToolAddFeature(QgsMapCanvas *canvas, QgsAdvancedDigitizingDockWidget *cadDockWidget, CaptureMode mode = QgsMapToolCapture::CaptureNone);
	virtual ~QgsMapToolAddFeature();
private:
	bool addFeature(QgsVectorLayer *vlayer, const QgsFeature &f, bool showModal = true);
	void digitized(const QgsFeature &f)override;
};

