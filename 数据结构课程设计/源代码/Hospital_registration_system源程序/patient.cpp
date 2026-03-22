#include "patient.h"
#include "ui_patient.h"
#include "widget.h"  // 包含 GlobalRegistrationManager
#include <QMessageBox>
#include <QPainter>
#include <algorithm>

Patient::Patient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Patient)
    , patientListModel(new QStringListModel(this))
{
    ui->setupUi(this);
    
    // 设置列表视图的模型
    ui->PatientListView->setModel(patientListModel);
    
    //初始化
    ui->DepartmentComboBox->addItem("科室1",static_cast<int>(_Department1));
    ui->DepartmentComboBox->addItem("科室2",static_cast<int>(_Department2));
    ui->DepartmentComboBox->addItem("科室3",static_cast<int>(_Department3));
    ui->DepartmentComboBox->addItem("科室4",static_cast<int>(_Department4));
    ui->DepartmentComboBox->addItem("科室5",static_cast<int>(_Department5));
    ui->DepartmentComboBox->addItem("科室6",static_cast<int>(_Department6));
    ui->DepartmentComboBox->addItem("科室7",static_cast<int>(_Department7));
    ui->DepartmentComboBox->addItem("科室8",static_cast<int>(_Department8));
    ui->DepartmentComboBox->addItem("科室9",static_cast<int>(_Department9));
    ui->DepartmentComboBox->addItem("科室10",static_cast<int>(_Department10));
    ui->DepartmentComboBox->addItem("科室11",static_cast<int>(_Department11));
    ui->DepartmentComboBox->addItem("科室12",static_cast<int>(_Department12));
    ui->DepartmentComboBox->addItem("科室13",static_cast<int>(_Department13));
    ui->DepartmentComboBox->addItem("科室14",static_cast<int>(_Department14));
    ui->DepartmentComboBox->addItem("科室15",static_cast<int>(_Department15));
    ui->DepartmentComboBox->addItem("B超室",static_cast<int>(_B));//B超室排队情况

    ui->RDepartmentComboBox->addItem("科室1",static_cast<int>(_Department1));
    ui->RDepartmentComboBox->addItem("科室2",static_cast<int>(_Department2));
    ui->RDepartmentComboBox->addItem("科室3",static_cast<int>(_Department3));
    ui->RDepartmentComboBox->addItem("科室4",static_cast<int>(_Department4));
    ui->RDepartmentComboBox->addItem("科室5",static_cast<int>(_Department5));
    ui->RDepartmentComboBox->addItem("科室6",static_cast<int>(_Department6));
    ui->RDepartmentComboBox->addItem("科室7",static_cast<int>(_Department7));
    ui->RDepartmentComboBox->addItem("科室8",static_cast<int>(_Department8));
    ui->RDepartmentComboBox->addItem("科室9",static_cast<int>(_Department9));
    ui->RDepartmentComboBox->addItem("科室10",static_cast<int>(_Department10));
    ui->RDepartmentComboBox->addItem("科室11",static_cast<int>(_Department11));
    ui->RDepartmentComboBox->addItem("科室12",static_cast<int>(_Department12));
    ui->RDepartmentComboBox->addItem("科室13",static_cast<int>(_Department13));
    ui->RDepartmentComboBox->addItem("科室14",static_cast<int>(_Department14));
    ui->RDepartmentComboBox->addItem("科室15",static_cast<int>(_Department15));

    ui->DoctorComboBox->addItem("医生A/A号机",static_cast<int>(_DoctorA));
    ui->DoctorComboBox->addItem("医生B/B号机",static_cast<int>(_DoctorB));
    ui->DoctorComboBox->addItem("医生C/C号机",static_cast<int>(_DoctorC));

    ui->RDoctorComboBox->addItem("医生A/A号机",static_cast<int>(_DoctorA));
    ui->RDoctorComboBox->addItem("医生B/B号机",static_cast<int>(_DoctorB));
    ui->RDoctorComboBox->addItem("医生C/C号机",static_cast<int>(_DoctorC));

}

Patient::~Patient()
{
    delete ui;
}


void Patient::on_ReturnPushButton_clicked()
{
    this->hide();
    emit returnToMain();
}

