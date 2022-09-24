/*
 * Copyright 2011-2022 Arx Libertatis Team (see the AUTHORS file)
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

#include "io/IniSection.h"

#include <sstream>
#include <ios>

#include "io/log/Logger.h"

#include "util/Number.h"
#include "util/String.h"

int IniKey::getValue(int defaultValue) const {
	return (value == "false") ? 0 : (value == "true") ? 1 : util::toInt(value, true).value_or(defaultValue);
}

float IniKey::getValue(float defaultValue) const {
	return util::toFloat(value).value_or(defaultValue);
}

bool IniKey::getValue(bool defaultValue) const {
	
	if("false" == value || "0" == value) {
		return false;
	} else if("true" == value || "1" == value) {
		return true;
	} else {
		return defaultValue;
	}
	
}

const IniKey * IniSection::getKey(std::string_view name) const {
	
	// Try to match the key to one in the section
	for(const IniKey & key : keys) {
		if(key.getName() == name) { // If the key name matches that specified
			return &key;
		}
	}
	
	// If the key was not found, return nullptr
	return nullptr;
}

IniKey & IniSection::addKey(std::string_view key, std::string_view value, bool overrideValues) {
	
	IniKey k(util::toLowercase(key), std::string(value));
	
	if(overrideValues) {
		for(IniKey & old : keys) {
			if(old.name == k.name) {
				LogDebug("replaced key " << k.name << "=\"" << k.value << "\"" << " old: " << "\"" << old.value << "\"");
				old = std::move(k);
				return old;
			}
		}
	}
	
	LogDebug("added key " << k.name << "=\"" << k.value << "\"");
	return keys.emplace_back(std::move(k));
}
