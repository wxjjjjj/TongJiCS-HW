#ifndef DOCTOR_H
#define DOCTOR_H

#include <QWidget>
#include <QStringListModel>
#include <QTimer>
#include <QTime>

namespace Ui {
class Doctor;
}

enum DepartmentType2{
    _Department21,
    _Department22,
    _Department23,
    _Department24,
    _Department25,
    _Department26,
    _Department27,
    _Department28,
    _Department29,
    _Department210,
    _Department211,
    _Department212,
    _Department213,
    _Department214,
    _Department215
};

enum DoctorType2{
    _Doctor2A,
    _Doctor2B,
    _Doctor2C
};

class Doctor : public QWidget
{
    Q_OBJECT

public:
    explicit Doctor(QWidget *parent = nullptr);
    ~Doctor();

signals:
    void DoctorReturnToMain();

private slots:
    void on_PushButton_clicked();
    void on_CheckPushButton_clicked();  // 添加查看按钮的槽函数
    void on_StartPushButton_clicked();  // 开始诊断
    void on_BPushButton_clicked();      // 结束诊断(开具B超检查)
    void on_NBPushButton_clicked();     // 结束诊断(不开具B超检查)
    void updateTimer();                 // 更新计时器显示

private:
    Ui::Doctor *ui;
    QStringListModel *nextPatientModel;     // 未看诊患者模型
    QStringListModel *previousPatientModel; // 已看诊患者模型
    QTimer *diagnosisTimer;                 // 诊断计时器
    QTime startTime;                        // 开始时间
    QString currentPatientId;               // 当前正在诊断的患者身份证号
    bool isDiagnosing;                      // 是否正在诊断
};

#endif // DOCTOR_H