void Patient::on_OKPushButton_clicked()
{
    // 获取输入信息
    QString name = ui->NameLineEdit->text().trimmed();//去掉字符串前后的空白字符
    QString idCard = ui->IDLineEdit->text().trimmed();//去掉字符串前后的空白字符
    int department = ui->RDepartmentComboBox->currentData().toInt();
    int doctor = ui->RDoctorComboBox->currentData().toInt();
    
    // 验证输入
    if (name.isEmpty()) {
        QMessageBox::information(this, "提示", "患者姓名不能为空！");
        ui->NameLineEdit->clear();
        ui->IDLineEdit->clear();
        return;
    }
    
    if (idCard.isEmpty()) {
        QMessageBox::information(this, "提示", "身份证号不能为空！");
        ui->NameLineEdit->clear();
        ui->IDLineEdit->clear();
        return;
    }
    
    if (idCard.length() != 8) {
        QMessageBox::warning(this, "警告", "身份证号格式不正确！应为8位数字。");
        ui->NameLineEdit->clear();
        ui->IDLineEdit->clear();
        return;
    }
    
    // 检查是否全为数字
    bool isNumber = false;
    idCard.toInt(&isNumber);
    if (!isNumber) {
        QMessageBox::warning(this, "警告", "身份证号格式不正确！应为8位数字。");
        ui->NameLineEdit->clear();
        ui->IDLineEdit->clear();
        return;
    }
    
    // 创建全局管理挂号的类
    GlobalRegistrationManager& manager = GlobalRegistrationManager::instance();//静态成员函数，确保全局只有一个GlobalRegistrationManager对象，整个程序共享这一个实例。
    
    // 检查是否已经挂号
    PatientR* existingReg = manager.findPatientByIdCard(idCard);
    if (existingReg&&existingReg->name==ui->NameLineEdit->text().trimmed()) {
        // 如果已挂号，询问是否取消
        int ret = QMessageBox::question(this, "提示", 
            QString("患者 %1 已挂号（%2科室），是否取消原来挂号？")
            .arg(existingReg->name)
            .arg(ui->RDepartmentComboBox->itemText(existingReg->department)),
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            if (manager.cancelRegistration(idCard)) {
                QMessageBox::information(this, "成功", "挂号已取消，请重新输入信息进行挂号！");
                // 清空输入框
                ui->NameLineEdit->clear();
                ui->IDLineEdit->clear();
            }
        }
        return;
    }
    else if(existingReg&&existingReg->name!=ui->NameLineEdit->text().trimmed()){
        QMessageBox::warning(this, "警告", "请检查是否输入了正确的身份证号码！");
    }
    
    // 进行挂号
    RegistrationResult result = manager.registerPatient(name, idCard, department, doctor);
    
    if (result == REG_SUCCESS) {
        int queueNumber = manager.getNextQueueNumber(department, doctor) - 1; // 当前号码
        QMessageBox::information(this, "挂号成功", 
            QString("挂号成功！\n患者：%1\n科室：%2\n医生：%3，再次点击“查看”即更新排队信息。")
            .arg(name)
            .arg(ui->RDepartmentComboBox->currentText())
            .arg(ui->RDoctorComboBox->currentText())
            );
        
        // 清空输入框
        ui->IDLineEdit->clear();
        ui->NameLineEdit->clear();
    } else {
        QMessageBox::warning(this, "挂号失败", "该身份证号已存在未完成的挂号记录！");
    }
}

void Patient::on_CheckPushButton_clicked()
{
    // 获取选定的科室和医生
    int department = ui->DepartmentComboBox->currentData().toInt();
    int doctor = ui->DoctorComboBox->currentData().toInt();
    
    GlobalRegistrationManager& manager = GlobalRegistrationManager::instance();
    
    // 获取当前排队情况（未看诊患者列表）
    QList<PatientR> waitingPatients = manager.getQueueByDepartmentAndDoctor(department, doctor);
    QStringList waitingList;
    
    if (waitingPatients.isEmpty()) {
        waitingList << "当前无患者排队";
    } else {
        // 按排队号排序
        std::sort(waitingPatients.begin(), waitingPatients.end(), 
                 [](const PatientR& a, const PatientR& b) {
                     return a.queueNumber < b.queueNumber;
                 });
        
        for (const auto& patient : waitingPatients) {
            waitingList << QString("患者：%1  身份证号：%2 ")
                          .arg(patient.name)
                          .arg(patient.idCard);
        }
    }
    
    patientListModel->setStringList(waitingList);
}
