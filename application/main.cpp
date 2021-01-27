/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "mainwindow.h"

#include <tapplication.h>
#include <tsettings.h>
#include <QCommandLineParser>
#include <QJsonArray>
#include <QDir>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);

    if (QDir("/usr/share/thepage/").exists()) {
        a.setShareDir("/usr/share/thepage/");
    } else if (QDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thepage/")).exists()) {
        a.setShareDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thepage/"));
    }
    a.installTranslators();

    a.setApplicationIcon(QIcon::fromTheme("thepage", QIcon(":/icons/thepage.svg")));
    a.setApplicationVersion("1.0");
    a.setGenericName(QApplication::translate("main", "Document Viewer"));
    a.setAboutDialogSplashGraphic(a.aboutDialogSplashGraphicFromSvg(":/icons/aboutsplash.svg"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2021");
    a.setOrganizationName("theSuite");
//    a.setApplicationUrl(tApplication::HelpContents, QUrl("https://help.vicr123.com/docs/thefrisbee/intro"));
    a.setApplicationUrl(tApplication::Sources, QUrl("http://github.com/vicr123/thepage"));
    a.setApplicationUrl(tApplication::FileBug, QUrl("http://github.com/vicr123/thepage/issues"));
#ifdef T_BLUEPRINT_BUILD
    a.setApplicationName("thePage Blueprint");
    a.setDesktopFileName("com.vicr123.thepage-blueprint");
#else
    a.setApplicationName("thePage");
    a.setDesktopFileName("com.vicr123.thepage");
#endif

    a.registerCrashTrap();

    tSettings::registerDefaults(a.applicationDirPath() + "/defaults.conf");
    tSettings::registerDefaults("/etc/theSuite/thepage/defaults.conf");


    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(a.translate("main", "file"), a.translate("main", "File to open"), QStringLiteral("[%1]").arg(a.translate("main", "file")));
    parser.process(a);

    MainWindow* w = new MainWindow();

    QObject::connect(&a, &tApplication::singleInstanceMessage, [ = ](QJsonObject launchMessage) {
        if (launchMessage.contains("files")) {
            QJsonArray files = launchMessage.value("files").toArray();

            for (const QJsonValue& file : qAsConst(files)) {
                w->newTab(QUrl(file.toString()));
            }

            w->show();
            w->activateWindow();
        }
    });

    QStringList files;
    for (const QString& arg : parser.positionalArguments()) {
        if (QUrl::fromLocalFile(arg).isValid()) {
            files.append(QUrl::fromLocalFile(arg).toEncoded());
        } else {
            files.append(QUrl(arg).toEncoded());
        }
    }
    a.ensureSingleInstance({
        {"files", QJsonArray::fromStringList(files)}
    });

    for (const QString& file : qAsConst(files)) {
        w->newTab(QUrl(file));
    }

    w->show();

    return a.exec();
}
