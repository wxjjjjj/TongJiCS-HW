#include "doctor.h"
#include "ui_doctor.h"
#include "widget.h"  // 包含 GlobalRegistrationManager
#include <QMessageBox>
#include <QTime>
#include <cstdlib>

Doctor::Doctor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Doctor)
    , nextPatientModel(new QStringListModel(this))
    , previousPatientModel(new QStringListModel(this))
    , diagnosisTimer(new QTimer(this))
    , isDiagnosing(false)
{
    ui->setupUi(this);
    
    // 设置列表视图的模型
    ui->NextPatientListView->setModel(nextPatientModel);
    ui->PreviousPatientListView->setModel(previousPatientModel);
    
    // 连接计时器信号
    connect(diagnosisTimer, &QTimer::timeout, this, &Doctor::updateTimer);
    
    // 初始化时间显示
    ui->TimeLineEdit->setText("00:00:00");
    
    //初始化
    ui->DepartmentComboBox->addItem("科室1",static_cast<int>(_Department21));
    ui->DepartmentComboBox->addItem("科室2",static_cast<int>(_Department22));
    ui->DepartmentComboBox->addItem("科室3",static_cast<int>(_Department23));
    ui->DepartmentComboBox->addItem("科室4",static_cast<int>(_Department24));
    ui->DepartmentComboBox->addItem("科室5",static_cast<int>(_Department25));
    ui->DepartmentComboBox->addItem("科室6",static_cast<int>(_Department26));
    ui->DepartmentComboBox->addItem("科室7",static_cast<int>(_Department27));
    ui->DepartmentComboBox->addItem("科室8",static_cast<int>(_Department28));
    ui->DepartmentComboBox->addItem("科室9",static_cast<int>(_Department29));
    ui->DepartmentComboBox->addItem("科室10",static_cast<int>(_Department210));
    ui->DepartmentComboBox->addItem("科室11",static_cast<int>(_Department211));
    ui->DepartmentComboBox->addItem("科室12",static_cast<int>(_Department212));
    ui->DepartmentComboBox->addItem("科室13",static_cast<int>(_Department213));
    ui->DepartmentComboBox->addItem("科室14",static_cast<int>(_Department214));
    ui->DepartmentComboBox->addItem("科室15",static_cast<int>(_Department215));

    ui->DoctorComboBox->addItem("医生A",static_cast<int>(_Doctor2A));
    ui->DoctorComboBox->addItem("医生B",static_cast<int>(_Doctor2B));
    ui->DoctorComboBox->addItem("医生C",static_cast<int>(_Doctor2C));
}

Doctor::~Doctor()
{
    delete ui;
}

void Doctor::on_PushButton_clicked()
{
    this->hide();
    emit DoctorReturnToMain();
}

void Doctor::on_CheckPushButton_clicked()
{
    // 获取选定的科室和医生
    int department = ui->DepartmentComboBox->currentData().toInt();
    int doctor = ui->DoctorComboBox->currentData().toInt();
    
    GlobalRegistrationManager& manager = GlobalRegistrationManager::instance();
    
    // 获取未看诊患者列表
    QList<PatientR> waitingPatients = manager.getQueueByDepartmentAndDoctor(department, doctor);
    QStringList waitingList;
    for (const auto& patient : waitingPatients) {
        waitingList << QString("患者：%1  身份证号：%2 ")
                           .arg(patient.name)
                           .arg(patient.idCard);
    }
    nextPatientModel->setStringList(waitingList);
    
    // 获取已看诊患者列表
    QList<PatientR> allPatients = manager.getAllRegistrations();
    QStringList completedList;
    for (const auto& patient : allPatients) {
        if (patient.department == department && patient.doctor == doctor && patient.isCompleted) {
            QString diagnosisTimeText = patient.diagnosisTime.isEmpty() ? "未记录" : patient.diagnosisTime;
            completedList << QString("患者：%1  身份证号：%2  听诊用时：%3")
                                 .arg(patient.name)
                                 .arg(patient.idCard)
                                 .arg(diagnosisTimeText);
        }
    }
    previousPatientModel->setStringList(completedList);
    
    // 显示当前正在看诊的患者（未看诊患者中的第一个）
    if (!waitingPatients.isEmpty()) {
        // 找到排队号最小的患者
        PatientR currentPatient = waitingPatients[0];
        for (const auto& patient : waitingPatients) {
            if (patient.queueNumber < currentPatient.queueNumber) {
                currentPatient = patient;
            }
        }
        ui->PatientNameLineEdit->setText(QString("%1 (%2)")
                                        .arg(currentPatient.name)
                                        .arg(currentPatient.idCard));
    } else {
        ui->PatientNameLineEdit->setText("无患者");
    }
}

