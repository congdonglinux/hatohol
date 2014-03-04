/*
 * Copyright (C) 2013 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hatohol. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DataStore_h
#define DataStore_h

#include <map>
#include <vector>
#include <ArmBase.h>
#include "UsedCountable.h"

class DataStore : public UsedCountable {
public:
	DataStore(void);

	virtual ArmBase &getArmBase(void) = 0;
	virtual void setCopyOnDemandEnable(bool enable);
protected:
	virtual ~DataStore();
};

typedef std::vector<DataStore *>        DataStoreVector;
typedef DataStoreVector::iterator       DataStoreVectorIterator;
typedef DataStoreVector::const_iterator DataStoreVectorConstIterator;

#endif // DataStore_h
