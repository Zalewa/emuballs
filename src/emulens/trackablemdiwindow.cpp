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
#include "trackablemdiwindow.hpp"

namespace Emulens
{

TrackableMdiWindow::TrackableMdiWindow(QWidget *parent) : QMdiSubWindow(parent) {}

void TrackableMdiWindow::closeEvent(QCloseEvent *event)
{
	emit stateChanged();
	QMdiSubWindow::closeEvent(event);
}

void TrackableMdiWindow::hideEvent(QHideEvent *event)
{
	emit stateChanged();
	QMdiSubWindow::hideEvent(event);
}

void TrackableMdiWindow::showEvent(QShowEvent *event)
{
	emit stateChanged();
	QMdiSubWindow::showEvent(event);
}

}
