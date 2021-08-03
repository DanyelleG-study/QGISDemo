#include "qgisDemo.h"
#include"qfiledialog.h"
#include"qmessagebox.h"
#include"qgsvectorlayer.h"
#include"qgsrasterlayer.h"
#include"qgsdockwidget.h"
#include"qboxlayout.h"
#include"qgsdockwidget.h"
#include"qgsapplication.h"
#include"qgslayertreeviewdefaultactions.h"
#include "qgstransaction.h"
#include "qgstransactiongroup.h"
#include "qgsmessagebar.h"
#include "qgsmessageviewer.h"
#include "qgsmessagebaritem.h"
#include"qgsadvanceddigitizingdockwidget.h"
#include <vector>
#include <qtoolbar.h>
#include <qmap.h>
#include<QPair>
using namespace  std;
qgisDemo::qgisDemo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_StyleManger = NULL;
	m_VecProperty = NULL;
	m_RasterProperty = NULL;
	m_MapCanvas = NULL;
	inintMapCanvasAndTreeWidget();//初始化画布目录树
	addEditToolsAction();//添加编辑工具的按钮
	initMapTool();//初始化编辑工具
	connect(m_LayerTreeView, &QAbstractItemView::doubleClicked, this, &qgisDemo::layerTreeViewDoubleClicked);
	connect(m_LayerTreeView, &QgsLayerTreeView::currentLayerChanged, this, &qgisDemo::onActiveLayerChanged);
	connect(ui.actionaddVectorLayer, &QAction::triggered, this, &qgisDemo::addVecLayer);
	connect(ui.actionaddRasterLaer, &QAction::triggered, this, &qgisDemo::addRasterLayer);
	connect(ui.actionlayerProperty, &QAction::triggered, this, &qgisDemo::layerProperty);
	connect(ui.actionstyleManager, &QAction::triggered, this, &qgisDemo::styleManager);
}
void qgisDemo::onActiveLayerChanged(QgsMapLayer *layer)
{
	if (layer != NULL)
	{
		m_MapCanvas->setCurrentLayer(layer);
		emit activeLayerChanged(layer);
	}
	connect(this, &qgisDemo::activeLayerChanged, this, &qgisDemo::activateDeactivateLayerRelatedActions);
}
void qgisDemo::activateDeactivateLayerRelatedActions(QgsMapLayer *layer)
{
	
}
void qgisDemo::saveActiveLayerEdits()
{
	saveEdits(activeLayer(), true, true);
}
void qgisDemo::saveEdits(QgsMapLayer*layer, bool leaveEditable, bool tirggerRapaint)
{
	QgsVectorLayer *vec = qobject_cast<QgsVectorLayer*>(activeLayer());
	if(!vec||!vec->isEditable()||!vec->isModified())
	{
		return;//不存在，不可编辑，不可修改
	}
	if(layer!=activeLayer())
	{
		m_SaveRollbackInProgress = true;
	}
	if(!vec->commitChanges(!leaveEditable))
	{
		m_SaveRollbackInProgress = false;
		commitError(vec);
	}
	if(tirggerRapaint)
	{
		vec->triggerRepaint();
	}
}
void qgisDemo::commitError(QgsVectorLayer* vlayer)
{
	const QStringList commitErrors = vlayer->commitErrors();
	if(!vlayer->allowCommit()&&commitErrors.empty())
	{
		return;
	}
	QgsMessageViewer *mv = new QgsMessageViewer();
	mv->setWindowTitle(QStringLiteral("抛出异常"));
	mv->setMessageAsPlainText(QStringLiteral("不能对图层%1改变").arg(vlayer->name())
		+ "\n\n"
		+ QStringLiteral("错误: %1\n").arg(commitErrors.join(QLatin1String("\n  ")))
	);

	QToolButton *showMore = new QToolButton();
	// store pointer to vlayer in data of QAction
	QAction *act = new QAction(showMore);
	act->setData(QVariant(QMetaType::QObjectStar, &vlayer));
	act->setText(tr("展示更多"));
	showMore->setStyleSheet(QStringLiteral("background-color: rgba(255, 255, 255, 0); color: black; text-decoration: underline;"));
	showMore->setCursor(Qt::PointingHandCursor);
	showMore->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	showMore->addAction(act);
	showMore->setDefaultAction(act);
	connect(showMore, &QToolButton::triggered, mv, &QDialog::exec);
	connect(showMore, &QToolButton::triggered, showMore, &QObject::deleteLater);

	// no timeout set, since notice needs attention and is only shown first time layer is labeled
	QgsMessageBarItem *errorMsg = new QgsMessageBarItem(
		tr("Commit errors"),
		tr("Could not commit changes to layer %1").arg(vlayer->name()),
		showMore,
		Qgis::Warning,
		0,
		m_InfoBar);
	m_InfoBar->pushItem(errorMsg);
}
void qgisDemo::layerTreeViewDoubleClicked(const QModelIndex &index)
{
	QgsMapLayer *layer = activeLayer();
	if (!layer)
		return;
	switch (layer->type())
	{
	case QgsMapLayerType::RasterLayer:
	{

	}
	case QgsMapLayerType::VectorLayer:
	{
		QgsVectorLayer *vec = qobject_cast<QgsVectorLayer*>(layer);
		QgsVectorLayerProperties *vectorLayerPropertiesDialog = new QgsVectorLayerProperties(m_MapCanvas, m_InfoBar, vec, this);
		vectorLayerPropertiesDialog->exec();
		delete vectorLayerPropertiesDialog;
		break;
	}
	default:
		break;
	}
}
void qgisDemo::addDockWidget(Qt::DockWidgetArea area, QDockWidget *thepDockWidget)
{
	QMainWindow::addDockWidget(area, thepDockWidget);
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	thepDockWidget->show();
	if (m_MapCanvas != NULL)
		m_MapCanvas->refresh();
}
void qgisDemo::inintMapCanvasAndTreeWidget()
{
	QWidget *centralWidget = this->centralWidget();
	QGridLayout *centralLayout = new QGridLayout(centralWidget);
	centralWidget->setLayout(centralLayout);
	centralLayout->setContentsMargins(0, 0, 0, 0);
	m_InfoBar = new QgsMessageBar(centralWidget);
	m_InfoBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	centralLayout->addWidget(m_InfoBar, 0, 0, 1, 1);
	m_MapCanvas = new QgsMapCanvas(centralWidget);
	m_MapCanvas->setCanvasColor(QColor(255, 255, 255));//设置背景颜色
	m_MapCanvas->setProject(QgsProject::instance());
	m_LayerTreeView = new QgsLayerTreeView(this);
	m_model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
	m_model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
	m_model->setFlag(QgsLayerTreeModel::AllowNodeRename);
	m_model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
	m_model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
	m_model->setFlag(QgsLayerTreeModel::UseEmbeddedWidgets);
	m_model->setFlag(QgsLayerTreeModel::UseTextFormatting);
	m_model->setAutoCollapseLegendNodes(10);
	m_LayerTreeView->setModel(m_model);
	m_LayerTreeView->setMessageBar(m_InfoBar);
	QTreeView v;
	QStandardItemModel m;
	v.setModel(&m);
	QAction *actionAddGroup = new QAction(QStringLiteral("添加组"), this);
	actionAddGroup->setIcon(QgsApplication::getThemeIcon(QStringLiteral("/mActionAddGroup.svg")));
	actionAddGroup->setToolTip(QStringLiteral("添加组"));
	connect(actionAddGroup, &QAction::triggered, m_LayerTreeView->defaultActions(), &QgsLayerTreeViewDefaultActions::addGroup);
	m_ActionStyleDock = new QAction(QStringLiteral("图层风格"), this);
	m_ActionStyleDock->setCheckable(true);
	m_ActionStyleDock->setToolTip(QStringLiteral("打开图层风格面板"));
	m_ActionStyleDock->setIcon(QgsApplication::getThemeIcon(QStringLiteral("propertyicons/symbology.svg")));
	connect(m_ActionStyleDock, &QAction::toggled, this, &qgisDemo::mapStyleDock);
	QToolBar *toolbar = new QToolBar();
	toolbar->setIconSize(QgsGuiUtils::iconSize(true));
	toolbar->addAction(actionAddGroup);
	toolbar->addAction(m_ActionStyleDock);
	QVBoxLayout *vboxLayout = new QVBoxLayout();
	vboxLayout->setContentsMargins(0, 0, 0, 0);
	vboxLayout->setSpacing(0);
	vboxLayout->addWidget(toolbar);
	vboxLayout->addWidget(m_LayerTreeView);
	m_LayerTreeDock = new QgsDockWidget(QStringLiteral("图层树"), this);
	m_LayerTreeDock->setObjectName(QStringLiteral("图层树"));
	m_LayerTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	QWidget *w = new QWidget;
	w->setLayout(vboxLayout);
	m_LayerTreeDock->setWidget(w);
	addDockWidget(Qt::LeftDockWidgetArea, m_LayerTreeDock);
	m_LayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), m_MapCanvas, this);
	m_CentralContainer = new QStackedWidget;
	m_CentralContainer->insertWidget(0, m_MapCanvas);
	centralLayout->addWidget(m_CentralContainer, 0, 0, 2, 1);
	m_CentralContainer->setCurrentIndex(0);
}
void qgisDemo::mapStyleDock(bool enabled)
{
}
void qgisDemo::addEditToolsAction()
{
	m_ActionToggleEditing = new QAction(QStringLiteral("编辑"));
	m_ActionToggleEditing->setIcon(QgsApplication::getThemeIcon(QStringLiteral("/mActionToggleEditing.svg")));
	m_ActionToggleEditing->setToolTip(QStringLiteral("编辑"));
	m_ActionAddFeature = new QAction(QStringLiteral("添加要素"));
	m_ActionAddFeature->setIcon(QgsApplication::getThemeIcon(QStringLiteral("/mActionCapturePoint.svg")));
	m_ActionSaveLayerEdits = new QAction(QStringLiteral("保存图层编辑"));
	m_ActionSaveLayerEdits->setIcon(QgsApplication::getThemeIcon(QStringLiteral("/ mActionSaveAllEdits.svg")));
	ui.mainToolBar->addAction(m_ActionToggleEditing);
	ui.mainToolBar->addAction(m_ActionAddFeature);
	ui.mainToolBar->addAction(m_ActionSaveLayerEdits);
	connect(m_ActionToggleEditing, &QAction::triggered, this, [=] { toggleEditing(); });
	connect(m_ActionSaveLayerEdits, &QAction::triggered, this, [=] {saveActiveLayerEdits(); });
	connect(m_ActionAddFeature, &QAction::triggered, this, &qgisDemo::addFeature);
}
void qgisDemo::addFeature()
{
	m_MapCanvas->setMapTool(m_AddFeature);

}
void qgisDemo::initMapTool()
{
	//m_AdvancedDigitizingDockWidget = new QgsAdvancedDigitizingDockWidget(m_MapCanvas, this);
	//m_AdvancedDigitizingDockWidget->setWindowTitle(QStringLiteral("先进编辑工具"));
	//m_AdvancedDigitizingDockWidget->setObjectName(QStringLiteral("先进编辑工具"));
	//m_AddFeature = new QgsMapToolDigitizeFeature(m_MapCanvas, m_AdvancedDigitizingDockWidget);
	//m_AddFeature->setAction(m_ActionAddFeature);
}
void qgisDemo::toggleEditing()
{
	QgsVectorLayer *vec = qobject_cast<QgsVectorLayer*>(activeLayer());
	if (vec)
	{
		toggleEditing(vec, true);
	}
}
bool qgisDemo::toggleEditing(QgsMapLayer *layer, bool allowCancel)
{
	QgsVectorLayer *vec = qobject_cast<QgsVectorLayer*>(layer);
	if (!vec)
		return false;
	bool res = true;
	QString connString = QgsTransaction::connectionString(vec->source());
	QString key = vec->providerType();//ogr
	QMap< QPair< QString, QString>, QgsTransactionGroup *>transactionGroup = QgsProject::instance()->transactionGroups();//"ogr" "shp文件完整路径"
	QMap< QPair< QString, QString>, QgsTransactionGroup *>::iterator tIt = transactionGroup.find(qMakePair(key, connString));
	QgsTransactionGroup *tg = (tIt != transactionGroup.end() ? tIt.value() : nullptr);
	bool isModified = false;
	QString modifiedLayerNames;
	bool hasSeveralModifiedLayers = false;//有很多个图层发生修改
	if (tg&&tg->layers().contains(vec) && tg->modified())
	{
		isModified = true;
		vector<QString> vectModifiedLayerNames;
		if (vec->isModified())
			vectModifiedLayerNames.push_back(vec->name());
		for (QgsVectorLayer *iterLayer : tg->layers())
		{
			if (iterLayer != vec && iterLayer->isModified())
			{
				vectModifiedLayerNames.push_back((iterLayer->name()));
			}
		}
		if (vectModifiedLayerNames.size() == 1)
		{
			modifiedLayerNames = vectModifiedLayerNames[0];
		}
		else if (vectModifiedLayerNames.size() == 2)
		{
			modifiedLayerNames = QStringLiteral("%1 and %2").arg(vectModifiedLayerNames[0]).arg(vectModifiedLayerNames[1]);
		}
		hasSeveralModifiedLayers = vectModifiedLayerNames.size() > 1;
	}
	else if (vec->isModified())
	{
		isModified = true;
		modifiedLayerNames = vec->name();
	}
	if (!vec->isEditable() && !vec->readOnly())
	{
		if (!(vec->dataProvider()->capabilities()&QgsVectorDataProvider::EditingCapabilities))
		{
			m_ActionToggleEditing->setCheckable(false);
			m_ActionToggleEditing->setEnabled(false);
			QgsSettings settings;
			m_InfoBar->pushMessage(QStringLiteral("开始编辑失败！"), QStringLiteral("编辑器不能启动编辑"), Qgis::Info, settings.value(QStringLiteral("qgis/messageTimeout"), 5).toInt());
			return false;
		}
		vec->startEditing();
		QgsSettings settings;
		QString markerType = settings.value(QStringLiteral("qgis/digitizing/marker_style"), "Cross").toString();
		bool markSelectedOnly = settings.value(QStringLiteral("qgis/digitizing/marker_only_for_selected"), true).toBool();

		// redraw only if markers will be drawn
		if ((!markSelectedOnly || vec->selectedFeatureCount() > 0) &&
			(markerType == QLatin1String("Cross") || markerType == QLatin1String("SemiTransparentCircle")))
		{
			vec->triggerRepaint();
		}
	}
	else if (isModified)
	{
		QMessageBox::StandardButtons buttons = QMessageBox::Save | QMessageBox::Discard;
		if (allowCancel)
			buttons |= QMessageBox::Cancel;
		switch (QMessageBox::question(nullptr, QStringLiteral("停止编辑"), hasSeveralModifiedLayers ? QStringLiteral("是否想要保存所有的编辑到图层%1？").arg(modifiedLayerNames) : QStringLiteral("是否想要保存编辑到该图层%1？").arg(modifiedLayerNames), buttons))
		{
		case QMessageBox::Cancel:
			res = false;
			break;
		case QMessageBox::Save:
			QApplication::setOverrideCursor(Qt::WaitCursor);

			if (!vec->commitChanges())
			{
				//commitError(vlayer);
				res = false;
			}
			vec->triggerRepaint();
			QApplication::restoreOverrideCursor();
			break;
		case QMessageBox::Discard:
		{
			QApplication::setOverrideCursor(Qt::WaitCursor);
			if (!vec->rollBack())
			{
				m_InfoBar->pushMessage(QStringLiteral("错误"),
					QStringLiteral("回滚期间的问题"),
					Qgis::Critical);
				res = false;
			}

			vec->triggerRepaint();

			QApplication::restoreOverrideCursor();
			break;
		}

		default:
			break;
		}
	}
	else //layer not modified
	{
		vec->rollBack();
		res = true;
		vec->triggerRepaint();
	}
	return res;
}
void qgisDemo::setupLayerTreeViewFromSettings()
{
	QgsLayerTreeModel *model = m_LayerTreeView->layerTreeModel();
	QFont fontLayer, fontGroup;
	fontLayer.setBold(true);//加粗显示
	fontGroup.setBold(false);
	model->setLayerTreeNodeFont(QgsLayerTree::NodeLayer, fontLayer);
	model->setLayerTreeNodeFont(QgsLayerTree::NodeGroup, fontGroup);
}
QgsMapLayer * qgisDemo::activeLayer()
{
	return m_LayerTreeView ? m_LayerTreeView->currentLayer() : nullptr;
}
qgisDemo::~qgisDemo()
{
	if (m_MapCanvas != NULL)
	{
		delete m_MapCanvas;
		m_MapCanvas = NULL;
	}
	QgsApplication::exitQgis();//删除提供程序注册表和图层注册表
}

