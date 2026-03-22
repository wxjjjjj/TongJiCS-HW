#include "widget.h"
#include "./ui_widget.h"
#include "patient.h"
#include "doctor.h"
#include <QMessageBox>
#include <QRandomGenerator>

// 患者挂号函数
RegistrationResult GlobalRegistrationManager::registerPatient(const QString& name, const QString& idCard, int department, int doctor)
{
    // 检查身份证号是否已存在且未完成诊疗
    for (const auto& reg : registrations) {
        if (reg.idCard == idCard && !reg.isCompleted) {
            return REG_ALREADY_REGISTERED; // 已挂号且未完成诊疗
        }
    }
    
    // 获取下一个排队号码
    int queueNumber = getNextQueueNumber(department, doctor);
    
    // 创建新的挂号信息
    PatientR newReg(name, idCard, department, doctor, queueNumber);
    registrations.append(newReg);
    
    return REG_SUCCESS;
}

bool GlobalRegistrationManager::cancelRegistration(const QString& idCard)
{
    for (int i = 0; i < registrations.size(); ++i) {
        if (registrations[i].idCard == idCard && !registrations[i].isCompleted) {
            registrations.removeAt(i);
            return true;
        }
    }
    return false;
}

QList<PatientR> GlobalRegistrationManager::getQueueByDepartmentAndDoctor(int department, int doctor) const
{
    QList<PatientR> queue;
    for (const auto& reg : registrations) {
        if (reg.department == department && reg.doctor == doctor && !reg.isCompleted) {
            queue.append(reg);
        }
    }
    return queue;
}

PatientR* GlobalRegistrationManager::findPatientByIdCard(const QString& idCard)
{
    for (auto& reg : registrations) {
        if (reg.idCard == idCard && !reg.isCompleted) {
            return &reg;
        }
    }
    return nullptr;
}

int GlobalRegistrationManager::getQueueLength(int department, int doctor) const
{
    int count = 0;
    for (const auto& reg : registrations) {
        if (reg.department == department && reg.doctor == doctor && !reg.isCompleted) {
            count++;
        }
    }
    return count;
}

int GlobalRegistrationManager::getNextQueueNumber(int department, int doctor) const
{
    int maxQueue = 0;
    for (const auto& reg : registrations) {
        if (reg.department == department && reg.doctor == doctor) {
            maxQueue = qMax(maxQueue, reg.queueNumber);
        }
    }
    return maxQueue + 1;
}

bool GlobalRegistrationManager::completePatient(const QString& idCard)
{
    for (auto& reg : registrations) {
        if (reg.idCard == idCard && !reg.isCompleted) {
            reg.isCompleted = true;
            return true;
        }
    }
    return false;
}

PatientR GlobalRegistrationManager::getCurrentPatient(int department, int doctor) const
{
    QList<PatientR> queue = getQueueByDepartmentAndDoctor(department, doctor);
    if (!queue.isEmpty()) {
        // 找到排队号最小的患者
        PatientR current = queue[0];
        for (const auto& patient : queue) {
            if (patient.queueNumber < current.queueNumber) {
                current = patient;
            }
        }
        return current;
    }
    return PatientR(); // 返回空的患者信息
}

int GlobalRegistrationManager::assignToUltrasound(const QString& idCard)
{
    for (auto& reg : registrations) {
        if (reg.idCard == idCard && !reg.isCompleted) {
            // 标记为已完成诊疗
            reg.isCompleted = true;
            
            // 随机分配到B超室(0-2)
            int ultrasoundRoom = QRandomGenerator::global()->bounded(3);
            
            // 获取B超室的下一个排队号
            int queueNumber = getNextQueueNumber(15, ultrasoundRoom); // 15为B超室
            
            // 创建B超室挂号记录
            PatientR ultrasoundReg(reg.name, reg.idCard, 15, ultrasoundRoom, queueNumber);
            registrations.append(ultrasoundReg);
            
            return ultrasoundRoom + 1; // 返回B超室编号（1-3）
        }
    }
    return -1; // 失败返回-1
}

int GlobalRegistrationManager::assignToUltrasound(const QString& idCard, const QString& diagnosisTime)
{
    for (auto& reg : registrations) {
        if (reg.idCard == idCard && !reg.isCompleted) {
            // 标记为已完成诊疗并记录听诊时间
            reg.isCompleted = true;
            reg.diagnosisTime = diagnosisTime;
            
            // 随机分配到B超室(0-2)
            int ultrasoundRoom = QRandomGenerator::global()->bounded(3);
            
            // 获取B超室的下一个排队号
            int queueNumber = getNextQueueNumber(15, ultrasoundRoom); // 15为B超室
            
            // 创建B超室挂号记录
            PatientR ultrasoundReg(reg.name, reg.idCard, 15, ultrasoundRoom, queueNumber);
            registrations.append(ultrasoundReg);
            
            return ultrasoundRoom + 1; // 返回B超室编号（1-3）
        }
    }
    return -1; // 失败返回-1
}

bool GlobalRegistrationManager::completePatient(const QString& idCard, const QString& diagnosisTime)
{
    for (auto& reg : registrations) {
        if (reg.idCard == idCard && !reg.isCompleted) {
            reg.isCompleted = true;
            reg.diagnosisTime = diagnosisTime;
            return true;
        }
    }
    return false;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , patientWindow(nullptr)
    , doctorWindow(nullptr)
{
    ui->setupUi(this);
    //新窗口类型名初始化
    ui->TypeComboBox->addItem("患者",static_cast<int>(_Patient));
    ui->TypeComboBox->addItem("医生",static_cast<int>(_Doctor));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_YesPushButton_clicked()
{
    QString type=ui->TypeComboBox->currentText();
    
    if (type == "患者") {
        if (!patientWindow) {
            patientWindow = new Patient();
            connect(patientWindow, &Patient::returnToMain, this, &Widget::show);
        }
        this->hide();
        patientWindow->show();
    } else if (type == "医生") {
        if (!doctorWindow) {
            doctorWindow = new Doctor();
            connect(doctorWindow, &Doctor::DoctorReturnToMain, this, &Widget::show);
        }
        this->hide();
        doctorWindow->show();
    }
}

