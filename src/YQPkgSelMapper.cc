/*  ------------------------------------------------------
              __   _____  ____  _         
              \ \ / / _ \|  _ \| | ____ _ 
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include "Logger.h"

#include "YQPkgSelMapper.h"


int			YQPkgSelMapper::_refCount = 0;
YQPkgSelMapper::Cache	YQPkgSelMapper::_cache;


YQPkgSelMapper::YQPkgSelMapper()
{
    if ( ++_refCount == 1 )
	rebuildCache();
}


YQPkgSelMapper::~YQPkgSelMapper()
{
    if ( --_refCount == 0 )
    {
	logDebug() << "Destroying pkg -> selectable cache"  << endl;
	_cache.clear();
    }
}


void YQPkgSelMapper::rebuildCache()
{
    _cache.clear();
    logDebug() << "Building pkg -> selectable cache" << endl;

    for ( ZyppPoolIterator sel_it = zyppPkgBegin();
	  sel_it != zyppPkgEnd();
	  ++sel_it )
    {
	ZyppSel sel = *sel_it;

	if ( sel->installedObj() )
	{
	    // The installed package (if there is any) may or may not be in the list
	    // of available packages. Better make sure to insert it.

	    ZyppPkg installedPkg = tryCastToZyppPkg( sel->installedObj() );

	    if ( installedPkg )
		_cache.insert( CachePair( installedPkg, sel ) );
	}

	zypp::ui::Selectable::available_iterator it = sel->availableBegin();

	while ( it != sel->availableEnd() )
	{
	    ZyppPkg pkg = tryCastToZyppPkg( *it );

	    if ( pkg )
		_cache.insert( CachePair( pkg, sel ) );

	    ++it;
	}
    }

    logDebug() << "Building pkg -> selectable cache done" << endl;
}


ZyppSel
YQPkgSelMapper::findZyppSel( ZyppPkg pkg )
{
    YQPkgSelMapper mapper; // This will build a cache, if there is none yet
    ZyppSel sel;

    YQPkgSelMapper::CacheIterator it = YQPkgSelMapper::_cache.find( pkg );

    if ( it != YQPkgSelMapper::_cache.end() )
	sel = it->second;
    else
	logWarning() << "No selectable found for package " << pkg->name() << endl;

    return sel;
}

