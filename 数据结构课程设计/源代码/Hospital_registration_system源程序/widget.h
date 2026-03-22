#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QList>
#include "patient.h"
#include "doctor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

enum Type{
    _Patient,
    _Doctor
};

// 挂号结果枚举
enum RegistrationResult {
    REG_SUCCESS = 0,           // 挂号成功
    REG_ALREADY_REGISTERED     // 已经挂号
};

struct PatientR {
    QString name;           // 患者姓名
    QString idCard;         // 身份证号
    int department;         // 科室 (0-14为普通科室，15为B超室)
    int doctor;             // 医生 (0-2)
    int queueNumber;        // 排队号码
    bool isCompleted;       // 是否已完成诊疗
    QString diagnosisTime;  // 听诊时间

    PatientR() : department(0), doctor(0), queueNumber(0), isCompleted(false) {}
    PatientR(const QString& n, const QString& id, int dept, int doc, int queue)
        : name(n), idCard(id), department(dept), doctor(doc), queueNumber(queue), isCompleted(false) {}
};

// 全局挂号管理类
class GlobalRegistrationManager {
public:
    static GlobalRegistrationManager& instance() {
        static GlobalRegistrationManager instance;
        return instance;
    }

    // 挂号操作
    RegistrationResult registerPatient(const QString& name, const QString& idCard, int department, int doctor);//8.22
    bool cancelRegistration(const QString& idCard);//8.22
    PatientR* findPatientByIdCard(const QString& idCard);//8.22 根据身份证号查询患者信息
    // 查询操作
    QList<PatientR> getQueueByDepartmentAndDoctor(int department, int doctor) const;//8.22获取某个科室、某个医生名下的排队患者列表
    int getQueueLength(int department, int doctor) const;//8.22
    int getNextQueueNumber(int department, int doctor) const;//8.22


    // 医生操作
    bool completePatient(const QString& idCard);
    bool completePatient(const QString& idCard, const QString& diagnosisTime);
    PatientR getCurrentPatient(int department, int doctor) const;
    int assignToUltrasound(const QString& idCard); // 将患者分配到B超室，返回B超室编号（1-3），失败返回-1
    int assignToUltrasound(const QString& idCard, const QString& diagnosisTime);

    // 获取所有挂号信息
    const QList<PatientR>& getAllRegistrations() const { return registrations; }

private:
    GlobalRegistrationManager() = default;
    QList<PatientR> registrations;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_YesPushButton_clicked();

private:
    Ui::Widget *ui;
    Patient *patientWindow;
    Doctor *doctorWindow;
};
#endif // WIDGET_H
