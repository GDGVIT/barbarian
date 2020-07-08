
/*
 *
 * barbarian.cpp - this file is part of Barbarian, A frontend for the conan package manager written in Qt. 
 *
 * Copyright 2020 Govind K 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
*/


#include <iostream>
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
#include <QtWidgets>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

static QString Conan_file_remotes = "/.conan/remotes.json";
static QString Conan_file_config = "/.conan/conan.conf";
static QString buildSystem_File = "./buildsystem.txt";
static char * Conan_Dir_str = getenv("CONAN_USER_HOME");
static QString Conan_Dir = QString::fromLocal8Bit(Conan_Dir_str);
QProcess Install_Package;
QProcess Package_Find;

barbarian::barbarian(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::barbarian)
{
    if(system("conan > /dev/null") != 0) {
        QMessageBox::critical(this,"Conan not found!","It seems conan is not installed in your system. Please install and try opening again.");

    } else {
        ui->setupUi(this);
        ui->stackedWidget->setCurrentIndex(0);
        ui->lineEdit_Remotes->setText("all");
        ui->progressBar_Progress->hide();
        if(Conan_Dir.lastIndexOf('/') == (Conan_Dir.length()-1)) {
            Conan_Dir = Conan_Dir.remove(Conan_Dir.length()-1,1);
        }
        remotes_fileOpen();
        config_fileOpen();
        showBuildSystem();
        show_Storage_Path();
        ui->lineEdit_Conan_Dir->setText(Conan_Dir);
    }
}

barbarian::~barbarian()
{
    delete ui;
}

void barbarian::on_pushButton_Search_Packages_clicked() {

    QProcess netCheck;
    QStringList netArgs = {"-c", "1", "www.example.com"};
    netCheck.start("ping", netArgs, QIODevice::ReadOnly);
    netCheck.waitForFinished(-1);
    if(netCheck.exitCode() != 0) {
        QMessageBox::critical(this,"No Connection!","You're not connected to internet. Please connect and try again!");
    } else {
        ui->progressBar_Progress->setValue(0);
        ui->progressBar_Progress->show();
        QString search_Term = ui->lineEdit_Search->text();
        QString remotes_Term = ui->lineEdit_Remotes->text();
        if(search_Term.length() == 0 || remotes_Term.length() == 0) {
            QMessageBox::critical(this,"Error","The search field and/or remote field is empty.");
        } else {
			QString remotes_Term_Comb = "-r=" + remotes_Term;
            ui->progressBar_Progress->setValue(40);
            Package_Find.setProcessChannelMode(QProcess::MergedChannels);
        	Package_Find.start("conan", QStringList() << "search" << search_Term << remotes_Term_Comb, QIODevice::ReadWrite);
            ui->stackedWidget->setCurrentIndex(3);
            Package_Find.waitForFinished(-1);
            while(Package_Find.canReadLine()) {
                QByteArray line_QBA = Package_Find.readLine();
                QString line = QString::fromStdString(line_QBA.toStdString());
                ui->listWidget_PackageList->addItem(line);
            }
        }
    }
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
    ui->stackedWidget->setCurrentIndex(2);
	setofFuncs();
}

void barbarian::on_actionSearch_triggered()
{
    ui->listWidget_PackageList->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void barbarian::show_Installed() {
    ui->listWidget_Installed->clear();
    ui->stackedWidget->setCurrentIndex(1);
    QStringList searchyList = {"search"};
    QProcess Searchy;
    Searchy.start("conan", searchyList, QIODevice::ReadOnly);
    Searchy.waitForFinished(-1);
    while(Searchy.canReadLine()) {
        QByteArray searchy_QBA = Searchy.readLine();
        QString searchy_line = QString::fromStdString(searchy_QBA.toStdString());
        ui->listWidget_Installed->addItem(searchy_line);
    }
    Searchy.kill();
}

void barbarian::on_actionInstalled_triggered()
{
    show_Installed();
}


void barbarian::on_pushButton_Help_Remotes_clicked()
{
    QMessageBox::about(this,"Help","Add the remotes on which you want to search for the package.\n"
                                   "Remember, you can only search in remotes which you already have added in the remotes file.\n"
                       "You can either search in one specific remote, by mentioning it, or use 'all' to include all the remotes");
}


void barbarian::on_pushButton_Conan_Dir_Help_clicked()
{
    QMessageBox::information(this,"Help","This is where conan is currently installed. \nIf it's empty, it means you haven't added 'CONAN_USER_HOME' environment variable.");
}

void barbarian::remotes_fileOpen() {
    QString conan_remotes_Fileopen = Conan_Dir + Conan_file_remotes;
    QFile remotes_file(conan_remotes_Fileopen);

    if(!remotes_file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0,"Failure","Remotes file not found. Make sure the file is present and the environment variable 'CONAN_USER_HOME' set.");
    } else {
		QTextStream remotes_file_in(&remotes_file);
	    ui->textEdit_Remotes->setText(remotes_file_in.readAll());
		 remotes_file.close();
	}
}

