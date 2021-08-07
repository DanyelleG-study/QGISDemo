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
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget *thepDockWidget);//���dockwidget��������
	void inintMapCanvasAndTreeWidget();//��ʼ��������Ŀ¼��
	void setupLayerTreeViewFromSettings();//����������岻�Ӵ֣�ͼ������Ӵ�
	QgsMapLayer *activeLayer();//��ȡ��ǰѡ��ͼ��
	void onActiveLayerChanged(QgsMapLayer *layer);//�ͼ�㷢���ı�ʱ
	void mapStyleDock(bool enabled);
	void addEditToolsAction();//��ӱ༭��ť���
	bool toggleEditing(QgsMapLayer *layer, bool allowCancel);//�༭ͼ��
	void saveEdits(QgsMapLayer*layer, bool leaveEditable, bool tirggerRapaint);//����༭
	void initMapTool();//��ʼ����������
	void addAttributeToolsAction();//������Ա�ť
signals:
	void activeLayerChanged(QgsMapLayer *layer);
private:
	QgsMapCanvas *m_MapCanvas;//����
	QList<QgsMapLayer*> m_layers;//���������ӵ�ͼ��
	QgsStyleManagerDialog *m_StyleManger;
	QgsVectorLayerProperties *m_VecProperty;
	QgsRasterLayerProperties *m_RasterProperty;
	QgsLayerTreeView *m_LayerTreeView;//Ŀ¼��
	QgsLayerTreeModel *m_model;//Ŀ¼����Ӧģ��
	QgsDockWidget *m_LayerTreeDock;//Ŀ¼����dockwidget
	QgsLayerTreeMapCanvasBridge *m_LayerTreeCanvasBridge;//������Ŀ¼����������
	QStackedWidget *m_CentralContainer;
	QgsMessageBar * m_InfoBar;
	QAction *     m_ActionStyleDock;//�鿴������
	QgsDockWidget *m_MapStylingDock;//ͼ��������
	QAction *m_ActionToggleEditing;//�༭����
	QAction *m_ActionAddFeature;//���Ҫ�ع���
	QAction *m_ActionSaveLayerEdits;//����༭����
	QAction *m_ActionAttributeForm;//���Ա�ť
	bool m_SaveRollbackInProgress = false;//��Ǳ�ʾ���ڶԼ������б༭����/�ع�
	QgsMapToolDigitizeFeature *m_AddFeature = nullptr;
	QgsAdvancedDigitizingDockWidget *m_AdvancedDigitizingDockWidget = nullptr;
public slots:
	void addVecLayer();//���ʸ��ͼ��
	void addRasterLayer();//���դ��ͼ��
	void layerProperty();//ͼ������
	void styleManager();//��������
	void layerTreeViewDoubleClicked(const QModelIndex &index);//Ŀ¼��˫��һ�ڵ�
	void toggleEditing();//�����Ƿ���Ա༭
	void activateDeactivateLayerRelatedActions(QgsMapLayer *layer);//���ͼ�㷢���仯ʱ
	void saveActiveLayerEdits();//����༭����
	void commitError(QgsVectorLayer* vlayer);//���ͱ༭����
	void addFeature();//���Ҫ��
	void showAttributteForm();//��ʾ���Ա�
};
