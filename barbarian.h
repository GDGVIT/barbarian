#ifndef BARBARIAN_H
#define BARBARIAN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class barbarian; }
QT_END_NAMESPACE

class barbarian : public QMainWindow
{
    Q_OBJECT

public:
    barbarian(QWidget *parent = nullptr);
    ~barbarian();

private slots:
    void on_pushButton_Search_Packages_clicked();

    void on_actionQuit_triggered();

    void on_actionAbout_QT_triggered();

    void on_actionPreferences_triggered();

    void on_actionSearch_triggered();

    void on_actionInstalled_triggered();

    void on_pushButton_Help_Remotes_clicked();

    void on_pushButton_Conan_Dir_Help_clicked();

    void remotes_fileOpen();

    void config_fileOpen();

    void on_pushButton_Remotes_clicked();

    void on_pushButton_Config_clicked();

    void on_actionAbout_Conan_triggered();

    void on_pushButton_Package_Install_clicked();

    void on_pushButton_Package_List_Back_clicked();

    void showBuildSystem();

    void on_pushButton_Save_BuildSystem_clicked();

    void on_pushButton_Remove_clicked();

    void show_Compression();

    void show_Profile();

    void show_Storage_Path();

    void on_pushButton_Comp_Save_clicked();

    void on_pushButton_Profile_Save_clicked();

    void on_tabWidget_Preferences_currentChanged(int index);

    void on_pushButton_Storage_Save_clicked();

    void on_pushButton_Storage_Path_Help_clicked();

    void show_Installed();

    void on_actionAbout_Barbarian_triggered();

    void on_pushButton_Full_Path_Help_clicked();

    void setofFuncs();

    void show_Full_Path();

private:
    Ui::barbarian *ui;
};
#endif // BARBARIAN_H
