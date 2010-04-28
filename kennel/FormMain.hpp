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

#ifndef CAT_FORM_MAIN_HPP
#define CAT_FORM_MAIN_HPP

#include "UiFormMain.hpp"
#include <cat/AllFramework.hpp>

namespace cat {


class QDebugStream : public QObject, public std::basic_streambuf<char>
{
	Q_OBJECT

signals:
	void appendText(QString);

public:
	QDebugStream(std::ostream &stream) : m_stream(stream)
	{
		m_old_buf = stream.rdbuf();
		stream.rdbuf(this);
	}
	~QDebugStream()
	{
		// output anything that is left
		if (!m_string.empty())
			appendText(m_string.c_str());

		m_stream.rdbuf(m_old_buf);
	}

	void setLogWindow(QTextEdit* text_edit)
	{
		connect(this, SIGNAL(appendText(QString)), text_edit, SLOT(append(QString)), Qt::QueuedConnection);
	}

protected:
	virtual int_type overflow(int_type v)
	{
		if (v == '\n')
		{
			appendText(m_string.c_str());
			m_string.erase(m_string.begin(), m_string.end());
		}
		else
			m_string += v;

		return v;
	}

	virtual std::streamsize xsputn(const char *p, std::streamsize n)
	{
		m_string.append(p, p + n);

		int pos = 0;
		while (pos != std::string::npos)
		{
			pos = (int)m_string.find('\n');
			if (pos != std::string::npos)
			{
				std::string tmp(m_string.begin(), m_string.begin() + pos);
				appendText(tmp.c_str());
				m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
			}
		}

		return n;
	}

private:
	std::ostream &m_stream;
	std::streambuf *m_old_buf;
	std::string m_string;
};


class FormMain : public QMainWindow, private Ui_MainWindow
{
	Q_OBJECT

private:
	QDebugStream _qout;

public:
	FormMain();
};


} // namespace cat

#endif // CAT_FORM_MAIN_HPP