void Doctor::on_StartPushButton_clicked()
{
    // 检查是否有患者正在等待
    if (ui->PatientNameLineEdit->text() == "无患者" || ui->PatientNameLineEdit->text().isEmpty()) {
        QMessageBox::information(this, "提示", "当前没有患者，请先查看患者列表！");
        return;
    }
    
    if (isDiagnosing) {
        QMessageBox::information(this, "提示", "正在诊断中，请先结束当前诊断！");
        return;
    }
    
    // 开始计时
    startTime = QTime::currentTime();
    diagnosisTimer->start(1000); // 每秒更新一次
    isDiagnosing = true;
    
    // 从患者姓名中提取身份证号
    QString patientInfo = ui->PatientNameLineEdit->text();
    int startPos = patientInfo.indexOf('(');
    int endPos = patientInfo.indexOf(')');
    if (startPos != -1 && endPos != -1) {
        currentPatientId = patientInfo.mid(startPos + 1, endPos - startPos - 1);
    }
    
    QMessageBox::information(this, "开始诊断", "已开始为患者诊断");
}

void Doctor::on_BPushButton_clicked()
{
    if (!isDiagnosing) {
        QMessageBox::information(this, "提示", "请先开始诊断！");
        return;
    }
    
    // 停止计时并获取听诊时间
    diagnosisTimer->stop();
    isDiagnosing = false;
    QString diagnosisTime = ui->TimeLineEdit->text();
    
    GlobalRegistrationManager& manager = GlobalRegistrationManager::instance();
    
    // 将患者分配到B超室并标记为已看诊
    int ultrasoundRoom = manager.assignToUltrasound(currentPatientId, diagnosisTime);
    if (ultrasoundRoom > 0) {
        QString roomName;
        switch (ultrasoundRoom) {
            case 1: roomName = "A号机"; break;
            case 2: roomName = "B号机"; break;
            case 3: roomName = "C号机"; break;
            default: roomName = QString("%1号机").arg(ultrasoundRoom); break;
        }
        QMessageBox::information(this, "诊断完成", 
                                QString("患者诊断完成，已安排到B超室%1！").arg(roomName));
        
        // 刷新患者列表
        on_CheckPushButton_clicked();
        
        // 清空当前患者显示和时间
        currentPatientId.clear();
        ui->TimeLineEdit->setText("00:00:00");
    } else {
        QMessageBox::warning(this, "错误", "处理患者信息时出错！");
    }
}

void Doctor::on_NBPushButton_clicked()
{
    if (!isDiagnosing) {
        QMessageBox::information(this, "提示", "请先开始诊断！");
        return;
    }
    
    // 停止计时并获取听诊时间
    diagnosisTimer->stop();
    isDiagnosing = false;
    QString diagnosisTime = ui->TimeLineEdit->text();
    
    GlobalRegistrationManager& manager = GlobalRegistrationManager::instance();
    
    // 直接标记患者为已看诊（不开具B超）
    if (manager.completePatient(currentPatientId, diagnosisTime)) {
        QMessageBox::information(this, "诊断完成", "患者诊断完成！");
        
        // 刷新患者列表
        on_CheckPushButton_clicked();
        
        // 清空当前患者显示和时间
        currentPatientId.clear();
        ui->TimeLineEdit->setText("00:00:00");
    } else {
        QMessageBox::warning(this, "错误", "处理患者信息时出错！");
    }
}

void Doctor::updateTimer()
{
    if (isDiagnosing) {
        QTime currentTime = QTime::currentTime();
        int elapsed = startTime.msecsTo(currentTime);
        
        int hours = elapsed / 3600000;
        int minutes = (elapsed % 3600000) / 60000;
        int seconds = (elapsed % 60000) / 1000;
        
        QString timeString = QString("%1:%2:%3")
                           .arg(hours, 2, 10, QChar('0'))
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(seconds, 2, 10, QChar('0'));
        
        ui->TimeLineEdit->setText(timeString);
    }
}

