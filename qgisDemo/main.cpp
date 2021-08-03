#include "qgisDemo.h"
#include <QtWidgets/QApplication>
#include"qgsapplication.h"
int main(int argc, char *argv[])
{
	QgsApplication app(argc, argv, true);
	app.setPrefixPath("../../../Devenv/", true);
	QTranslator *qgislator = new QTranslator();
	if (qgislator->load(":/i18n/qgis_zh-Hans.qm"))
	{
		app.installTranslator(qgislator);
	}
	QgsApplication::setTranslation("zh-Hans");
	app.init();
	app.initQgis();
	app.processEvents();
	qgisDemo *qgis = new qgisDemo;
	qgis->show();
	int retval = app.exec();
	if (qgis != NULL)
	{
		delete qgis;
		qgis = NULL;
	}
	if (qgislator != NULL)
	{
		delete qgislator;
		qgislator = NULL;
	}
	return retval;
}
