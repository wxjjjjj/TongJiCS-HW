#include "widget.h"
#include "./ui_widget.h"
//#include <QStringList>

// PaintWidget类的实现
PaintWidget::PaintWidget(QWidget *parent) : QWidget(parent), m_mainWidget(nullptr)
{
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);
}

void PaintWidget::setMainWidget(Widget* mainWidget)
{
    m_mainWidget = mainWidget;
}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 清空背景为白色
    painter.fillRect(rect(), Qt::white);
    
    // 绘制测试方块
    //painter.fillRect(50, 50, 100, 100, Qt::red);
    //painter.setPen(QPen(Qt::black, 2));
    //painter.setFont(QFont("Arial", 16));
    //painter.drawText(60, 110, "TEST");
    
    // 如果有主窗口，调用绘制链表
    if (m_mainWidget) {
        m_mainWidget->drawListPublic(painter, width(), height());
    }
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);//根据wdget.ui创建界面
    
    // 用自定义的PaintWidget替换UI中的PaintWidget
    QRect paintGeometry = ui->PaintWidget->geometry();//！！！非常关键！！！
    delete ui->PaintWidget;  // 删除原来的widget
    
    ui->PaintWidget = new PaintWidget(this);
    ui->PaintWidget->setGeometry(paintGeometry);
    static_cast<PaintWidget*>(ui->PaintWidget)->setMainWidget(this);
    
    //1.链表类型
    ui->TypeComboBox->addItem("单链表",static_cast<int>(_SingleList));
    ui->TypeComboBox->addItem("循环链表",static_cast<int>(_CycleList));
    ui->TypeComboBox->addItem("双向链表",static_cast<int>(_DoubleList));
    //2.操作类型
    ui->OPTypeComboBox->addItem("建立",static_cast<int>(_SetUp));
    ui->OPTypeComboBox->addItem("删除",static_cast<int>(_Delete));
    ui->OPTypeComboBox->addItem("查找",static_cast<int>(_Find));
    ui->OPTypeComboBox->addItem("插入",static_cast<int>(_Insert));

    isListCreated=false;//窗口初始时默认未建立列表
    head=nullptr;
    tail=nullptr;
    current=nullptr;
    myTimerID=0;
    draw=false;
    listSize=-1;
    drawNode=nullptr;
    drawStep=0;
    
    // 初始化操作状态
    currentOperation = OP_NONE;
    isPaused = false;
    setupCurrentIndex = 0;
    isSetupMode = false;
    traverseNode = nullptr;
    traverseIndex = 0;
    operationFound = false;
    
    ui->CPPLineEdit->setText("(1)建立:通过输入序列创建链表;(2)插入:在指定位置插入新元素;(3)删除:删除指定位置的元素;(4)查找:按值查找元素位置");
}

Widget::~Widget()
{
    clearList();
    delete ui;
}

void Widget::clearList()
{
    if(head==nullptr){
        return;
    }
    QString type=ui->TypeComboBox->currentText();

    if(type=="循环链表"&&head->next!=head){
        ListNode* tmp=head;
        while(tmp->next!=head){
            tmp=tmp->next;
        }
        tmp->next=nullptr;
    }
    while(head!=nullptr){
        ListNode* tmp=head;
        head=head->next;
        delete tmp;
    }
    head=nullptr;
    tail=nullptr;
    current=nullptr;
    listSize=-1;
    drawNode=nullptr;
    drawStep=0;
    
    // 重置操作状态
    currentOperation = OP_NONE;
    isPaused = false;
    setupCurrentIndex = 0;
    isSetupMode = false;
    setupNumbers.clear();
    setupType.clear();
    traverseNode = nullptr;
    traverseIndex = 0;
    operationFound = false;
    
    // 重置按钮文本
    ui->StartPushButton->setText("开始");
    ui->StopPushButton->setText("暂停");
    
    ui->TypeComboBox->setEnabled(true);
}