void barbarian::config_fileOpen() {
    QString conan_config_Fileopen = Conan_Dir + Conan_file_config;
    QFile config_file(conan_config_Fileopen);

    if(!config_file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0,"Failure","Config file not found. Make sure the file is present and the environment variable 'CONAN_USER_HOME' set.");
    } else {
	    QTextStream config_file_in(&config_file);
    	ui->textEdit_Config->setText(config_file_in.readAll());
    	config_file.close();
	}
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

void barbarian::on_pushButton_Package_Install_clicked() {
    ui->progressBar_Installation->setValue(0);
    QString Conan_File_TXT = "/conanfile.txt";
    int current_Index_PackageList=-1;
    if(!(ui->listWidget_PackageList->currentRow() >= 0)) {
        QMessageBox::information(this,"Error","Please select a package.");
        return;
    }
    current_Index_PackageList = ui->listWidget_PackageList->currentRow();
    QString current_Install_Package = ui->listWidget_PackageList->item(current_Index_PackageList)->text();
    if(!current_Install_Package.contains("/") || current_Install_Package.isNull()) {
        QMessageBox::information(this,"Error","Select a package.");
    } else {
        QString Proj_Dir = ui->lineEdit_Project_Directory->text();
        QString Proj_generator = ui->lineEdit_BuildSystem->text();
        if(Proj_Dir.length() == 0) {
            QMessageBox::critical(this,"Error","Please specifiy the project directory.");
        } else {
            QFileInfo Proj_Dir_Check(Proj_Dir);
            if(!(Proj_Dir_Check.isDir())) {
                QMessageBox::critical(this,"Invalid Directory","Please specify a valid directory.");
                return;
            }
            if(current_Install_Package.length() == 0 ) {
                QMessageBox::critical(this, "Error" ,"Please choose a package");
            } else {
                Conan_File_TXT = Proj_Dir + Conan_File_TXT;
                QFile Proj_Dir_File(Conan_File_TXT);
                if(Proj_Dir_File.open(QIODevice::ReadWrite)) {
                    QTextStream Proj_Dir_File_Write(&Proj_Dir_File);
                    Proj_Dir_File_Write << "[requires]" << "\n";
                    Proj_Dir_File_Write << current_Install_Package << "\n\n";
                    Proj_Dir_File_Write << "[generators]" << "\n";
                    Proj_Dir_File_Write << Proj_generator;
                    Proj_Dir_File.close();
                }
                remotes_fileOpen();
                config_fileOpen();
                showBuildSystem();
                ui->lineEdit_Conan_Dir->setText(Conan_Dir);
                QString Conan_Install_Command = "conan";
                ui->progressBar_Installation->setValue(70);
                Install_Package.setProcessChannelMode(QProcess::MergedChannels);
                QStringList installList = {"install", Proj_Dir};
                Install_Package.start(Conan_Install_Command, installList, QIODevice::ReadWrite);
                if(Install_Package.waitForFinished(-1)) {
                    while(Install_Package.canReadLine()) {
                        QByteArray IP_line_QBA = Install_Package.readLine();
                        QString IP_line = QString::fromStdString(IP_line_QBA.toStdString());
                        ui->textEdit_Installation->append(IP_line);
                    }
                    ui->progressBar_Installation->setValue(100);
                }
				if(Install_Package.exitCode() == 0) {
					QMessageBox::information(this,"Success","The package was successfully installed!");
				} else {
					QMessageBox::information(this,"Failed!","The package was not installed! Please check the output for further information.");
				}
                ui->progressBar_Installation->setValue(0);
                QString Conan_mv = "mv";
				QProcess file_Transfer;
                QStringList transList = { "./conanbuildinfo.cmake", "./conanbuildinfo.txt", "./conaninfo.txt", "./conan.lock", "./graph_info.json", Proj_Dir};
                file_Transfer.start(Conan_mv, transList, QIODevice::ReadWrite);
                if(file_Transfer.waitForFinished(-1)) {
					ui->textEdit_Installation->append("Added project files.");
                }
				if(file_Transfer.exitCode() != 0) {
					QMessageBox::information(this,"File transfer failed.","The conan project files were not added!");
				}
			}
        }
    }
}

