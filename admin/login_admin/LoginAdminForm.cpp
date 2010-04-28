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

#include "LoginAdminForm.moc"
#include <QtGui/QFileDialog>
#include <Qt/QSettings.h>
#include <QtGui/QMessageBox>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cat;

LoginAdminForm::LoginAdminForm()
	: _qout(std::cout)
{
	_client = 0;

	setupUi(this);

	_qout.setLogWindow(textEditStatus);

	QSettings settings("Splane", "LoginAdminTool");
	if (settings.value("valid_geometry").toBool())
	{
		move(settings.value("pos").toPoint());
		resize(settings.value("size").toSize());
	}
	lineEditIP->setText(settings.value("LoginServerIP", "splane.game-server.cc").toString());
	lineEditPort->setText(settings.value("LoginServerPort", "22000").toString());
	lineEditPublicKey->setText(settings.value("LoginServerKey", "z3HNuK3BBBxfbqu3klZ9E2snQ8ob7g+25kMuf7zO1mIaB8aD8g6Fscz+V8zssB9BiTp2poRWvyNcOWnrX3dNIg==").toString());
	lineEditUserName->setText(settings.value("UserName", "").toString());
	checkBoxUseKeyFile->setChecked(settings.value("UseKeyFile", true).toBool());
	lineEditKeyFilePath->setText(settings.value("KeyFilePath", "user_key_file.private").toString());
	lineEditAlias->setText(settings.value("Alias", "").toString());
	textEditStatus->setEnabled(false);

	SetConnectStatus(false);

	lineEditPassword->setFocus();

	_player_list_model = new QStringListModel();
	listViewChatUsers->setModel(_player_list_model);
}


void LoginAdminForm::DisconnectClient()
{
	SetConnectStatus(false);

	if (_client) _client->Disconnect();
	ThreadRefObject::SafeRelease(_client);
}

bool LoginAdminForm::IsValidLoginInfo()
{
	if (lineEditPublicKey->text().isEmpty())
	{
		QMessageBox::warning(this, "Please enter a Public Key path", "Please enter a Public Key path");
		return false;
	}
	if (checkBoxUseKeyFile->isChecked() && lineEditKeyFilePath->text().isEmpty())
	{
		QMessageBox::warning(this, "Please enter a Key File path", "Please enter a Key File path");
		return false;
	}
	if (lineEditPassword->text().isEmpty())
	{
		QMessageBox::warning(this, "Please enter a password", "Please enter a password");
		return false;
	}

	return true;
}

void LoginAdminForm::SetConnectStatus(bool connecting)
{
	pushButtonConnect->setEnabled(!connecting);
	pushButtonDisconnect->setEnabled(connecting);
	groupBoxAddress->setEnabled(!connecting);
	groupBoxPubKey->setEnabled(!connecting);
	groupBoxPassword->setEnabled(!connecting);
	groupBoxKeyFile->setEnabled(!connecting);

	tabWidget->removeTab(tabWidget->indexOf(tabLog));
	tabWidget->removeTab(tabWidget->indexOf(tabChat));
	tabWidget->removeTab(tabWidget->indexOf(tabActive));
	tabWidget->removeTab(tabWidget->indexOf(tabUsers));
	tabWidget->removeTab(tabWidget->indexOf(tabBans));
	tabWidget->removeTab(tabWidget->indexOf(tabZones));

	if (connecting)
	{
		statusbar->showMessage("Connecting...");
		textEditStatus->setEnabled(true);
	}
	else
	{
		statusbar->showMessage("Not connected");
	}
}

void LoginAdminForm::SetConnected()
{
	tabWidget->insertTab(1, tabChat, "Chat");
	tabWidget->insertTab(2, tabActive, "Active Users");
	tabWidget->insertTab(3, tabUsers, "User Accounts");
	tabWidget->insertTab(4, tabBans, "Bans");
	tabWidget->insertTab(5, tabZones, "Zone Servers");
	tabWidget->insertTab(6, tabLog, "Monitor Log");

	statusbar->showMessage("Connected.");
}

