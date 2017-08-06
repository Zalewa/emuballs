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
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QVBoxLayout>

namespace Emulens
{
class Canvas : public Emuballs::Canvas
{
public:
	Canvas(QGraphicsView *graphics, QGraphicsScene *scene)
	{
		this->graphics = graphics;
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
		fitView();
	}

	void drawPicture(int32_t x, int32_t y, int32_t width, int32_t height,
		int32_t bitsPerPixel, int32_t pitch, const std::vector<uint8_t> &pixels) override
	{
		std::copy(pixels.begin(), pixels.end(), image.bits());
	}

	void drawPixel(int32_t x, int32_t y, const Emuballs::Color &color) override
	{
		image.setPixel(x, y, 0xff000000 |
			static_cast<uint32_t>(color.r) << 16 |
			static_cast<uint32_t>(color.g) << 8 |
			static_cast<uint32_t>(color.b));
	}

	void changeSize(int32_t width, int32_t height, Emuballs::BitDepth depth) override
	{
		auto format = QImage::Format_RGB32;
		switch (depth)
		{
		case Emuballs::BitDepth::HighColor:
			format = QImage::Format_RGB16;
			break;
		default:
			// Stick with RAII value.
			break;
		}
		image = QImage(width, height, format);
	}

	void fitView()
	{
		graphics->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	}

private:
	QGraphicsView *graphics;
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
};

DPointeredNoCopy(Display)

Display::Display(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
: QWidget(parent)
{
	d->device = device;
	d->graphics = new QGraphicsView(&d->scene, this);
	d->canvas.reset(new Canvas(d->graphics, &d->scene));
	setLayout(new QVBoxLayout(this));
	setWindowTitle(tr("Display"));
	layout()->setContentsMargins(0, 0, 0, 0);
	layout()->addWidget(d->graphics);
}

void Display::resizeEvent(QResizeEvent *)
{
	d->canvas->fitView();
}

void Display::update()
{
	d->device->draw(*d->canvas);
}
