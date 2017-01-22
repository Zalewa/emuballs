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
#include "display.hpp"

#include <emuballs/canvas.hpp>
#include <emuballs/color.hpp>
#include <emuballs/picture.hpp>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QVBoxLayout>

namespace Emulens
{
class Canvas : public Emuballs::Canvas
{
public:
	Canvas(QGraphicsScene *scene)
	{
		this->scene = scene;
		this->drawTarget.reset(new QGraphicsPixmapItem());
		scene->addItem(drawTarget.get());
	}

	void begin() override
	{
	}

	void end() override
	{
		drawTarget->setPixmap(QPixmap::fromImage(image));
	}

	void drawPixel(int x, int y, const Emuballs::Color &color)
	{
		image.setPixel(x, y, 0xff000000 |
			static_cast<uint32_t>(color.r) << 16 |
			static_cast<uint32_t>(color.g) << 8 |
			static_cast<uint32_t>(color.b));
	}

	void setPicture(const Emuballs::Picture &picture) override
	{
		image = QImage(picture.width(), picture.height(), QImage::Format_RGB32);
	}

private:
	QGraphicsScene *scene;
	std::unique_ptr<QGraphicsPixmapItem> drawTarget;
	QImage image;
};

}

using namespace Emulens;

DClass<Display>
{
public:
	std::shared_ptr<Emuballs::Device> device;
	QGraphicsScene scene;
	QGraphicsView *graphics;
	std::unique_ptr<Canvas> canvas;

	void fitView()
	{
		graphics->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
	}
};

DPointeredNoCopy(Display)

Display::Display(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
: QWidget(parent)
{
	d->device = device;
	d->graphics = new QGraphicsView(&d->scene, this);
	d->canvas.reset(new Canvas(&d->scene));
	setLayout(new QVBoxLayout(this));
	setWindowTitle(tr("Display"));
	layout()->setContentsMargins(0, 0, 0, 0);
	layout()->addWidget(d->graphics);
}

void Display::resizeEvent(QResizeEvent *)
{
	d->fitView();
}

void Display::update()
{
	d->device->draw(*d->canvas);
	d->fitView();
}
