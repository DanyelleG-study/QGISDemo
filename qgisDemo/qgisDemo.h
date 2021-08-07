#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qgisDemo.h"
#include"qgsmapcanvas.h"
#include"qgsvectorlayerproperties.h"
#include"qgsrasterlayerproperties.h"
#include"qgsmessagebar.h"
#include"qgsstylemanagerdialog.h"
#include"qgslayertreeview.h"
#include"qgsdockwidget.h"
#include"qgslayertreemapcanvasbridge.h"
#include "qgsmaptooldigitizefeature.h"
class qgisDemo : public QMainWindow
{
    Q_OBJECT

public:
    qgisDemo(QWidget *parent = Q_NULLPTR);
	~qgisDemo();
private:
    Ui::qgisDemoClass ui;
private:
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget *thepDockWidget);//添加dockwidget到主界面
	void inintMapCanvasAndTreeWidget();//初始化画布和目录树
	void setupLayerTreeViewFromSettings();//设置组的字体不加粗，图层字体加粗
	QgsMapLayer *activeLayer();//获取当前选中图层
	void onActiveLayerChanged(QgsMapLayer *layer);//活动图层发生改变时
	void mapStyleDock(bool enabled);
	void addEditToolsAction();//添加编辑按钮添加
	bool toggleEditing(QgsMapLayer *layer, bool allowCancel);//编辑图层
	void saveEdits(QgsMapLayer*layer, bool leaveEditable, bool tirggerRapaint);//保存编辑
	void initMapTool();//初始化画布工具
	void addAttributeToolsAction();//添加属性表按钮
signals:
	void activeLayerChanged(QgsMapLayer *layer);
private:
	QgsMapCanvas *m_MapCanvas;//画布
	QList<QgsMapLayer*> m_layers;//存放所有添加的图层
	QgsStyleManagerDialog *m_StyleManger;
	QgsVectorLayerProperties *m_VecProperty;
	QgsRasterLayerProperties *m_RasterProperty;
	QgsLayerTreeView *m_LayerTreeView;//目录树
	QgsLayerTreeModel *m_model;//目录树对应模型
	QgsDockWidget *m_LayerTreeDock;//目录树的dockwidget
	QgsLayerTreeMapCanvasBridge *m_LayerTreeCanvasBridge;//画布与目录树的链接桥
	QStackedWidget *m_CentralContainer;
	QgsMessageBar * m_InfoBar;
	QAction *     m_ActionStyleDock;//查看风格面板
	QgsDockWidget *m_MapStylingDock;//图层风格设置
	QAction *m_ActionToggleEditing;//编辑工具
	QAction *m_ActionAddFeature;//添加要素工具
	QAction *m_ActionSaveLayerEdits;//保存编辑工具
	QAction *m_ActionAttributeForm;//属性表按钮
	bool m_SaveRollbackInProgress = false;//标记表示正在对激活层进行编辑保存/回滚
	QgsMapToolDigitizeFeature *m_AddFeature = nullptr;
	QgsAdvancedDigitizingDockWidget *m_AdvancedDigitizingDockWidget = nullptr;
public slots:
	void addVecLayer();//添加矢量图层
	void addRasterLayer();//添加栅格图层
	void layerProperty();//图层属性
	void styleManager();//风格管理器
	void layerTreeViewDoubleClicked(const QModelIndex &index);//目录树双击一节点
	void toggleEditing();//控制是否可以编辑
	void activateDeactivateLayerRelatedActions(QgsMapLayer *layer);//当活动图层发生变化时
	void saveActiveLayerEdits();//保存编辑内容
	void commitError(QgsVectorLayer* vlayer);//发送编辑错误
	void addFeature();//添加要素
	void showAttributteForm();//显示属性表
};
