#include "barbarian.h"
#include "./ui_barbarian.h"
#include <QMessageBox>
#include <QStackedWidget>
#include <QFile>
#include <QTextStream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <QString>
#include <QFileInfo>

static QString Conan_Dir_Data = "/data/";
static QString Conan_file_remotes = "/remotes.json";
static QString Conan_file_config = "/conan.conf";
static QString installedList_File = "./installed.txt";
static QString buildSystem_File = "./buildsystem.txt";
static char packageList_File[] = "./pkglist.txt";
static QString Conan_Dir = QString::fromLocal8Bit(getenv("CONAN_GUI_DIR"));

barbarian::barbarian(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::barbarian)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    char * Conan_Dir_str = getenv("CONAN_GUI_DIR");
    QString Conan_Dir = QString::fromLocal8Bit(Conan_Dir_str);
    ui->lineEdit_Conan_Dir->setText(Conan_Dir);
    ui->lineEdit_Remotes->setText("all");
}

barbarian::~barbarian()
{
    delete ui;
}


void barbarian::on_pushButton_Search_Packages_clicked()
{
    QString search_Term = ui->lineEdit_Search->text();
    QString remotes_Term = ui->lineEdit_Remotes->text();
    QByteArray search_Term_qbyte = search_Term.toLocal8Bit();
    QByteArray remotes_Term_qbyte = remotes_Term.toLocal8Bit();
    char * search_Term_char = search_Term_qbyte.data();
    char * remotes_Term_char = remotes_Term_qbyte.data();
    char combined_Buffer[100];
    sprintf(combined_Buffer,"conan search %s -r=%s > %s", search_Term_char, remotes_Term_char, packageList_File);
    system(combined_Buffer);
    show_Search_Result();

}
void barbarian::on_actionQuit_triggered()
{
    qApp->quit();
}

void barbarian::on_actionAbout_QT_triggered()
{
    QMessageBox::aboutQt(this,"About QT");
}

void barbarian::on_actionPreferences_triggered()
{
    remotes_fileOpen();
    config_fileOpen();
    showBuildSystem();
    ui->stackedWidget->setCurrentIndex(2);
}

void barbarian::on_actionSearch_triggered()
{
    ui->listWidget_PackageList->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void barbarian::on_actionInstalled_triggered()
{
    ui->listWidget_Installed->clear();
    ui->stackedWidget->setCurrentIndex(1);
    QFile installed_Files_List(installedList_File);
    if(installed_Files_List.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        system("conan search > installed.txt");
        while(!installed_Files_List.atEnd()) {
            QString list_item = installed_Files_List.readLine();
            if(list_item.contains("/") || list_item.contains("Existing")) {
                    ui->listWidget_Installed->addItem(list_item);
            }
        }
        installed_Files_List.close();
    }
}


void barbarian::on_pushButton_Help_Remotes_clicked()
{
    QMessageBox::about(this,"Help","Add the remotes on which you want to search for the package.\n"
                                   "Remember, you can only search in remotes which you already have added in the remotes file.\n"
                       "You can either search in one specific remote, by mentioning it, or use 'all' to include all the remotes");
}


void barbarian::on_pushButton_Conan_Dir_Help_clicked()
{
    QMessageBox::information(this,"Help","This is where conan is currently installed. \nIf it's empty, it means you haven't added 'CONAN_GUI_DIR' environment variable.");
}

void barbarian::remotes_fileOpen() {
    QString conan_remotes_Fileopen = Conan_Dir + Conan_file_remotes;
    QFile remotes_file(conan_remotes_Fileopen);

    if(!remotes_file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0,"info",remotes_file.errorString());
    }

    QTextStream remotes_file_in(&remotes_file);
    ui->textEdit_Remotes->setText(remotes_file_in.readAll());
    remotes_file.close();

}

void barbarian::config_fileOpen() {
    QString conan_config_Fileopen = Conan_Dir + Conan_file_config;
    QFile config_file(conan_config_Fileopen);

    if(!config_file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0,"info",config_file.errorString());
    }

    QTextStream config_file_in(&config_file);
    ui->textEdit_Config->setText(config_file_in.readAll());
    config_file.close();

}

void barbarian::show_Search_Result() {
    ui->stackedWidget->setCurrentIndex(3);
    ui->listWidget_PackageList->clear();
    QFile search_Result_File(packageList_File);

    if(search_Result_File.open(QIODevice::ReadOnly | QIODevice::ReadWrite )) {
        while(!search_Result_File.atEnd()) {
            QString result_Item = search_Result_File.readLine();
            if (result_Item.contains("/") || result_Item.contains("Remote2")) {
                ui->listWidget_PackageList->addItem(result_Item);
            }
        }
    }
    search_Result_File.close();
}

