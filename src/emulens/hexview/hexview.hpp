/*
The MIT License (MIT)

Copyright (c) 2015

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Source: https://github.com/virinext/QHexView

#pragma once

#include <QAbstractScrollArea>
#include <QByteArray>
#include <QFile>

class HexView: public QAbstractScrollArea

{
	public:
		class DataStorage
		{
			public:
				virtual ~DataStorage() {};
				virtual QByteArray getData(std::size_t position, std::size_t length) = 0;
				virtual std::size_t size() = 0;
		};


		class DataStorageArray: public DataStorage
		{
			public:
				DataStorageArray(const QByteArray &arr);
				virtual QByteArray getData(std::size_t position, std::size_t length);
				virtual std::size_t size();
			private:
				QByteArray    m_data;
		};

		class DataStorageFile: public DataStorage
		{
			public:
				DataStorageFile(const QString &fileName);
				virtual QByteArray getData(std::size_t position, std::size_t length);
				virtual std::size_t size();
			private:
				QFile      m_file;
		};



		HexView(QWidget *parent = 0);
		~HexView();

	public slots:
		void setData(DataStorage *pData);
		void clear();
		void showFromOffset(std::size_t offset);

	protected:
		void paintEvent(QPaintEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
	private:
		DataStorage          *m_pdata;
		std::size_t           m_posAddr;
		std::size_t           m_posHex;
		std::size_t           m_posAscii;
		std::size_t           m_charWidth;
		std::size_t           m_charHeight;


		std::size_t           m_selectBegin;
		std::size_t           m_selectEnd;
		std::size_t           m_selectInit;
		std::size_t           m_cursorPos;


		QSize fullSize() const;
		void resetSelection();
		void resetSelection(int pos);
		void setSelection(int pos);
		void ensureVisible();
		void setCursorPos(int pos);
		std::size_t cursorPos(const QPoint &position);
};