void Widget::on_StartPushButton_clicked()
{
    //开始根据TypeComboBox、OPTypeComboBox的选择进行线性表可视化
    QString value = ui->ValueLineEdit->text();//获取输入值
    QString op = ui->OPTypeComboBox->currentText();//获取操作类型
    QString type = ui->TypeComboBox->currentText();//获取链表类型
    QString pos = ui->PositionLineEdit->text();//获取输入位置

    if(isListCreated==0&&op!="建立"){
        QMessageBox::warning(this, "提示", "请先建立链表!");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    else if(isListCreated==1&&op=="建立"){
        QMessageBox::warning(this, "提示", "请不要重复建立链表!");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }

    drawStep=0;
    drawNode=nullptr;
    //清除之前的高亮
    ListNode*tmp=head;
    if(tmp!=nullptr){
        do{
            tmp->isHighlighted=false;
            tmp=tmp->next;
        }while(tmp!=nullptr&&tmp!=head);
    }

    if(op=="建立"){
        setupList(value,type,pos);
    }
    else if(op=="删除"){
        deleteList(value,type,pos);
    }
    else if(op=="插入"){
        insertList(value,type,pos);
    }
    else{
        findList(value,type,pos);//"查找"操作
    }
    //开启定时器，返回定时器编号
    if(myTimerID==0){
        myTimerID=this->startTimer(TIMEOUT);//触发事件--调用timerEvent函数
        draw=true;//开始动画演示了
    }
    updateCurrentPointer();
}

void Widget::setupList(const QString& value,const QString& type,const QString& pos)
{
    // 解析输入的数字序列
    QStringList numbers = value.split(QRegularExpression("[^0-9]+"), Qt::SkipEmptyParts);

    if (value.isEmpty()) {
        QMessageBox::warning(this, "提示", "建立链表的输入序列不能为空！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    if(numbers.isEmpty()){
        QMessageBox::warning(this, "提示", "输入中必需包含数字!");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    
    // 检查节点数量限制
    const int MAX_NODES = 16;
    if(numbers.size() > MAX_NODES) {
        QMessageBox::warning(this, "提示", QString("节点数量不能超过%1个！\n当前输入了%2个节点，请重新输入。").arg(MAX_NODES).arg(numbers.size()));
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    
    clearList();//清理之前的链表

    isListCreated=true;
    // 禁用TypeComboBox，建立后不能再改变链表类型
    ui->TypeComboBox->setEnabled(!isListCreated);
    QMessageBox::information(this, "提示", "已成功建立链表！删除完本链表后才能再次改变链表类型！");

    // 设置建立模式
    currentOperation = OP_SETUP;
    isSetupMode = true;
    setupNumbers = numbers;
    setupType = type;
    setupCurrentIndex = 0;
    listSize = numbers.size();
    
    ui->ValueLineEdit->clear();
    ui->PositionLineEdit->clear();
}

void Widget::deleteList(const QString& value,const QString& type,const QString& pos)
{
    if (pos.isEmpty()) {
        QMessageBox::warning(this, "提示", "删除的位置不能为空！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    // 解析输入的数字序列
    QStringList no = pos.split(QRegularExpression("[^0-9]+"), Qt::SkipEmptyParts);
    if(no.size()!=1){
        QMessageBox::warning(this, "提示", "位置只能为1个数字！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    if(!QRegularExpression("^\\d+$").match(pos).hasMatch()){
        QMessageBox::warning(this, "提示", "输入的位置不能包含字母！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    int position=pos.toInt();
    //ui->ValueLineEdit->setText(QString::number(listSize));
    if(position>=listSize||position<0){
        QMessageBox::warning(this, "提示", "位置不合法！请重新输入！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    if(!value.isEmpty()){
        QMessageBox::information(this, "提示", "请指定删除位置，无需指定值！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }


    // 设置删除模式
    currentOperation = OP_DELETE;
    targetPosition = position;  // 使用位置而不是值
    traverseNode = head;
    traverseIndex = 0;
    operationFound = false;
    current = head;  // current指针从头开始跟随查找
    drawNode = head;

    ui->ValueLineEdit->clear();
    ui->PositionLineEdit->clear();
}

void Widget::findList(const QString& value,const QString& type,const QString& pos)
{
    if(!pos.isEmpty()){
        QMessageBox::information(this, "提示", "请指定查找元素值，无需指定位置！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    // 解析要查找的值
    QStringList no = value.split(QRegularExpression("[^0-9]+"), Qt::SkipEmptyParts);
    if(no.size()!=1){
        QMessageBox::warning(this, "提示", "一次只能查找一个指定元素值！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    bool ok;
    int findValue = value.toInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "提示", "请输入有效的数字！");
        return;
    }
    
    // 设置查找模式
    currentOperation = OP_FIND;
    targetValue = findValue;
    traverseNode = head;
    traverseIndex = 0;
    operationFound = false;
    current = head;  // current指针从头开始跟随查找
    drawNode = head;
    
    ui->ValueLineEdit->clear();
    ui->PositionLineEdit->clear();
}

void Widget::insertList(const QString& value,const QString& type,const QString& pos)
{

    // 解析要插入的值和位置
    bool ok1, ok2;
    int insertValue = value.toInt(&ok1);
    int insertPos = pos.toInt(&ok2);
    // 解析要查找的值
    QStringList no = value.split(QRegularExpression("[^0-9]+"), Qt::SkipEmptyParts);
    if(no.size()!=1){
        QMessageBox::warning(this, "提示", "一次必须只能插入一个值！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    if(!ok1) {
        QMessageBox::warning(this, "提示", "请输入有效的数字值！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    if(!ok2 || insertPos < 0 || insertPos>listSize) {
        QMessageBox::warning(this, "提示", "请输入有效的插入位置！");
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    // 检查节点数量限制
    const int MAX_NODES = 16;
    if(listSize >= MAX_NODES) {
        QMessageBox::warning(this, "警告", QString("链表节点数量已达上限（%1个），无法插入新节点！").arg(MAX_NODES));
        ui->ValueLineEdit->clear();
        ui->PositionLineEdit->clear();
        return;
    }
    // 设置插入模式
    currentOperation = OP_INSERT;
    targetValue = insertValue;
    targetPosition = insertPos;
    traverseNode = head;
    traverseIndex = 0;
    operationFound = false;
    current = head;  // current指针从头开始跟随查找
    drawNode = head;
    
    ui->ValueLineEdit->clear();
    ui->PositionLineEdit->clear();
}

void Widget::updateCurrentPointer()
{
    if(current != nullptr && head != nullptr) {
        // 计算current指针在链表中的位置索引
        int position = 0;
        ListNode* tmp = head;
        QString type = ui->TypeComboBox->currentText();
        
        // 遍历链表查找current指针的位置
        while(tmp != nullptr && tmp != current) {
            tmp = tmp->next;
            position++;
            // 防止循环链表无限循环
            if(type == "循环链表" && tmp == head) {
                break;
            }
        }
        
        if(tmp == current) {
            // 找到了current指针，显示位置
            ui->CPPLineEdit->setText(QString("current指针位置: %1 (节点data值: %2)").arg(position).arg(current->data));
        } else {
            // 没找到current指针（理论上不应该发生）
            ui->CPPLineEdit->setText("位置未知");
        }
    } else {
        ui->CPPLineEdit->setText("NULL");
    }
}

void Widget::timerEvent(QTimerEvent *event)
{
    if(event->timerId()!=myTimerID)
        return;
        
    if(draw){
        // 如果暂停状态，只执行闪烁动画
        if(isPaused) {
            if(drawNode != nullptr) {
                drawNode->isHighlighted = (drawStep % 2 == 0);
            }
            ui->PaintWidget->update();
            drawStep++;
            return;
        }
        
        // 根据当前操作类型执行相应动画
        switch(currentOperation) {
            case OP_SETUP:
                handleSetupAnimation();
                break;
            case OP_FIND:
                handleFindAnimation();
                break;
            case OP_DELETE:
                handleDeleteAnimation();
                break;
            case OP_INSERT:
                handleInsertAnimation();
                break;
            default:
                // 没有操作进行时，让current指针指向的节点闪烁
                if(current != nullptr && isListCreated) {
                    // 先清除所有高亮
                    ListNode* tmp = head;
                    if(tmp != nullptr) {
                        do {
                            tmp->isHighlighted = false;
                            tmp = tmp->next;
                        } while(tmp != nullptr && tmp != head);
                    }
                    // 让current指向的节点闪烁
                    current->isHighlighted = (drawStep % 2 == 0);
                } else if(drawNode != nullptr) {
                    // 兼容原来的闪烁逻辑
                    drawNode->isHighlighted = (drawStep % 2 == 0);
                }
                break;
        }
        
        ui->PaintWidget->update();
        drawStep++;
    }
}

void Widget::handleSetupAnimation()
{
    if(setupCurrentIndex < setupNumbers.size()) {
        // 添加下一个节点
        int num = setupNumbers[setupCurrentIndex].toInt();
        ListNode* newNode = new ListNode(num);
        
        if(head == nullptr) {
            // 第一个节点
            head = newNode;
            tail = newNode;
            current = head;  // current指针跟随新节点
            drawNode = head;
            if(setupType == "循环链表") {
                head->next = head;
            }
        } else {
            // 后续节点
            tail->next = newNode;
            
            if(setupType == "循环链表") {
                newNode->next = head;
            } else if(setupType == "双向链表") {
                newNode->prev = tail;
            }
            
            tail = newNode;
            drawNode = newNode;
            current = newNode;  // current指针跟随新节点
        }
        
        // 高亮新添加的节点
        newNode->isHighlighted = true;
        setupCurrentIndex++;
        updateCurrentPointer();
        
        // 检查是否完成
        if(setupCurrentIndex >= setupNumbers.size()) {
            currentOperation = OP_NONE;
            isListCreated = true;
            isPaused = false;
            ui->StartPushButton->setText("开始");
            ui->StopPushButton->setText("暂停");
            ui->TypeComboBox->setEnabled(false);
        }
    }
}

void Widget::handleFindAnimation()
{
    static QStringList foundPositions; // 记录找到的位置
    static bool searchCompleted = false; // 是否完成搜索
    static bool showResult = false; // 是否需要显示结果
    
    // 如果需要显示结果，先显示结果再重置
    if(showResult) {
        if(foundPositions.isEmpty()) {
            QMessageBox::information(this, "提示", QString("未找到指定元素值 %1").arg(targetValue));
        } else {
            QString message = QString("找到指定元素值 %1 在以下位置：%2").arg(targetValue).arg(foundPositions.join(", "));
            QMessageBox::information(this, "提示", message);
        }
        
        // 重置所有状态
        foundPositions.clear();
        searchCompleted = false;
        showResult = false;
        currentOperation = OP_NONE;
        return;
    }
    
    if(traverseNode != nullptr && !searchCompleted) {
        // 清除之前的高亮
        ListNode* tmp = head;
        if(tmp != nullptr) {
            do {
                tmp->isHighlighted = false;
                tmp = tmp->next;
            } while(tmp != nullptr && tmp != head);
        }
        
        // 高亮当前检查的节点
        traverseNode->isHighlighted = true;
        drawNode = traverseNode;
        current = traverseNode;  // current指针跟随查找过程
        updateCurrentPointer();
        
        // 检查是否找到目标
        if(traverseNode->data == targetValue) {
            foundPositions.append(QString::number(traverseIndex));
        }
        
        // 移动到下一个节点
        traverseNode = traverseNode->next;
        traverseIndex++;
        
        // 检查是否遍历完成
        if(traverseNode == nullptr || (ui->TypeComboBox->currentText() == "循环链表" && traverseNode == head)) {
            searchCompleted = true;
            showResult = true; // 标记下次需要显示结果
        }
    }
}

void Widget::handleDeleteAnimation()
{
    static bool readyToDelete = false; // 是否准备删除
    static int waitCounter = 0; // 等待计数器
    
    // 如果已经准备删除，等待几帧让用户看到current指针位置
    if(readyToDelete) {
        waitCounter++;
        if(waitCounter >= 1) { // 等待3帧
            // 执行实际的删除操作
            ListNode* nodeToDelete = traverseNode;
            QString type = ui->TypeComboBox->currentText();
            
            // 找到前一个节点
            ListNode* prevNode = nullptr;
            if(nodeToDelete != head) {
                // 不是头节点，找前一个节点
                prevNode = head;
                while(prevNode->next != nodeToDelete) {
                    prevNode = prevNode->next;
                }
            }
            
            // 根据链表类型执行删除
            if(type == "单链表") {
                if(nodeToDelete == head) {
                    // 删除头节点
                    head = head->next;
                    if(head == nullptr) tail = nullptr;
                } else {
                    // 删除非头节点
                    prevNode->next = nodeToDelete->next;
                    if(nodeToDelete == tail) tail = prevNode;
                }
            } else if(type == "循环链表") {
                if(listSize == 1) {
                    // 只有一个节点
                    head = nullptr;
                    tail = nullptr;
                } else if(nodeToDelete == head) {
                    // 删除头节点
                    tail->next = head->next;
                    head = head->next;
                } else {
                    // 删除非头节点
                    prevNode->next = nodeToDelete->next;
                    if(nodeToDelete == tail) {
                        tail = prevNode;
                        tail->next = head;
                    }
                }
            } else if(type == "双向链表") {
                if(nodeToDelete == head && nodeToDelete == tail) {
                    // 只有一个节点
                    head = nullptr;
                    tail = nullptr;
                } else if(nodeToDelete == head) {
                    // 删除头节点
                    head = head->next;
                    head->prev = nullptr;
                } else if(nodeToDelete == tail) {
                    // 删除尾节点
                    tail = tail->prev;
                    tail->next = nullptr;
                } else {
                    // 删除中间节点
                    nodeToDelete->prev->next = nodeToDelete->next;
                    nodeToDelete->next->prev = nodeToDelete->prev;
                }
            }
            
            // 更新链表大小
            listSize--;
            
            // 设置current指针
            if(head == nullptr) {
                current = nullptr;
            } else if(nodeToDelete == current) {
                // 如果删除的是current指向的节点
                if(nodeToDelete->next != nullptr && nodeToDelete->next != head) {
                    current = nodeToDelete->next;
                } else if(prevNode != nullptr) {
                    current = prevNode;
                } else {
                    current = head;
                }
            }
            
            // 删除节点
            delete nodeToDelete;
            
            // 检查链表是否为空
            if(head == nullptr) {
                QMessageBox::information(this, "提示", "节点删除成功！链表现在为空。");
                isListCreated = false;
                ui->TypeComboBox->setEnabled(true);
            }
            updateCurrentPointer();
            
            // 重置状态
            readyToDelete = false;
            waitCounter = 0;
            currentOperation = OP_NONE;
        }
        return;
    }
    
    if(traverseNode != nullptr && !operationFound) {
        // 清除之前的高亮
        ListNode* tmp = head;
        if(tmp != nullptr) {
            do {
                tmp->isHighlighted = false;
                tmp = tmp->next;
            } while(tmp != nullptr && tmp != head);
        }
        
        // 高亮当前检查的节点
        traverseNode->isHighlighted = true;
        drawNode = traverseNode;
        current = traverseNode;  // current指针跟随查找过程
        updateCurrentPointer();
        
        // 检查是否到达目标位置
        if(traverseIndex == targetPosition) {
            operationFound = true;
            readyToDelete = true;
            waitCounter = 0;
        } else {
            // 移动到下一个节点
            traverseNode = traverseNode->next;
            traverseIndex++;
            
            // 检查是否遍历完成
            if(traverseNode == nullptr || (ui->TypeComboBox->currentText() == "循环链表" && traverseNode == head)) {
                QMessageBox::information(this, "提示", QString("位置 %1 超出链表范围").arg(targetPosition));
                currentOperation = OP_NONE;
            }
        }
    }
}

void Widget::handleInsertAnimation()
{
    static bool readyToInsert = false; // 是否准备插入
    static int waitCounter = 0; // 等待计数器
    
    // 如果已经准备插入，等待几帧让用户看到current指针位置
    if(readyToInsert) {
        waitCounter++;
        if(waitCounter >= 1) { // 等待1帧
            // 创建新节点
            ListNode* newNode = new ListNode(targetValue);
            QString type = ui->TypeComboBox->currentText();
            
            if(targetPosition == 0) {
                // 在头部插入
                if(type == "单链表") {
                    newNode->next = head;
                    head = newNode;
                    if(tail == nullptr) tail = newNode;
                } else if(type == "循环链表") {
                    if(head == nullptr) {
                        head = newNode;
                        tail = newNode;
                        newNode->next = head;
                    } else {
                        newNode->next = head;
                        tail->next = newNode;
                        head = newNode;
                    }
                } else if(type == "双向链表") {
                    newNode->next = head;
                    if(head != nullptr) head->prev = newNode;
                    head = newNode;
                    if(tail == nullptr) tail = newNode;
                }
            } else {
                // 在指定位置插入（尾插法）
                ListNode* prevNode = head;
                for(int i = 0; i < targetPosition - 1 && prevNode != nullptr; i++) {
                    prevNode = prevNode->next;
                    if(type == "循环链表" && prevNode == head) break;
                }
                
                if(prevNode == nullptr || (type == "循环链表" && targetPosition > listSize)) {
                    // 位置超出范围，在尾部插入
                    if(type == "单链表") {
                        tail->next = newNode;
                        tail = newNode;
                    } else if(type == "循环链表") {
                        tail->next = newNode;
                        newNode->next = head;
                        tail = newNode;
                    } else if(type == "双向链表") {
                        tail->next = newNode;
                        newNode->prev = tail;
                        tail = newNode;
                    }
                } else {
                    // 在prevNode后面插入
                    if(type == "单链表") {
                        newNode->next = prevNode->next;
                        prevNode->next = newNode;
                        if(prevNode == tail) tail = newNode;
                    } else if(type == "循环链表") {
                        newNode->next = prevNode->next;
                        prevNode->next = newNode;
                        if(prevNode == tail) {
                            tail = newNode;
                            newNode->next = head;
                        }
                    } else if(type == "双向链表") {
                        newNode->next = prevNode->next;
                        newNode->prev = prevNode;
                        if(prevNode->next != nullptr) {
                            prevNode->next->prev = newNode;
                        }
                        prevNode->next = newNode;
                        if(prevNode == tail) tail = newNode;
                    }
                }
            }
            
            // 更新链表大小
            listSize++;
            
            // 设置current指针指向新插入的节点
            current = newNode;
            newNode->isHighlighted = true;
            
            updateCurrentPointer();
            
            // 重置状态
            readyToInsert = false;
            waitCounter = 0;
            currentOperation = OP_NONE;
        }
        return;
    }
    
    if(traverseIndex <= targetPosition && traverseNode != nullptr) {
        // 清除之前的高亮
        ListNode* tmp = head;
        if(tmp != nullptr) {
            do {
                tmp->isHighlighted = false;
                tmp = tmp->next;
            } while(tmp != nullptr && tmp != head);
        }
        
        // 高亮当前遍历的节点
        traverseNode->isHighlighted = true;
        drawNode = traverseNode;
        current = traverseNode;  // current指针跟随遍历过程
        updateCurrentPointer();
        
        // 如果已经到达目标位置，准备插入
        if(traverseIndex == targetPosition) {
            readyToInsert = true;
            waitCounter = 0;
        } else {
            // 移动到下一个位置
            traverseNode = traverseNode->next;
            traverseIndex++;
            
            // 检查是否到达链表末尾但还没到目标位置
            if(traverseNode == nullptr || (ui->TypeComboBox->currentText() == "循环链表" && traverseNode == head)) {
                // 已到末尾，准备在尾部插入
                readyToInsert = true;
                waitCounter = 0;
            }
        }
    } else {
        // 到达插入位置，准备插入
        readyToInsert = true;
        waitCounter = 0;
    }
}

void Widget::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);
    // 主窗口不需要绘制任何内容，绘制都在PaintWidget中进行
}

void Widget::drawList(QPainter& painter,int width,int height)
{
    if(head==nullptr) return;

    qDebug() << "drawList called, width:" << width << ", height:" << height;

    //绘制图形的大小
    const int nodeWidth=60;
    const int nodeHeight=40;
    const int spacing=75;  // 减小间距
    const int startX=50;
    const int startY=80;   // 从更靠上的位置开始
    const int maxNodesPerRow = (width - 100) / spacing;  // 每行最多显示的节点数
    const int rowSpacing = 90;  // 减小行间距

    qDebug() << "Drawing position - startX:" << startX << ", startY:" << startY;
    qDebug() << "maxNodesPerRow:" << maxNodesPerRow;

    QString type=ui->TypeComboBox->currentText();
    ListNode* tmp=head;
    int totalNodes=0;

    // 先计算总节点数
    do{
        totalNodes++;
        tmp=tmp->next;
        if(totalNodes>=50) break;  // 防止无限循环，最多显示50个节点
    }while(tmp!=nullptr&&tmp!=head);

    qDebug() << "totalNodes:" << totalNodes;

    tmp=head;
    for(int i=0;i<totalNodes&&tmp!=nullptr;i++){
        // 计算当前节点在第几行第几列 - 蛇形布局
        int row = i / maxNodesPerRow;
        int col;
        
        if(row % 2 == 0) {
            // 偶数行：从左到右
            col = i % maxNodesPerRow;
        } else {
            // 奇数行：从右到左
            col = maxNodesPerRow - 1 - (i % maxNodesPerRow);
        }
        
        int nodeX = startX + col * spacing;
        int nodeY = startY + row * rowSpacing - nodeHeight/2;

        qDebug() << "Node" << i << "position: row=" << row << ", col=" << col << ", x=" << nodeX << ", y=" << nodeY << ", value=" << tmp->data;

        // 如果超出显示区域，停止绘制
        if(nodeY + nodeHeight > height - 50) break;

        QColor nodeColor=Qt::lightGray;
        if(tmp==current){
            nodeColor=QColor(13, 61, 193);
        }
        if(tmp->isHighlighted){
            nodeColor=QColor(161, 207, 242);
        }
        //绘制节点
        painter.setBrush(nodeColor);
        painter.setPen(QPen(Qt::black,2));
        painter.drawRect(nodeX,nodeY,nodeWidth,nodeHeight);
        //绘制节点值
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial",12,QFont::Bold));
        painter.drawText(QRect(nodeX,nodeY,nodeWidth,nodeHeight),Qt::AlignCenter,QString::number(tmp->data));
        
        //绘制指针箭头
        if(type=="单链表"){
            if(tmp->next!=nullptr && i < totalNodes-1){
                // 计算下一个节点的位置
                int nextRow = (i+1) / maxNodesPerRow;
                int nextCol;
                if(nextRow % 2 == 0) {
                    nextCol = (i+1) % maxNodesPerRow;
                } else {
                    nextCol = maxNodesPerRow - 1 - ((i+1) % maxNodesPerRow);
                }
                int nextX = startX + nextCol * spacing;
                int nextY = startY + nextRow * rowSpacing - nodeHeight/2;
                
                if(row == nextRow) {
                    // 同一行，绘制水平箭头
                    if(row % 2 == 0) {
                        // 偶数行：从左到右
                        drawArrow(painter,nodeX+nodeWidth,nodeY+nodeHeight/2,nextX,nextY+nodeHeight/2);
                    } else {
                        // 奇数行：从右到左
                        drawArrow(painter,nodeX,nodeY+nodeHeight/2,nextX+nodeWidth,nextY+nodeHeight/2);
                    }
                } else {
                    // 换行，绘制转折箭头
                    if(row % 2 == 0) {
                        // 从偶数行末尾到奇数行开头
                        drawArrow(painter, nodeX+nodeWidth/2, nodeY+nodeHeight, nodeX+nodeWidth/2, nextY);
                    } else {
                        // 从奇数行末尾到偶数行开头
                        drawArrow(painter, nodeX+nodeWidth/2, nodeY+nodeHeight, nodeX+nodeWidth/2, nextY);
                    }
                }
            }
        }
        else if(type=="循环链表"){
            if(tmp->next!=nullptr && tmp->next!=head && i < totalNodes-1){
                // 普通的节点间连接（不包括最后一个节点到第一个节点）
                // 计算下一个节点的位置
                int nextRow = (i+1) / maxNodesPerRow;
                int nextCol;
                if(nextRow % 2 == 0) {
                    nextCol = (i+1) % maxNodesPerRow;
                } else {
                    nextCol = maxNodesPerRow - 1 - ((i+1) % maxNodesPerRow);
                }
                int nextX = startX + nextCol * spacing;
                int nextY = startY + nextRow * rowSpacing - nodeHeight/2;
                
                if(row == nextRow) {
                    // 同一行，绘制水平箭头
                    if(row % 2 == 0) {
                        // 偶数行：从左到右
                        drawArrow(painter,nodeX+nodeWidth,nodeY+nodeHeight/2,nextX,nextY+nodeHeight/2);
                    } else {
                        // 奇数行：从右到左
                        drawArrow(painter,nodeX,nodeY+nodeHeight/2,nextX+nodeWidth,nextY+nodeHeight/2);
                    }
                } else {
                    // 换行，绘制转折箭头
                    drawArrow(painter, nodeX+nodeWidth/2, nodeY+nodeHeight, nodeX+nodeWidth/2, nextY);
                }
            }
        }
        else{//双向链表
            if(tmp->next!=nullptr && i < totalNodes-1){//向前指针
                // 计算下一个节点的位置
                int nextRow = (i+1) / maxNodesPerRow;
                int nextCol;
                if(nextRow % 2 == 0) {
                    nextCol = (i+1) % maxNodesPerRow;
                } else {
                    nextCol = maxNodesPerRow - 1 - ((i+1) % maxNodesPerRow);
                }
                int nextX = startX + nextCol * spacing;
                int nextY = startY + nextRow * rowSpacing - nodeHeight/2;
                
                if(row == nextRow) {
                    // 同一行的连接
                    if(row % 2 == 0) {
                        drawArrow(painter,nodeX+nodeWidth,nodeY+nodeHeight/3,nextX,nextY+nodeHeight/3);
                    } else {
                        drawArrow(painter,nodeX,nodeY+nodeHeight/3,nextX+nodeWidth,nextY+nodeHeight/3);
                    }
                } else {
                    // 换行的连接 - 绘制向前箭头（从当前节点底部中心向右0.5指向下一个节点顶部中心向右0.5）
                    if(row % 2 == 0) {
                        // 从偶数行末尾到奇数行开头
                        drawArrow(painter, nodeX+nodeWidth/2+0.5, nodeY+nodeHeight, nextX+nodeWidth/2+0.5, nextY);
                    } else {
                        // 从奇数行末尾到偶数行开头
                        drawArrow(painter, nodeX+nodeWidth/2+0.5, nodeY+nodeHeight, nextX+nodeWidth/2+0.5, nextY);
                    }
                }
            }
            if(tmp->prev!=nullptr && i > 0){//向后指针
                // 计算前一个节点的位置
                int prevRow = (i-1) / maxNodesPerRow;
                int prevCol;
                if(prevRow % 2 == 0) {
                    prevCol = (i-1) % maxNodesPerRow;
                } else {
                    prevCol = maxNodesPerRow - 1 - ((i-1) % maxNodesPerRow);
                }
                int prevX = startX + prevCol * spacing;
                int prevY = startY + prevRow * rowSpacing - nodeHeight/2;
                
                if(row == prevRow) {
                    // 同一行的连接
                    if(row % 2 == 0) {
                        drawArrow(painter,nodeX,nodeY+2*nodeHeight/3,prevX+nodeWidth,prevY+2*nodeHeight/3);
                    } else {
                        drawArrow(painter,nodeX+nodeWidth,nodeY+2*nodeHeight/3,prevX,prevY+2*nodeHeight/3);
                    }
                } else {
                    if(prevRow % 2 == 0) {
                        // 从偶数行末尾到奇数行开头
                        drawArrow(painter, nodeX+nodeWidth/2-10, nodeY, prevX+nodeWidth/2-10, prevY+nodeHeight);
                    } else {
                        // 从奇数行末尾到偶数行开头
                        drawArrow(painter, nodeX+nodeWidth/2-10,nodeY, prevX+nodeWidth/2-10, prevY+nodeHeight);
                    }
                }
            }
        }
        tmp=tmp->next;
        if(type=="循环链表"&&tmp==head) break;
    }
    
    //绘制当前指针标识
    if(current!=nullptr){
        tmp=head;
        for(int i=0;i<totalNodes&&tmp!=nullptr;i++){
            if(tmp==current){
                int row = i / maxNodesPerRow;
                int col;
                if(row % 2 == 0) {
                    col = i % maxNodesPerRow;
                } else {
                    col = maxNodesPerRow - 1 - (i % maxNodesPerRow);
                }
                int nodeX = startX + col * spacing;
                int nodeY = startY + row * rowSpacing - nodeHeight/2;
                // 绘制蓝色向上指向的箭头
                drawPointerArrow(painter, nodeX, nodeY, nodeWidth, nodeHeight, "current", QColor(228, 142, 142), -35);
                break;
            }
            tmp=tmp->next;
            if(type=="循环链表"&&tmp==head) break;
        }
    }
    
    //绘制头指针和尾指针标识
    tmp=head;
    for(int i=0;i<totalNodes&&tmp!=nullptr;i++){
        int row = i / maxNodesPerRow;
        int col;
        if(row % 2 == 0) {
            col = i % maxNodesPerRow;
        } else {
            col = maxNodesPerRow - 1 - (i % maxNodesPerRow);
        }
        int nodeX = startX + col * spacing;
        int nodeY = startY + row * rowSpacing - nodeHeight/2;
        
        if(tmp==head){
            //头指针
            drawPointerArrow(painter, nodeX, nodeY, nodeWidth, nodeHeight, "Head", Qt::black, nodeHeight + 25);
        }
        // 只有非循环链表才显示尾指针
        if(tmp==tail && tail != head && type != "循环链表"){  
            // 尾指针
            drawPointerArrow(painter, nodeX, nodeY, nodeWidth, nodeHeight, "Tail", Qt::black, nodeHeight + 55);
        }
        
        tmp=tmp->next;
        if(type=="循环链表"&&tmp==head) break;
    }
    
    // 最后绘制循环链表的回环箭头（只有在链表建立完成且不是建立过程中才绘制）
    if(type=="循环链表" && isListCreated && currentOperation != OP_SETUP) {
        // 计算最后一个节点和第一个节点的位置
        int lastNodeIndex = totalNodes - 1;
        int firstNodeIndex = 0;
        
        // 最后一个节点位置
        int lastRow = lastNodeIndex / maxNodesPerRow;
        int lastCol;
        if(lastRow % 2 == 0) {
            lastCol = lastNodeIndex % maxNodesPerRow;
        } else {
            lastCol = maxNodesPerRow - 1 - (lastNodeIndex % maxNodesPerRow);
        }
        int lastX = startX + lastCol * spacing;
        int lastY = startY + lastRow * rowSpacing - nodeHeight/2;
        
        // 第一个节点位置
        int firstRow = firstNodeIndex / maxNodesPerRow;
        int firstCol;
        if(firstRow % 2 == 0) {
            firstCol = firstNodeIndex % maxNodesPerRow;
        } else {
            firstCol = maxNodesPerRow - 1 - (firstNodeIndex % maxNodesPerRow);
        }
        int firstX = startX + firstCol * spacing;
        int firstY = startY + firstRow * rowSpacing - nodeHeight/2;
        
        // 绘制回环箭头 - 使用曼哈顿折线
        painter.setPen(QPen(Qt::black, 2));
        
        // 根据最后一个节点所在行的奇偶性确定起点
        int startX_arrow, startY_arrow;
        if(lastRow % 2 == 0) {
            // 偶数行：从左到右排列，从节点右边开始
            startX_arrow = lastX + nodeWidth;
            startY_arrow = lastY + nodeHeight/2;
        } else {
            // 奇数行：从右到左排列，从节点左边开始
            startX_arrow = lastX;
            startY_arrow = lastY + nodeHeight/2;
        }
        
        // 终点：根据第一个节点所在行的方向确定箭头指向位置
        int endX_arrow, endY_arrow;
        if(firstRow % 2 == 0) {
            // 偶数行：从左到右排列，箭头指向左边
            endX_arrow = firstX;
            endY_arrow = firstY + nodeHeight/2;
        } else {
            // 奇数行：从右到左排列，箭头指向右边
            endX_arrow = firstX + nodeWidth;
            endY_arrow = firstY + nodeHeight/2;
        }
        
        // 绘制曼哈顿折线（直角折线）
        // 第一段：向外延伸
        int extendDistance = 30;
        int midX;
        if(lastRow % 2 == 0) {
            // 偶数行：向右延伸
            midX = startX_arrow + extendDistance;
            painter.drawLine(startX_arrow, startY_arrow, midX, startY_arrow);
        } else {
            // 奇数行：向左延伸
            midX = startX_arrow - extendDistance;
            painter.drawLine(startX_arrow, startY_arrow, midX, startY_arrow);
        }
        
        // 第二段：向上弯曲
        int bendY;
        if(lastRow == firstRow) {
            // 同一行，向上弯曲
            bendY = startY_arrow - 50;
        } else {
            // 不同行，向第一个节点上方弯曲
            bendY = firstY - 30;
        }
        painter.drawLine(midX, startY_arrow, midX, bendY);
        
        // 第三段：水平向第一个节点侧面上方
        int finalX;
        if(firstRow % 2 == 0) {
            // 偶数行：向左边一点位置
            finalX = firstX - 10;
        } else {
            // 奇数行：向右边一点位置
            finalX = firstX + nodeWidth + 10;
        }
        painter.drawLine(midX, bendY, finalX, bendY);
        
        // 第四段：向下到第一个节点侧面中点
        painter.drawLine(finalX, bendY, finalX, endY_arrow);
        
        // 第五段：向第一个节点指向
        painter.drawLine(finalX, endY_arrow, endX_arrow, endY_arrow);
        
        // 绘制箭头头部 - 根据第一个节点所在行的方向确定箭头方向
        const int arrowSize = 8;
        if(firstRow % 2 == 0) {
            // 偶数行：从左到右，箭头向右指向节点左边
            painter.drawLine(endX_arrow, endY_arrow, endX_arrow - arrowSize, endY_arrow - arrowSize/2);
            painter.drawLine(endX_arrow, endY_arrow, endX_arrow - arrowSize, endY_arrow + arrowSize/2);
        } else {
            // 奇数行：从右到左，箭头向左指向节点右边
            painter.drawLine(endX_arrow, endY_arrow, endX_arrow + arrowSize, endY_arrow - arrowSize/2);
            painter.drawLine(endX_arrow, endY_arrow, endX_arrow + arrowSize, endY_arrow + arrowSize/2);
        }
    }
}

void Widget::drawArrow(QPainter& painter, int startX, int startY, int endX, int endY)
{
    painter.setPen(QPen(Qt::black,2));
    painter.drawLine(startX,startY,endX,endY);

    const int arrowSize=8;
    double angle=atan2(endY-startY,endX-startX);

    int arrowX1=endX-arrowSize*cos(angle-M_PI/6);
    int arrowY1=endY-arrowSize*sin(angle-M_PI/6);
    int arrowX2=endX-arrowSize*cos(angle+M_PI/6);
    int arrowY2=endY-arrowSize*sin(angle+M_PI/6);

    painter.drawLine(endX,endY,arrowX1,arrowY1);
    painter.drawLine(endX,endY,arrowX2,arrowY2);
}

void Widget::drawCurvedArrow(QPainter& painter, int startX, int startY, int endX, int endY)
{
    painter.setPen(QPen(Qt::black,2));
    //弧形箭头
    int midX=(startX+endX)/2;
    int midY=startY-60;

    QPainterPath path;
    path.moveTo(startX,startY);
    path.quadTo(midX,midY,endX,endY);
    painter.drawPath(path);

    const int arrowSize=8;
    painter.drawLine(endX,endY,endX-arrowSize,endY-arrowSize/2);
    painter.drawLine(endX,endY,endX-arrowSize,endY+arrowSize/2);
}

void Widget::drawPointerArrow(QPainter& painter, int nodeX, int nodeY, int nodeWidth, int nodeHeight, const QString& text, const QColor& color, int offsetY)
{
    painter.setPen(QPen(color, 2));
    painter.setBrush(color);
    
    int centerX = nodeX + nodeWidth / 2;
    int startY, endY;
    
    if (offsetY < 0) {
        // 向上指向的箭头 - 简洁风格
        endY = nodeY;
        startY = nodeY - 30;
        
        // 绘制箭头线
        painter.drawLine(centerX, startY, centerX, endY);
        
        // 绘制箭头头部 - 简单的线条形式，和链表箭头一样
        painter.drawLine(centerX, endY, centerX - 5, endY - 8);
        painter.drawLine(centerX, endY, centerX + 5, endY - 8);
        
        // 绘制文字
        painter.setPen(QPen(color, 1));
        painter.setFont(QFont("Arial", 9));
        QFontMetrics fm(painter.font());
        int textWidth = fm.horizontalAdvance(text);
        painter.drawText(centerX - textWidth/2, startY - 5, text);
        
    } else {
        // 向下指向的箭头 - 简洁风格
        startY = nodeY + nodeHeight;
        endY = nodeY + nodeHeight + 30;
        
        // 绘制箭头线
        painter.drawLine(centerX, startY, centerX, endY);
        
        // 绘制箭头头部 - 简单的线条形式，和链表箭头一样
        painter.drawLine(centerX, startY, centerX - 5, startY + 8);
        painter.drawLine(centerX, startY, centerX + 5, startY + 8);
        
        // 绘制文字
        painter.setPen(QPen(color, 1));
        painter.setFont(QFont("Arial", 9));
        QFontMetrics fm(painter.font());
        int textWidth = fm.horizontalAdvance(text);
        painter.drawText(centerX - textWidth/2, endY + 15, text);
    }
}

void Widget::drawListPublic(QPainter& painter, int width, int height)
{
    if(draw && head != nullptr) {
        drawList(painter, width, height);
    }
}

// ========== 指针操作方法实现 ==========

void Widget::moveCurrentNext()
{
    if(current == nullptr || current->next == nullptr) return;
    
    QString type = ui->TypeComboBox->currentText();
    if(type == "循环链表" || current->next != head) {
        current = current->next;
        updateCurrentPointer();
    }
}

void Widget::moveCurrentPrev()
{
    if(current == nullptr) return;
    
    QString type = ui->TypeComboBox->currentText();
    if(type == "双向链表" && current->prev != nullptr) {
        // 双向链表可以直接向前移动
        current = current->prev;
        updateCurrentPointer();
    } else {
        // 单链表和循环链表需要从头遍历
        if(current == head) return; // 已经是头节点，无法再向前
        
        ListNode* tmp = head;
        while(tmp != nullptr && tmp->next != current) {
            tmp = tmp->next;
            if(type == "循环链表" && tmp == head) break;
        }
        if(tmp != nullptr && tmp->next == current) {
            current = tmp;
            updateCurrentPointer();
        }
    }
}

void Widget::moveCurrentToHead()
{
    if(head != nullptr) {
        current = head;
        updateCurrentPointer();
    }
}

void Widget::moveCurrentToTail()
{
    if(tail != nullptr) {
        current = tail;
        updateCurrentPointer();
    }
}

void Widget::moveCurrentToPosition(int pos)
{
    if(head == nullptr || pos < 0) return;
    
    ListNode* tmp = head;
    QString type = ui->TypeComboBox->currentText();
    
    for(int i = 0; i < pos && tmp != nullptr; i++) {
        tmp = tmp->next;
        if(type == "循环链表" && tmp == head) break;
    }
    
    if(tmp != nullptr) {
        current = tmp;
        updateCurrentPointer();
    }
}

void Widget::on_StopPushButton_clicked()
{
    // 检查是否有动画正在运行
    if(currentOperation == OP_NONE) {
        QMessageBox::information(this, "提示", "当前没有正在进行的动画");
        return;
    }
    
    // 检查是否为暂停状态，如果是则继续
    if(isPaused) {
        isPaused = false;
        ui->StopPushButton->setText("暂停");
    } else {
        // 暂停动画
        isPaused = true;
        ui->StopPushButton->setText("继续");
    }
}
