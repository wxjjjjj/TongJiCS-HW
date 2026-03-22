#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QPainter>
#include <QTimerEvent>
#include <QPainterPath>
#include <cmath>
#include <QDebug>

#define TIMEOUT 1*1000 //1秒

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

enum Type{
    _SingleList,
    _CycleList,
    _DoubleList
};

enum OPType{
    _SetUp,
    _Delete,
    _Insert,
    _Find
};

struct ListNode{
    int data;
    ListNode* next;
    ListNode* prev;//双向链表
    bool isHighlighted;//用于高亮某个节点

    ListNode(int val) : data(val),next(nullptr),prev(nullptr),isHighlighted(false){}
};

class Widget;

// 自定义的绘制区域widget
class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);
    void setMainWidget(Widget* mainWidget);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Widget* m_mainWidget;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    virtual void timerEvent(QTimerEvent *event);//获取当前定时器编号
    virtual void paintEvent(QPaintEvent *event);//绘制
    ~Widget();
    
    // 公共绘制接口，供PaintWidget调用
    void drawListPublic(QPainter& painter, int width, int height);
    
    // 指针操作方法
    void moveCurrentNext();     // current指针向后移动
    void moveCurrentPrev();     // current指针向前移动（双向链表）
    void moveCurrentToHead();   // current指针移动到头节点
    void moveCurrentToTail();   // current指针移动到尾节点
    void moveCurrentToPosition(int pos); // current指针移动到指定位置
    
    // 提供数据访问接口给PaintWidget
    bool shouldDraw() const { return draw; }
    ListNode* getHead() const { return head; }
    ListNode* getTail() const { return tail; }
    ListNode* getCurrent() const { return current; }
    ListNode* getDrawNode() const { return drawNode; }
    int getDrawStep() const { return drawStep; }

private slots:
    void on_StartPushButton_clicked();
    void on_StopPushButton_clicked();

private:
    Ui::Widget *ui;
    int myTimerID;

    bool isListCreated;//标记是否建立了链表
    ListNode* head;       // 头指针
    ListNode* tail;       // 尾指针  
    ListNode* current;    // 当前操作指针
    int listSize;

    ListNode* drawNode;//当前绘制节点
    int drawStep;//绘画步骤
    bool draw;

    // 操作类型枚举
    enum OperationType {
        OP_NONE,
        OP_SETUP,
        OP_DELETE,
        OP_INSERT,
        OP_FIND
    };
    
    OperationType currentOperation;      // 当前操作类型
    bool isPaused;                      // 是否暂停状态
    
    // 建立链表动画变量
    QStringList setupNumbers;          // 待建立的数字列表
    QString setupType;                  // 建立的链表类型
    int setupCurrentIndex;              // 当前建立到第几个节点
    bool isSetupMode;                   // 是否正在建立模式
    
    // 查找/删除/插入动画变量
    int targetValue;                    // 目标值
    int targetPosition;                 // 目标位置
    ListNode* traverseNode;             // 当前遍历到的节点
    int traverseIndex;                  // 当前遍历索引
    bool operationFound;                // 是否找到目标

    void setupList(const QString& value,const QString& type,const QString& pos);
    void deleteList(const QString& value,const QString& type,const QString& pos);
    void findList(const QString& value,const QString& type,const QString& pos);
    void insertList(const QString& value,const QString& type,const QString& pos);
    void clearList();
    
    // 动画处理方法
    void handleSetupAnimation();
    void handleFindAnimation();
    void handleDeleteAnimation();
    void handleInsertAnimation();

    void drawList(QPainter& painter, int width, int height);
    void drawArrow(QPainter& painter, int startX, int startY, int endX, int endY);
    void drawCurvedArrow(QPainter& painter, int startX, int startY, int endX, int endY);
    void drawPointerArrow(QPainter& painter, int nodeX, int nodeY, int nodeWidth, int nodeHeight, const QString& text, const QColor& color, int offsetY);
    void updateCurrentPointer();
};
#endif // WIDGET_H
