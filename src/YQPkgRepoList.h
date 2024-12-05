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


#ifndef YQPkgRepoList_h
#define YQPkgRepoList_h

#include <zypp/Repository.h>
#include <zypp/Product.h>
#include "QY2ListView.h"

#include "YQZypp.h"


class YQPkgRepoListItem;

typedef zypp::Repository	ZyppRepo;


/**
 * Display a list of libzypp repositories.
 **/
class YQPkgRepoList : public QY2ListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgRepoList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgRepoList();

    /**
     * Returns the number of enabled repositories.
     **/
    static int countEnabledRepositories();


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();

    /**
     * Add a repository to the list.
     **/
    void addRepo( ZyppRepo repo );


public:

    // Column numbers

    int nameCol()	const	{ return _nameCol;	}
    int urlCol()	const	{ return _urlCol;	}


    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgRepoListItem * selection() const;


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package comes from the respective repository
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package does not come from the respective repository
     **/
    void filterNearMatch( ZyppSel	selectable,
			  ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    /**
     * Fill the list.
     **/
    void fillList();


private:

    //
    // Data members
    //

    int	_nameCol;
    int	_urlCol;

};



class YQPkgRepoListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgRepoListItem( YQPkgRepoList *parentList, ZyppRepo repo );

    /**
     * Destructor
     **/
    virtual ~YQPkgRepoListItem();

    /**
     * Returns the ZYPP repository this item corresponds to
     **/
    ZyppRepo zyppRepo() const { return _zyppRepo; }

    /**
     * Returns the parent list
     **/
    const YQPkgRepoList * repoList() const { return _repoList; }

    /**
     * Returns the product on a source if it has one single product
     * or 0 if there are no or multiple products.
     **/
    static ZyppProduct singleProduct( ZyppRepo repo );


    // Columns

    int nameCol()	const	{ return _repoList->nameCol();	}
    int urlCol()	const 	{ return _repoList->urlCol(); 	}

    virtual bool operator< ( const QTreeWidgetItem & other ) const;
protected:

    // Data members

    YQPkgRepoList *		_repoList;
    ZyppRepo			_zyppRepo;
};


#endif // ifndef YQPkgRepoList_h
