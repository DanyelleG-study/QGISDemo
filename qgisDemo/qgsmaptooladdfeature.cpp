#include "qgsmaptooladdfeature.h"
#include"qgsmapcanvas.h"
#include"qgsattribute.h"
#include"qgsrubberband.h"
#include<qdialog.h>

QgsMapToolAddFeature::QgsMapToolAddFeature(QgsMapCanvas *canvas, QgsAdvancedDigitizingDockWidget *cadDockWidget, CaptureMode mode)
	:QgsMapToolDigitizeFeature(canvas, cadDockWidget, mode)
{
	setLayer(canvas->currentLayer());
	mToolName = QStringLiteral("添加要素工具");
}


QgsMapToolAddFeature::~QgsMapToolAddFeature()
{
}

bool QgsMapToolAddFeature::addFeature(QgsVectorLayer * vlayer, const QgsFeature & f, bool showModal)
{
	QgsFeature feat(f);
	QgsAttribute *attribute = new QgsAttribute(vlayer, feat);
	attribute->showAttribute();
	connect(attribute, &QgsAttribute::addFeatureFinished, this, [=]
	{
		QgsRubberBand *rb = takeRubberBand();
		if (rb)
		{
			rb->deleteLater();
		}
	});
	//if (QgsRubberBand *rb = takeRubberBand())
	//	connect(attribute, &QgsAttribute::addFeatureFinished, rb, &QgsRubberBand::deleteLater);
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