void barbarian::on_pushButton_Package_List_Back_clicked()
{
    ui->listWidget_PackageList->clear();
    ui->stackedWidget->setCurrentIndex(0);
    ui->progressBar_Progress->hide();
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
	setofFuncs();
}

void barbarian::on_pushButton_Remove_clicked() {
    QString ver;
    QString current_Remove_Package_name;
    int current_Index_Remove;
    QString Conan_Dir_Data = ui->lineEdit_Storage_Path->text();
    if(!(ui->listWidget_Installed->currentRow() >= 0)) {
        QMessageBox::information(this,"Error","Please select a package.");
        return;
    }
    current_Index_Remove = ui->listWidget_Installed->currentRow();
    QString current_Remove_Package = ui->listWidget_Installed->item(current_Index_Remove)->text();
    if(current_Remove_Package.length() != 0 && (current_Remove_Package.contains("/") || current_Remove_Package.contains("@"))) {
        int Remove_Package_Index = current_Remove_Package.indexOf("/");
        current_Remove_Package_name = current_Remove_Package;
        current_Remove_Package_name.truncate(Remove_Package_Index);
        int current_Remove_Index_version = current_Remove_Package.indexOf("@");
        if(current_Remove_Index_version > -1) {
            ver = current_Remove_Package.mid((Remove_Package_Index+1), (current_Remove_Index_version-Remove_Package_Index-1));
        } else {
            ver = current_Remove_Package.mid(Remove_Package_Index+1);
        }
        /*    if(Conan_Dir_Data.startsWith('.')) {
            Conan_Dir_Data.remove(0,1);
         } */
        QString Conan_FP = Conan_Dir + + "/.conan/" + Conan_Dir_Data;
        QString beg_Rem = "rm";
        QString full_Path_Rem = Conan_FP+ "/"+ current_Remove_Package_name+ "/"+ ver;
        QMessageBox::information(this, "Successful!", full_Path_Rem);
        QStringList rem_Pack_List = {"-rf", full_Path_Rem};
        QProcess rem_Pack_Process;
        rem_Pack_Process.start(beg_Rem, rem_Pack_List, QIODevice::ReadWrite);
		rem_Pack_Process.waitForFinished(-1);
        if(rem_Pack_Process.exitCode() == 0) {
            QMessageBox::information(this, "Successful!", "The package has been successfully removed");
        } else {
            QMessageBox::information(this, "Failed!", "Failed to remove the package.");
        }
        show_Installed();
    } else {
        QMessageBox::information(this, "Error!", "Please select a package.");
    }
}

void barbarian::show_Compression() {
    QFile compress_Read_file(Conan_Dir + Conan_file_config);
    QTextStream compress_file_read_Stream(&compress_Read_file);
    QStringList compress_qsl_read;
    if(compress_Read_file.open(QIODevice::ReadWrite)) {
        while(!compress_file_read_Stream.atEnd()) {
            compress_qsl_read.append(compress_file_read_Stream.readLine());
        }
        int indexOfRead = compress_qsl_read.indexOf(QRegExp("compression_level*",Qt::CaseInsensitive,QRegExp::Wildcard));
        QString compress_Part = compress_qsl_read.at(indexOfRead);
        int indexOfEqual = compress_Part.lastIndexOf("=");
        compress_Part = compress_Part.mid((indexOfEqual+2),3);
        ui->lineEdit_Comp_Lev->setText(compress_Part);
        compress_Read_file.close();
    }
}

