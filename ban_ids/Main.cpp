/*
	Copyright (c) 2009-2010 Christopher A. Taylor.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of LibCat nor the names of its contributors may be used
	  to endorse or promote products derived from this software without
	  specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include "BanIdentifiers.hpp"
#include "QtInterface.hpp"
#include <QtGui/QStringListModel>
using namespace cat;

const char *SETTINGS_FILE_PATH = "BanIds.cfg";

void SetField(const char *SettingsKey, QLineEdit *control, QString &current_id, bool strong, QCheckBox *check)
{
	QByteArray qba = current_id.toAscii();
	const char *current = qba.data();
	const char *actual = Settings::ii->getStr(SettingsKey, current);

	if (!iStrEqual(actual, current))
	{
		QPalette p = control->palette();
		p.setColor( QPalette::Base, QColor(255, 255, 200) );
		control->setPalette(p);

		Settings::ii->setStr(SettingsKey, current);
	}

	control->setText(current_id);

	check->setText(strong ? "Strong" : "Weak");
	check->setCheckState(strong ? Qt::Checked : Qt::Unchecked);
}

void SetHarddriveSerials(Ui_MainWindow &ui, std::vector<HardwareSerial64> &serials)
{
	QStringList strings;
	QString ids;

	for (int ii = 0, sz = (int)serials.size(); ii < sz; ++ii)
	{
		ids.sprintf("%016llx Strong:[%c]", serials[ii].id, serials[ii].strong ? 'Y' : 'n');

		strings.append(ids);

		QByteArray qba = ids.toAscii();
		const char *current = qba.data();
		char key_name[256];
		sprintf(key_name, "Last.HardwareSerial.%d", ii);
		const char *actual = Settings::ii->getStr(key_name, current);

		if (!iStrEqual(actual, current))
		{
			QPalette p = ui.listViewSerials->palette();
			p.setColor( QPalette::Base, QColor(255, 255, 200) );
			ui.listViewSerials->setPalette(p);

			Settings::ii->setStr(key_name, current);
		}
	}

	QStringListModel *model = new QStringListModel;
	ui.listViewSerials->setModel(model);
	model->setStringList(strings);
}

int QtMain(char *arg)
{
	BanIdentifiers generator;

	int argc = 1;
	char *argv[1] = {arg};

	QApplication app(argc, argv);
	QMainWindow *mainwindow = new QMainWindow;
	Ui_MainWindow ui;
	ui.setupUi(mainwindow);

	QString ids;
	HardwareSerial64 id64;
	HardwareSerial32 id32;

	// Weak OS identifiers:

	generator.GetVSN(id32);
	ids.sprintf("%08x", id32.id);
	SetField("Last.VSN", ui.lineEditVolumeSerial, ids, id32.strong, ui.checkBoxVSN);

	generator.GetUserNameHash(id32);
	ids.sprintf("%08x", id32.id);
	SetField("Last.UserName", ui.lineEditUserName, ids, id32.strong, ui.checkBoxUserName);

	generator.GetComputerNameHash(id32);
	ids.sprintf("%08x", id32.id);
	SetField("Last.NetBIOS", ui.lineEditNETBIOS, ids, id32.strong, ui.checkBoxNetBIOS);

	// Strong OS identifiers:

	generator.GetMACAddress(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.MAC", ui.lineEditMACAddress, ids, id64.strong, ui.checkBoxMACAddress);

	generator.GetCryptographyMachineGuidHash(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.MachineGuid", ui.lineEditMachineGUID, ids, id64.strong, ui.checkBoxMachineGuid);

	generator.GetWindowsProductIDHash(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.ProductId", ui.lineEditProdId, ids, id64.strong, ui.checkBoxProductId);

	// Generated identifiers:

	generator.GetHiddenRegID1(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.RegKey1", ui.lineEditRegKey1, ids, id64.strong, ui.checkBoxReg1);

	generator.GetHiddenRegID2(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.RegKey2", ui.lineEditRegKey2, ids, id64.strong, ui.checkBoxReg2);

	generator.GetHiddenFileID1(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.FileKey1", ui.lineEditFileKey1, ids, id64.strong, ui.checkBoxFile1);

	generator.GetHiddenFileID2(id64);
	ids.sprintf("%016llx", id64.id);
	SetField("Last.FileKey2", ui.lineEditFileKey2, ids, id64.strong, ui.checkBoxFile2);

	// Hardware identifiers:

	//MessageBoxA(0, "GetCdRomSerial-ENTER", "TEST", 0);

	generator.GetCdRomSerial(id64);

	//MessageBoxA(0, "GetCdRomSerial-LEAVE", "TEST", 0);

	ids.sprintf("%016llx", id64.id);
	SetField("Last.CdRomSerial", ui.lineEditCdRom, ids, id64.strong, ui.checkBoxCdRom);

	//MessageBoxA(0, "GetHarddiskSerials", "TEST", 0);

	std::vector<HardwareSerial64> serials;
	generator.GetHarddiskSerials(serials);
	SetHarddriveSerials(ui, serials);

	//MessageBoxA(0, "GetBIOSInfo", "TEST", 0);

	HardwareSerial64 sys64, mb64, proc64;
	generator.GetBIOSInfo(sys64, mb64, proc64);
	ids.sprintf("%016llx", sys64.id);
	SetField("Last.SystemUUID", ui.lineEditSystemUUID, ids, sys64.strong, ui.checkBoxSystemUUID);
	ids.sprintf("%016llx", mb64.id);
	SetField("Last.MotherboardSerial", ui.lineEditMotherboard, ids, mb64.strong, ui.checkBoxMotherboardSerial);
	ids.sprintf("%016llx", proc64.id);
	SetField("Last.ProcessorSerial", ui.lineEditProcessor, ids, proc64.strong, ui.checkBoxProcessor);

	mainwindow->show();
	return app.exec();
}


//// Entrypoint

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!InitializeFramework(SETTINGS_FILE_PATH))
	{
		FatalStop("Unable to initialize framework!");
	}

	QtMain(lpCmdLine);

	ShutdownFramework(true);

	return 0;
}
