/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include "Exception.h"
#include "Logger.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgUpdatesFilterView.h"

#ifndef VERBOSE_FILTER_VIEWS
#  define VERBOSE_FILTER_VIEWS  0
#endif


YQPkgUpdatesFilterView::YQPkgUpdatesFilterView( QWidget * parent )
    : QWidget( parent )
    , _ui( new Ui::UpdatesFilterView )  // Use the Qt designer .ui form (XML)
{
    CHECK_NEW( _ui );
    _ui->setupUi( this ); // Actually create the widgets from the .ui form

    // See ui_updates-filter-view.h  in the build/ tree for the widget names.
    //
    // That header is generated by Qt's uic (user interface compiler)
    // from the XML .ui file created with Qt designer.
    //
    // Take care in Qt designer to give each widget a meaningful name in the
    // widget tree at the top right: They are also the member variable names
    // for the _ui object.

    connectWidgets();
}


YQPkgUpdatesFilterView::~YQPkgUpdatesFilterView()
{
    delete _ui;
}


void
YQPkgUpdatesFilterView::connectWidgets()
{
    connect( _ui->packageUpdateButton, SIGNAL( clicked()         ),
             this,                     SLOT  ( doPackageUpdate() ) );

    connect( _ui->distUpgradeButton,   SIGNAL( clicked()         ),
             this,                     SLOT  ( doDistUpgrade()   ) );

    connect( _ui->refreshListButton,   SIGNAL( clicked()         ),
             this,                     SLOT  ( refreshList()     ) );
}


void YQPkgUpdatesFilterView::doPackageUpdate()
{
    YQPkgConflictDialog::instance()->doPackageUpdate();
}


void YQPkgUpdatesFilterView::doDistUpgrade()
{
    YQPkgConflictDialog::instance()->doDistUpgrade();
}


void YQPkgUpdatesFilterView::refreshList()
{
    filter();
}


void
YQPkgUpdatesFilterView::showFilter( QWidget * newFilter )
{
    if ( newFilter == this )
        filter();
}


void
YQPkgUpdatesFilterView::filter()
{
 #if VERBOSE_FILTER_VIEWS
    logVerbose() << "Filtering" << endl;
#endif

    emit filterStart();

    for ( ZyppPoolIterator it = zyppPkgBegin();
          it != zyppPkgEnd();
          ++it )
    {
        ZyppSel selectable = *it;

        if ( isUpdateAvailableFor( selectable ) )
        {
            ZyppObj installed = selectable->installedObj();
            ZyppPkg zyppPkg   = tryCastToZyppPkg( installed );

            if ( zyppPkg )
                emit filterMatch( selectable, zyppPkg );
        }
    }

    emit filterFinished();
}


int
YQPkgUpdatesFilterView::countUpdates()
{
    int count = 0;

    for ( ZyppPoolIterator it = zyppPkgBegin();
          it != zyppPkgEnd();
          ++it )
    {
        if ( isUpdateAvailableFor( *it ) )
            ++count;
    }

    return count;
}


bool
YQPkgUpdatesFilterView::isUpdateAvailableFor( ZyppSel selectable )
{
    bool haveUpdate = false;
    const ZyppObj candidate = selectable->candidateObj();
    const ZyppObj installed = selectable->installedObj();

    if ( candidate && installed )
    {
        if ( installed->edition() < candidate->edition() )
        {
            ZyppPkg zyppPkg = tryCastToZyppPkg( installed );

            if ( zyppPkg )
                haveUpdate = true;
        }
    }

    return haveUpdate;
}


QSize
YQPkgUpdatesFilterView::minimumSizeHint() const
{
    QSize size( _ui->distUpgradeButton->sizeHint() );
    size.setWidth( size.width() * 1.6 );

    return size;
}

QSize
YQPkgUpdatesFilterView::sizeHint() const
{
    return minimumSizeHint();
}

