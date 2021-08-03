#include "qgsmaptooladdfeature.h"
#include"qgsmapcanvas.h"
#include"qgsvectorlayerutils.h"

QgsMapToolAddFeature::QgsMapToolAddFeature(QgsMapCanvas *canvas, QgsAdvancedDigitizingDockWidget *cadDockWidget, CaptureMode mode)
	:QgsMapToolDigitizeFeature(canvas,cadDockWidget,mode)
{
	setLayer(canvas->currentLayer());
	mToolName=QStringLiteral("添加要素工具");
}


QgsMapToolAddFeature::~QgsMapToolAddFeature()
{
}

bool QgsMapToolAddFeature::addFeature(QgsVectorLayer * vlayer, const QgsFeature & f, bool showModal)
{
	QgsFeature feat(f);
	QgsFields fields = vlayer->fields();
	QgsAttributeMap initialAttributeValues;
	QgsExpressionContext context = vlayer->createExpressionContext();
	QgsFeature newFeature = QgsVectorLayerUtils::createFeature(vlayer, feat.geometry(), initialAttributeValues, &context);
	for (int index = 0; index < fields.count(); ++index)
	{

	}
	return true;
}

void QgsMapToolAddFeature::digitized(const QgsFeature & f)
{
	QgsVectorLayer *curlayer = currentVectorLayer();
	bool res = addFeature(curlayer, f, false);
	if (res)
	{

	}
}