void barbarian::show_Profile() {
    QFile profile_Read_file(Conan_Dir + Conan_file_config);
    QTextStream profile_file_read_Stream(&profile_Read_file);
    QStringList profile_qsl_read;
    if(profile_Read_file.open(QIODevice::ReadWrite)) {
        while(!profile_file_read_Stream.atEnd()) {
            profile_qsl_read.append(profile_file_read_Stream.readLine());
        }
        int indexOfProfRead = profile_qsl_read.indexOf(QRegExp("default_profile*",Qt::CaseInsensitive,QRegExp::Wildcard));
        QString profile_Part = profile_qsl_read.at(indexOfProfRead);
        profile_Part = profile_Part.mid((profile_Part.lastIndexOf(" ")+1));
        ui->lineEdit_Profile->setText(profile_Part);
        profile_Read_file.close();
    }
}

void barbarian::show_Storage_Path() {
    int j=0;
    QString comb_Path = Conan_Dir + Conan_file_config;
    QFile path_Read_f(comb_Path);
    QTextStream path_f_read_Stream(&path_Read_f);
    QStringList path_qsl_read;
    if(path_Read_f.open(QIODevice::Append)) {
        if(path_Read_f.pos() == 0) {
            QMessageBox::information(this,"Failed!", "Failed to show Storage Path.");
            path_Read_f.close();
            return;
        } else {
            path_Read_f.close();
        }
    } else {
        QMessageBox::information(this,"Failed!", "Failed to show Storage Path.");
        return;
    }
    QFile path_Read_file(comb_Path);
    QTextStream path_file_read_Stream(&path_Read_file);
    if(path_Read_file.open(QIODevice::ReadWrite)) {
        while(!path_file_read_Stream.atEnd()) {
            path_qsl_read.append(path_file_read_Stream.readLine());
            j++;
        }
        int indexOfPathRead = path_qsl_read.indexOf(QRegExp("path = *",Qt::CaseInsensitive,QRegExp::Wildcard));
        QString path_Part = path_qsl_read.at(indexOfPathRead);
        path_Part = path_Part.mid((path_Part.lastIndexOf(" ")+1));
        ui->lineEdit_Storage_Path->setText(path_Part);
        path_Read_file.close();
    } else {
        QMessageBox::information(this,"Failed!", "Failed to show Storage Path.");
        return;
    }
}
void barbarian::on_pushButton_Comp_Save_clicked()
{
    int i=0; bool isItOK;
    QString Comp_Lev = ui->lineEdit_Comp_Lev->text().trimmed();
    if(Comp_Lev.toInt(&isItOK,10) != 0) {
        QFile compress_Change_file(Conan_Dir + Conan_file_config);
        QTextStream compress_file_Stream(&compress_Change_file);
        QStringList compress_qsl_opt;
        if(compress_Change_file.open(QIODevice::ReadWrite)) {
            while(!compress_file_Stream.atEnd()) {
                compress_qsl_opt.append(compress_file_Stream.readLine());
            }
            compress_Change_file.close();
            int index = compress_qsl_opt.indexOf(QRegExp("compression_level*",Qt::CaseInsensitive,QRegExp::Wildcard));
            QString compress_New = "compression_level = " + Comp_Lev;
            compress_qsl_opt.replace(index,compress_New);
            int ind2 = compress_qsl_opt.length();
            QFile compress_Change_file2(Conan_Dir + Conan_file_config);
            QTextStream compress_file_Stream2(&compress_Change_file2);
            if(compress_Change_file2.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
                while(i<ind2) {
                    compress_file_Stream2 << compress_qsl_opt.at(i) << "\n";
                    i++;
                }
                i=0;
            }
            compress_Change_file.close();
            QMessageBox::information(this,"Success!","Compression level has been updated successfully!");
			setofFuncs();
        }
    } else {
       QMessageBox::information(this,"Error","Input a numerical value.");
    }
}

