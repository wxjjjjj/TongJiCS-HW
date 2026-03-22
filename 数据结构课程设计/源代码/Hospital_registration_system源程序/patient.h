#ifndef PATIENT_H
#define PATIENT_H

#include <QWidget>
#include <QStringListModel>

namespace Ui {
class Patient;
}

enum DepartmentType{
    _Department1,//0
    _Department2,//1
    _Department3,
    _Department4,
    _Department5,
    _Department6,
    _Department7,
    _Department8,
    _Department9,
    _Department10,
    _Department11,
    _Department12,
    _Department13,
    _Department14,
    _Department15,
    _B//15
};

enum DoctorType{
    _DoctorA,
    _DoctorB,
    _DoctorC
};

class Patient : public QWidget
{
    Q_OBJECT
public:
    explicit Patient(QWidget *parent = nullptr);
    ~Patient();

signals:
    void returnToMain();

private slots:
    void on_ReturnPushButton_clicked();
    void on_OKPushButton_clicked();  // 添加挂号确定按钮的槽函数
    void on_CheckPushButton_clicked();  // 添加查看按钮的槽函数

private:
    Ui::Patient *ui;
    QStringListModel *patientListModel;  // 患者列表模型
};

#endif // PATIENT_H