bool LoginAdminForm::GenerateNewKeyPair(ThreadPoolLocalStorage *tls, u8 *public_key, u8 *private_key)
{
	KeyMaker bob;
	if (!bob.GenerateKeyPair(tls->math, tls->csprng, public_key, sizeof(public_key), private_key, sizeof(private_key)))
	{
		FATAL("DatabaseClient") << "Unable to generate a key pair for some reason!";
		return false;
	}

	char public_key_base64[256];
	char private_key_base64[256];

	int public_bytes = WriteBase64(public_key, sizeof(public_key), public_key_base64, sizeof(public_key_base64));
	int private_bytes = WriteBase64(private_key, sizeof(private_key), private_key_base64, sizeof(private_key_base64));

	if (!public_bytes || !private_bytes)
	{
		FATAL("DatabaseClient") << "Unable to encode keys in Base64";
		return false;
	}

	Settings::ref()->setStr("Database.Key.Public", public_key_base64);
	Settings::ref()->setStr("Database.Key.Private", private_key_base64);

	// Flush new keys to disk right away so that the Login Database
	// server can be updated without restarting the Login server
	Settings::ref()->write();

	return true;
}


//// UI Events

void LoginAdminForm::closeEvent(QCloseEvent *e)
{
	QSettings settings("Splane", "LoginAdminTool");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("valid_geometry", !isMaximized() && !isFullScreen() && !isMinimized());
	settings.setValue("LoginServerIP", lineEditIP->text());
	settings.setValue("LoginServerPort", lineEditPort->text());
	settings.setValue("LoginServerKey", lineEditPublicKey->text());
	settings.setValue("UserName", lineEditUserName->text());
	settings.setValue("UseKeyFile", checkBoxUseKeyFile->isChecked());
	settings.setValue("KeyFilePath", lineEditKeyFilePath->text());
	settings.setValue("Alias", lineEditAlias->text());

	DisconnectClient();
}

void LoginAdminForm::on_pushButtonBrowseKeyFile_clicked()
{
	QString file_path = QFileDialog::getSaveFileName(0, "Choose a Key File location");

	if (!file_path.isEmpty())
		lineEditKeyFilePath->setText(file_path);
}

void LoginAdminForm::on_pushButtonBrowsePubKey_clicked()
{
	QString file_path = QFileDialog::getOpenFileName(0, "Choose a Server Public Key file");
	if (file_path.isEmpty()) return;

	QFile file(file_path);
	if (!file.exists()) return;

	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, "Unable to open file", file_path + " is not accessible");
		return;
	}

	lineEditPublicKey->setText(file.readLine());
}

void LoginAdminForm::DisplayMessageBox(QString str)
{
	QMessageBox::warning(this, "Splane - Notice", str);
}

void LoginAdminForm::on_pushButtonConnect_clicked()
{
	// If connecting with invalid login info,
	if (!IsValidLoginInfo())
	{
		// Abort
		return;
	}

	bool connecting = false;

	if (!_client)
	{
		// Create a new client object
		_client = new LoginAdminClient(this);
		_client->AddRef();
		connecting = true;
	}

	// Set server key
	ThreadPoolLocalStorage tls;
	u8 public_key[sphynx::PUBLIC_KEY_BYTES];

	if (sizeof(public_key) != ReadBase64(lineEditPublicKey->text().toLatin1().data(), lineEditPublicKey->text().toLatin1().size(), public_key, sizeof(public_key)))
	{
		QMessageBox::warning(this, "Invalid Server Public Key", "The length is wrong.  Please enter a valid Server Public Key");
		return;
	}

	if (!_client->SetServerKey(&tls, public_key, sizeof(public_key), "Login"))
	{
		QMessageBox::warning(this, "Invalid Server Public Key", "Please enter a valid Server Public Key");
		return;
	}

	if (!checkBoxUseKeyFile->isChecked())
	{
		_client->SetLoginNoKey(
			lineEditUserName->text().toLatin1().data(),
			lineEditAlias->text().toLatin1().data(),
			lineEditPassword->text().toLatin1().data());
	}
	else
	{
		u8 public_key[64], private_key[32];

		ifstream file(lineEditKeyFilePath->text().toLatin1().data(), ios::binary);
		if (!file)
		{
			ofstream output(lineEditKeyFilePath->text().toLatin1().data(), ios::binary);
			if (!output)
			{
				QMessageBox::warning(this, "Unable to generate key", "Unable to overwrite file");
				return;
			}
			else
			{
				ThreadPoolLocalStorage tls;

				if (!tls.Valid())
				{
					QMessageBox::warning(this, "Unable to generate key", "TLS failure");
					return;
				}
				else
				{
					QMessageBox::warning(this, "Creating Key File", "Unable to read specified key file.  Creating a new key.");

					KeyMaker bob;
					if (!bob.GenerateKeyPair(tls.math, tls.csprng, public_key, 64, private_key, 32))
					{
						QMessageBox::warning(this, "Unable to generate key", "KeyMaker failure");
						return;
					}
					else
					{
						output.write((char*)public_key, sizeof(public_key));
						output.write((char*)private_key, sizeof(private_key));

						WARN("LoginAdminForm") << "Wrote public and private key pair to " << lineEditKeyFilePath->text().toLatin1().data();
					}
				}
			}
		}
		else
		{
			file.read((char*)public_key, 64);
			file.read((char*)private_key, 32);

			if (file.fail())
			{
				QMessageBox::warning(this, "Invalid Key File", "Truncated key file");
				return;
			}

			WARN("LoginAdminForm") << "Read public and private key pair from " << lineEditKeyFilePath->text().toLatin1().data();
		}

		_client->SetLoginHasKey(
			lineEditUserName->text().toLatin1().data(),
			lineEditAlias->text().toLatin1().data(),
			lineEditPassword->text().toLatin1().data(),
			public_key, private_key);
	}

	SetConnectStatus(true);

	if (connecting)
	{
		_client->Connect(lineEditIP->text().toLatin1().data(), lineEditPort->text().toLong());
	}
	else
	{
		_client->PostLogin();
	}
}

