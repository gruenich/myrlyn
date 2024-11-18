/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*
  File:	      YQPkgStatusFilterView.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include "Logger.h"

#include <yui/qt/YQUI.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QPixmap>

#include "YQPkgStatusFilterView.h"
#include "YQIconPool.h"
#include "QY2LayoutUtils.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgStatusFilterView::YQPkgStatusFilterView( QWidget * parent )
    : QScrollArea( parent )
{
    QWidget * content = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout();
    content->setLayout(layout);
    layout->setMargin( MARGIN );
    layout->setSpacing( SPACING );

    layout->addStretch();

#if 0
    // Headline
    QLabel * label = new QLabel( _( "Changes Overview" ), this );
    Q_CHECK_PTR( label );
    label->setFont( YQUI::ui()->headingFont() );
#endif


    //
    // Packages with what status to show
    //

    QGroupBox * gbox = new QGroupBox( _( "Show packages with status" ), content );
    Q_CHECK_PTR( gbox );

    QVBoxLayout *box = new QVBoxLayout;
    gbox->setLayout(box);
    layout->addWidget(gbox);

    _showDel		= addStatusCheckBox( gbox, _( "Delete" ), 	YQIconPool::pkgDel(),	   	true );
    _showInstall	= addStatusCheckBox( gbox, _( "Install" ), 	YQIconPool::pkgInstall(),	true );
    _showUpdate		= addStatusCheckBox( gbox, _( "Update" ), 	YQIconPool::pkgUpdate(),	true );
    _showAutoDel	= addStatusCheckBox( gbox, _( "Autodelete" ), 	YQIconPool::pkgAutoDel(),	true );
    _showAutoInstall	= addStatusCheckBox( gbox, _( "Autoinstall" ), 	YQIconPool::pkgAutoInstall(),   true );
    _showAutoUpdate	= addStatusCheckBox( gbox, _( "Autoupdate" ), 	YQIconPool::pkgAutoUpdate(),	true );
    _showTaboo		= addStatusCheckBox( gbox, _( "Taboo" ), 	YQIconPool::pkgTaboo(),	   	true );
    _showProtected	= addStatusCheckBox( gbox, _( "Protected" ), 	YQIconPool::pkgProtected(),	true );

    box->addSpacing( 8 );
    box->addStretch(); // For the other columns of the QGroupBox ( prevent wraparound )
    box->addStretch();

    _showKeepInstalled	= addStatusCheckBox( gbox, _( "Keep" ), 	  YQIconPool::pkgKeepInstalled(), false );
    _showNoInst		= addStatusCheckBox( gbox, _( "Do not install" ), YQIconPool::pkgNoInst(),	  false );

    layout->addStretch();


    // Box for refresh button
    QHBoxLayout *hbox = new QHBoxLayout();
    layout->addLayout(hbox);

    hbox->addStretch();

    // Refresh button
    _refreshButton = new QPushButton( _( "&Refresh List" ), content );
    Q_CHECK_PTR( _refreshButton );
    hbox->addWidget(_refreshButton);

    hbox->addStretch();

    connect( _refreshButton,	SIGNAL( clicked() ),
	     this,		SLOT  ( filter()  ) );

    for ( int i=0; i < 6; i++ )
	layout->addStretch();

    setWidgetResizable(true);
    setWidget(content);
}


YQPkgStatusFilterView::~YQPkgStatusFilterView()
{
    // NOP
}



QCheckBox *
YQPkgStatusFilterView::addStatusCheckBox( QWidget *		parent,
					  const QString &	text,
					  const QPixmap &	icon,
					  bool			initiallyChecked )
{
    QBoxLayout *layout = dynamic_cast<QBoxLayout*>(parent->layout());

    QHBoxLayout *hbox = new QHBoxLayout;
    layout->addLayout(hbox);

    QCheckBox * checkBox = new QCheckBox( text, parent );
    Q_CHECK_PTR( checkBox );
    checkBox->setChecked( initiallyChecked );

    hbox->addWidget(checkBox);

    QLabel * label = new QLabel( parent );
    Q_CHECK_PTR( label );
    label->setPixmap( icon );

    hbox->addWidget(label);

    layout->addStretch();

    connect( checkBox,	SIGNAL( clicked() ),
	     this,	SLOT  ( filter()  ) );

    return checkBox;
}


QSize
YQPkgStatusFilterView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


void
YQPkgStatusFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgStatusFilterView::filter()
{
    emit filterStart();

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppSel selectable = *it;

	bool match =
	    check( selectable, selectable->candidateObj() ) ||
	    check( selectable, selectable->installedObj() );

	// If there is neither an installed nor a candidate package, check
	// any other instance.

	if ( ! match			  &&
	     ! selectable->candidateObj() &&
	     ! selectable->installedObj()   )
	    check( selectable,  selectable->theObj() );
    }

    emit filterFinished();
}


bool
YQPkgStatusFilterView::check( ZyppSel	selectable,
			      ZyppObj	zyppObj )
{
    bool match = false;

    if ( ! zyppObj )
	return false;

    switch ( selectable->status() )
    {
	case S_AutoDel:		match = _showAutoDel->isChecked();		break;
	case S_AutoInstall:	match = _showAutoInstall->isChecked();		break;
	case S_AutoUpdate:	match = _showAutoUpdate->isChecked();		break;
	case S_Del:		match = _showDel->isChecked();			break;
	case S_Install:		match = _showInstall->isChecked();		break;
	case S_KeepInstalled:	match = _showKeepInstalled->isChecked();	break;
	case S_NoInst:		match = _showNoInst->isChecked();		break;
	case S_Protected:	match = _showProtected->isChecked();		break;
	case S_Taboo:		match = _showTaboo->isChecked();		break;
	case S_Update:		match = _showUpdate->isChecked();		break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }

    if ( match )
    {
	ZyppPkg zyppPkg = tryCastToZyppPkg( zyppObj );

	if ( zyppPkg )
	    emit filterMatch( selectable, zyppPkg );
    }

    return match;
}



void YQPkgStatusFilterView::clear()
{
    _showDel->setChecked( false );
    _showInstall->setChecked( false );
    _showUpdate->setChecked( false );
    _showAutoDel->setChecked( false );
    _showAutoInstall->setChecked( false );
    _showAutoUpdate->setChecked( false );
    _showTaboo->setChecked( false );
    _showProtected->setChecked( false );
    _showKeepInstalled->setChecked( false );
    _showNoInst->setChecked( false );
}


void YQPkgStatusFilterView::showTransactions()
{
    showManualTransactions();
    showAutoTransactions();
}


void YQPkgStatusFilterView::showManualTransactions()
{
    _showDel->setChecked( true );
    _showInstall->setChecked( true );
    _showUpdate->setChecked( true );
}


void YQPkgStatusFilterView::showAutoTransactions()
{
    _showAutoDel->setChecked( true );
    _showAutoInstall->setChecked( true );
    _showAutoUpdate->setChecked( true );
}


void YQPkgStatusFilterView::showLocks()
{
    _showTaboo->setChecked( true );
    _showProtected->setChecked( true );
}


void YQPkgStatusFilterView::showInstalled()
{
    _showKeepInstalled->setChecked( true );
}


void YQPkgStatusFilterView::showNotInstalled()
{
    _showNoInst->setChecked( true );
}