void barbarian::on_pushButton_Remotes_clicked()
{

    QString remotes_File_Final = Conan_Dir + Conan_file_remotes;
    QFile conan_Remotes_File_Write(remotes_File_Final);
    conan_Remotes_File_Write.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QString text_now_remotes = ui->textEdit_Remotes->toPlainText();
    QTextStream remotes_file_update(&conan_Remotes_File_Write);
    remotes_file_update << text_now_remotes;
    conan_Remotes_File_Write.close();
}

void barbarian::on_pushButton_Config_clicked()
{
    QString config_File_Final = Conan_Dir + Conan_file_config;
    QFile conan_Config_File_Write(config_File_Final);
    conan_Config_File_Write.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QString text_now_config = ui->textEdit_Config->toPlainText();
    QTextStream config_file_update(&conan_Config_File_Write);
    config_file_update << text_now_config;
    conan_Config_File_Write.close();
}

void barbarian::on_actionAbout_Conan_triggered()
{
    QMessageBox::about(this, "About Conan", "Conan is a portable package manager, intended for C and C++ developers,"
                       " but it is able to manage builds from source, dependencies,\nand precompiled binaries for any language.\n\n"
                       "For more information, check conan.io.");
}

void barbarian::on_pushButton_Package_Install_clicked()
{
    QString Conan_File_TXT = "/conanfile.txt";
    int current_Index_PackageList = ui->listWidget_PackageList->currentRow();
    QString current_Install_Package = ui->listWidget_PackageList->item(current_Index_PackageList)->text();
    QString Proj_Dir = ui->lineEdit_Project_Directory->text();
    QString Proj_generator = ui->lineEdit_BuildSystem->text();
    if(Proj_Dir.length() == 0) {
        QMessageBox::critical(this,"Error","Please specifiy the project directory.");
    } else {
        Conan_File_TXT = Proj_Dir + Conan_File_TXT;
        QFile Proj_Dir_File(Conan_File_TXT);
        if(Proj_Dir_File.open(QIODevice::WriteOnly)) {
            QTextStream Proj_Dir_File_Write(&Proj_Dir_File);
            Proj_Dir_File_Write << "[requires]" << "\n";
            Proj_Dir_File_Write << current_Install_Package << "\n\n";
            Proj_Dir_File_Write << "[generators]" << "\n";
            Proj_Dir_File_Write << Proj_generator;
            Proj_Dir_File.close();
        }
        QByteArray Proj_Dir_QBA = Proj_Dir.toLocal8Bit();
        char * Proj_Dir_char = Proj_Dir_QBA.data();
        char Conan_Install_Command[100];
        sprintf(Conan_Install_Command,"conan install %s", Proj_Dir_char);
        system(Conan_Install_Command);
    }

}

void barbarian::on_pushButton_Package_List_Back_clicked()
{
    ui->listWidget_PackageList->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void barbarian::showBuildSystem() {

    QFile buildFile(buildSystem_File);

    if(buildFile.open(QIODevice::ReadWrite)) {
        QTextStream buildFile_In(&buildFile);
        QString generators = buildFile_In.readAll();
        if((QString::compare(generators,"")) == 0) {
            ui->lineEdit_BuildSystem->setText("cmake");
            buildFile_In << "cmake";
        } else {
            ui->lineEdit_BuildSystem->setText(generators);
        }
        buildFile.close();
    } else {
        QMessageBox::critical(this,"Error","Couldn't get enough permissions for the file.");
    }
}


void barbarian::on_pushButton_Save_BuildSystem_clicked()
{
    QFile buildFile_Write(buildSystem_File);
    if(buildFile_Write.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QTextStream buildFile_Out(&buildFile_Write);
        QString generators_out = ui->lineEdit_BuildSystem->text();
        buildFile_Out << generators_out;
        buildFile_Write.close();
    }
}

void barbarian::on_pushButton_Remove_clicked()
{
    int current_Index_Remove = ui->listWidget_Installed->currentRow();
    QString current_Remove_Package = ui->listWidget_Installed->item(current_Index_Remove)->text();
    int Remove_Package_Index = current_Remove_Package.indexOf("/");
    current_Remove_Package.truncate(Remove_Package_Index);
    QByteArray current_Remove_Package_QBA = current_Remove_Package.toLocal8Bit();
    char * curr_Rem_Package_char = current_Remove_Package_QBA.data();
    char Full_Path_Rem[100];
    QString Conan_FP = Conan_Dir + Conan_Dir_Data;
    QByteArray Conan_FP_QBA = Conan_FP.toLocal8Bit();
    char * Conan_Dir_char = Conan_FP_QBA.data();
    sprintf(Full_Path_Rem,"rm -rf %s/%s",Conan_Dir_char,curr_Rem_Package_char);
    system(Full_Path_Rem);
}