void qgisDemo::addRasterLayer()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("open vector"), "", "*.tif");
	QStringList temp = filename.split(QDir::separator());
	QString basename = temp.at(temp.size() - 1);
	QgsRasterLayer::LayerOptions layerOption;
	layerOption.loadDefaultStyle = false;
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(filename, basename, "gdal", layerOption);
	if (!rasterLayer->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层无效!\n") + filename);
		return;
	}
	m_layers.append(rasterLayer);
	m_MapCanvas->setExtent(rasterLayer->extent());
	m_MapCanvas->setVisible(true);
	m_MapCanvas->freeze(false);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(rasterLayer);
	QgsProject::instance()->addMapLayer(rasterLayer);
}
void qgisDemo::layerProperty()
{
	if (m_MapCanvas->currentLayer()->type() == QgsMapLayerType::VectorLayer)
	{
		QgsMessageBar *messageBar = new QgsMessageBar();
		QgsVectorLayer *vec = qobject_cast<QgsVectorLayer *>(m_MapCanvas->currentLayer());
		if (m_VecProperty == NULL)
		{
			m_VecProperty = new QgsVectorLayerProperties(m_MapCanvas, messageBar, vec);
		}
		m_VecProperty->exec();
	}
	if (m_MapCanvas->currentLayer()->type() == QgsMapLayerType::RasterLayer)
	{
		if (m_RasterProperty == NULL)
		{
			m_RasterProperty = new QgsRasterLayerProperties(qobject_cast<QgsRasterLayer *>(m_MapCanvas->currentLayer()), m_MapCanvas, NULL);
		}
		m_RasterProperty->exec();
	}
}
void qgisDemo::styleManager()
{
	QgsStyle *style = QgsStyle::defaultStyle();
	if (m_StyleManger == NULL)
	{
		m_StyleManger = new QgsStyleManagerDialog(style);
	}
	m_StyleManger->show();
}
void qgisDemo::addVecLayer()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open shape file"), "", "*.shp");
	QStringList temp = fileName.split('/');
	QString basename = temp.at(temp.size() - 1);
	QgsVectorLayer* vecLayer = new QgsVectorLayer(fileName, basename, "ogr");

	if (!vecLayer->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层无效!\n") + fileName);
		return;
	}
	m_MapCanvas->setExtent(vecLayer->extent());
	m_layers.append(vecLayer);
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(vecLayer);
	QgsProject::instance()->addMapLayer(vecLayer);
}