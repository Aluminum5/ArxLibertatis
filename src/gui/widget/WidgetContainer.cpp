/*
 * Copyright 2015-2021 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/widget/WidgetContainer.h"

#include "core/ArxGame.h"
#include "graphics/DrawLine.h"

WidgetContainer::~WidgetContainer() {
	for(Widget * w : m_widgets) {
		delete w;
	}
}

void WidgetContainer::update() {
	for(Widget * w : m_widgets) {
		w->update();
	}
}

void WidgetContainer::render(const Widget * selected) {
	for(Widget * w : m_widgets) {
		w->render(w == selected);
	}
}

void WidgetContainer::add(Widget * widget) {
	m_widgets.push_back(widget);
}

Widget * WidgetContainer::getWidgetAt(const Vec2f & mousePos) const {
	
	for(Widget * widget : m_widgets) {
		
		if(!widget->isEnabled()) {
			continue;
		}
		
		if(Widget * mouseOverWidget = widget->getWidgetAt(mousePos)) {
			return mouseOverWidget;
		}
		
	}
	
	return nullptr;
}

void WidgetContainer::move(const Vec2f & offset) {
	for(Widget * w : m_widgets) {
		w->move(offset);
	}
}

void WidgetContainer::drawDebug() {
	
	if(g_debugInfo != InfoPanelGuiDebug) {
		return;
	}
	
	for(Widget * w : m_widgets) {
		drawLineRectangle(w->m_rect, 0.f, Color::red);
	}
	
}