void LoginAdminForm::on_pushButtonDisconnect_clicked()
{
	DisconnectClient();
}

void LoginAdminForm::on_checkBoxUseKeyFile_toggled(bool checked)
{
	lineEditKeyFilePath->setEnabled(checked);
	pushButtonBrowseKeyFile->setEnabled(checked);
}

void LoginAdminForm::on_lineEditUserName_textEdited(const QString &text)
{
	lineEditAlias->setText(lineEditUserName->text());
}

void LoginAdminForm::on_lineEditChat_returnPressed()
{
	if (_client) _client->PostChat(lineEditChat->text().toLatin1().data());

	lineEditChat->clear();
}


//// Network Events

void LoginAdminForm::on_connect()
{
}

void LoginAdminForm::on_disconnect()
{
	DisconnectClient();
}

void LoginAdminForm::on_unknown_username()
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Splane - New User",
		"Unknown Account.\nWould you like to create a new Account?",
		QMessageBox::Yes, QMessageBox::No))
	{
		if (_client) _client->PostCreateAccount();
	}
}

void LoginAdminForm::on_unknown_alias()
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Splane - New Alias",
		"Unknown Alias.\nWould you like to create a new Alias?",
		QMessageBox::Yes, QMessageBox::No))
	{
		if (_client) _client->PostCreateAlias();
	}
}

void LoginAdminForm::on_login_failure()
{
	pushButtonConnect->setEnabled(true);
	pushButtonDisconnect->setEnabled(true);
	groupBoxAddress->setEnabled(true);
	groupBoxPubKey->setEnabled(true);
	groupBoxPassword->setEnabled(true);
	groupBoxKeyFile->setEnabled(true);

	tabWidget->removeTab(tabWidget->indexOf(tabLog));
	tabWidget->removeTab(tabWidget->indexOf(tabChat));
	tabWidget->removeTab(tabWidget->indexOf(tabActive));
	tabWidget->removeTab(tabWidget->indexOf(tabUsers));
	tabWidget->removeTab(tabWidget->indexOf(tabBans));
	tabWidget->removeTab(tabWidget->indexOf(tabZones));

	statusbar->showMessage("Login failed");
	textEditStatus->setEnabled(true);
}

void LoginAdminForm::on_login_success()
{
	SetConnected();
}

void LoginAdminForm::on_add_player(QString name)
{
	_player_list.append(name);
	_player_list_model->setStringList(_player_list);

	textEditChat->append("Player joined: " + name);
}

void LoginAdminForm::on_kill_player(QString name)
{
	_player_list.removeOne(name);
	_player_list_model->setStringList(_player_list);

	textEditChat->append("Player left: " + name);
}

void LoginAdminForm::on_chat_message(QString name, QString message)
{
	textEditChat->append("<" + name + "> " + message);
}
