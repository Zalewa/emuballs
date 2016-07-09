/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emulens.
 *
 * Emulens is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emulens is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Emulens.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "editarray.hpp"

#include "errors.hpp"

#include <QFont>
#include <QFontDatabase>
#include <QFormLayout>
#include <QLineEdit>
#include <QMap>

using namespace Emulens;

DClass<EditArray>
{
public:
	bool readOnly;
	QFormLayout *layout;
	QMap<QString, QLineEdit*> editors;
	QString inputMask;

	QFont font() const
	{
		return QFontDatabase::systemFont(QFontDatabase::FixedFont);
	}
};

DPointeredNoCopy(EditArray)

EditArray::EditArray(QWidget *parent)
	: QWidget(parent)
{
	d->readOnly = false;
	d->layout = new QFormLayout(this);
	d->layout->setContentsMargins(0, 0, 0, 0);
	setAutoFillBackground(true);
	setLayout(d->layout);
	setFont(d->font());
	setStyleSheet("QLineEdit {"
		"border: 0px;"
		"padding: 0px;"
		"}");
}

void EditArray::addEditor(const QString &displayName, const QString &id)
{
	if (d->editors.contains(id))
	{
		throw new DuplicateIdError("editor with id " + displayName.toStdString()
			+ " already exists");
	}

	auto *editor = new QLineEdit(this);
	editor->setFont(d->font());
	editor->setInputMask(d->inputMask);
	editor->setReadOnly(d->readOnly);
	connect(editor, &QLineEdit::editingFinished, this, &EditArray::onValueEdited);

	d->editors.insert(id, editor);
	d->layout->addRow(displayName, editor);
}

void EditArray::setInputMask(const QString &inputMask)
{
	d->inputMask = inputMask;
	for (auto *editor : d->editors.values())
		editor->setInputMask(inputMask);
}

void EditArray::setReadOnly(bool readOnly)
{
	d->readOnly = readOnly;
	for (auto *editor : d->editors)
		editor->setReadOnly(readOnly);
}

void EditArray::setValue(const QString &id, const QString &value)
{
	auto *editor = d->editors[id];
	if (editor == nullptr)
		throw KeyError("no editor " + id.toStdString());
	editor->setText(value);
}

QString EditArray::value(const QString &id) const
{
	auto *editor = d->editors[id];
	if (editor == nullptr)
		throw KeyError("no editor " + id.toStdString());
	return editor->text();
}

void EditArray::onValueEdited()
{
	auto *widget = qobject_cast<QLineEdit*>(sender());
	for (const auto &candidateId : d->editors.keys())
	{
		if (d->editors[candidateId] == widget)
		{
			emit valueEdited(candidateId, widget->text());
			return;
		}
	}
}