void barbarian::on_pushButton_Profile_Save_clicked()
{
    int g=0;
    QFile profile_Change_file(Conan_Dir + Conan_file_config);
    QTextStream profile_file_Stream(&profile_Change_file);
    QStringList profile_qsl_opt;
    if(profile_Change_file.open(QIODevice::ReadWrite)) {
        while(!profile_file_Stream.atEnd()) {
            profile_qsl_opt.append(profile_file_Stream.readLine());
        }
        profile_Change_file.close();
        int index = profile_qsl_opt.indexOf(QRegExp("default_profile*",Qt::CaseInsensitive,QRegExp::Wildcard));
        QString profile_New = "default_profile = " + (ui->lineEdit_Profile->text()).trimmed();
        profile_qsl_opt.replace(index,profile_New);
        int ind3 = profile_qsl_opt.length();
        QFile profile_Change_file2(Conan_Dir + Conan_file_config);
        QTextStream profile_file_Stream2(&profile_Change_file2);
        if(profile_Change_file2.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            while(g<ind3) {
                profile_file_Stream2 << profile_qsl_opt.at(g) << "\n";
                g++;
            }
            g=0;
        }
        profile_Change_file.close();
        QMessageBox::information(this,"Success!","Default profile has been updated successfully!");
		setofFuncs();
    }
}

void barbarian::on_tabWidget_Preferences_currentChanged(int index)
{
    if(index == 0) {
        show_Compression();
        show_Profile();
        show_Storage_Path();
        showBuildSystem();
        show_Full_Path();
    }

    else if(index == 1) {
        remotes_fileOpen();
        config_fileOpen();
    }
}

void barbarian::on_pushButton_Storage_Save_clicked()
{
    int j=0;
    QFile path_Change_file(Conan_Dir + Conan_file_config);
    QTextStream path_file_Stream(&path_Change_file);
    QStringList path_qsl_opt;
    if(path_Change_file.open(QIODevice::ReadWrite)) {
        while(!path_file_Stream.atEnd()) {
            path_qsl_opt.append(path_file_Stream.readLine());
        }
        path_Change_file.close();
        int index = path_qsl_opt.indexOf(QRegExp("path =*",Qt::CaseInsensitive,QRegExp::Wildcard));
        QString path_New = "path = " + (ui->lineEdit_Storage_Path->text()).trimmed();
        path_qsl_opt.replace(index,path_New);
        int ind4 = path_qsl_opt.length();
        QFile path_Change_file2(Conan_Dir + Conan_file_config);
        QTextStream path_file_Stream2(&path_Change_file2);
        if(path_Change_file2.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            while(j<ind4) {
                path_file_Stream2 << path_qsl_opt.at(j) << "\n";
                j++;
            }
            j=0;
        }
        path_Change_file.close();
        QMessageBox::information(this,"Success!","The default storage path has been updated successfully!");
		setofFuncs();
    }
}

void barbarian::on_pushButton_Storage_Path_Help_clicked()
{
    QMessageBox::information(this,"Help","This is the directory where the packeges would be installed. \n"
                                         "Note: this directory is with respective to the conan user home.\n"
                                  "i.e. Give only the absolute path.");
}

void barbarian::on_actionAbout_Barbarian_triggered()
{
    QMessageBox about_Barbie;
    QPixmap barbie_Pixie;
    barbie_Pixie.load("../res/barbarian.jpeg");
    barbie_Pixie.scaled(3,3);
    about_Barbie.setIconPixmap(barbie_Pixie);
    about_Barbie.exec();
    about_Barbie.about(this,"About Barbarian","Barbarian is an open-source project started and maintained by Developer Students Club, Vit, Vellore, India.\n"
                                         "Barbarian is under a GPL 3.0 license, and the file 'COPYING' includes details of the license.\n");
}

void barbarian::on_pushButton_Full_Path_Help_clicked()
{
    QMessageBox::information(this, "Help", "This is the full path. You can't modify this. This is only to show you the full path.");
}

void barbarian::setofFuncs() {
    show_Compression();
    show_Profile();
    show_Storage_Path();
    show_Full_Path();
}

void barbarian::show_Full_Path() {
    QString full_yo = ui->lineEdit_Storage_Path->text();
    QString full_full = Conan_Dir + full_yo;
    ui->lineEdit_Full_Path->setText(full_full);
}
