/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include <QMessageBox>

#include <zypp/Target.h>

#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "MyrlynApp.h"
#include "WindowSettings.h"
#include "utf8.h"
#include "YQi18n.h"
#include "YQZypp.h"
#include "RepoEditDialog.h"
#include "RepoConfigDialog.h"


RepoConfigDialog::RepoConfigDialog( QWidget * parent )
    : QDialog( parent ? parent : MainWindow::instance() )
    , _ui( new Ui::RepoConfig )  // Use the Qt designer .ui form (XML)
    , _repoManager( MyrlynApp::instance()->repoManager()->repoManager() )
    , _restartNeeded( false )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this ); // Actually create the widgets from the .ui form

    // See ui_repo-config.h for the widget names.
    //
    // That header is generated by Qt's uic (user interface compiler)
    // from the XML .ui file created with Qt designer.
    //
    // Take care in Qt designer to give each widget a meaningful name in the
    // widget tree at the top right: They are also the member variable names
    // for the _ui object.

    WindowSettings::read( this, "RepoConfigDialog" );
    _ui->currentRepoName->setTextFormat( Qt::RichText );
    restartNeeded( false );
    _ui->repoTable->populate();
    _ui->repoTable->selectSomething();
    updateCurrentData();
    connectWidgets();

    if ( ! MyrlynApp::runningAsRoot() )
    {
        setReadOnlyMode( true );

        QString winTitle = windowTitle();
        winTitle += _( " (read-only)" );
        setWindowTitle( winTitle );
    }
}


RepoConfigDialog::~RepoConfigDialog()
{
    WindowSettings::write( this, "RepoConfigDialog" );

    delete _ui;
}


void RepoConfigDialog::setReadOnlyMode( bool readOnly )
{
    _ui->currentRepoPriority->setEnabled   ( ! readOnly );
    _ui->currentRepoEnabled->setEnabled    ( ! readOnly );
    _ui->currentRepoAutoRefresh->setEnabled( ! readOnly );

    _ui->addButton->setEnabled   ( ! readOnly );
    _ui->editButton->setEnabled  ( ! readOnly );
    _ui->deleteButton->setEnabled( ! readOnly );
}


void RepoConfigDialog::connectWidgets()
{
    connect( _ui->repoTable, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * ) ),
             this,           SLOT  ( currentChanged() ) );

    connect( _ui->currentRepoPriority,    SIGNAL( valueChanged ( int ) ),
             this,                        SLOT  ( currentEdited()      ) );

    connect( _ui->currentRepoEnabled,     SIGNAL( stateChanged ( int ) ),
             this,                        SLOT  ( currentEdited()      ) );

    connect( _ui->currentRepoAutoRefresh, SIGNAL( stateChanged ( int ) ),
             this,                        SLOT  ( currentEdited()      ) );

#if 0

    connect( _ui->repoTable, SIGNAL( columnDoubleClicked( int button, QTreeWidgetItem * item, int col, const QPoint & ) ),
             this,           SLOT  ( itemDoubleClicked  ( int button, QTreeWidgetItem * item, int col ) ) );;
#endif

    connect( _ui->addButton,    SIGNAL( clicked() ),
             this,              SLOT  ( addRepo() ) );

    connect( _ui->editButton,   SIGNAL( clicked() ),
             this,              SLOT  ( editRepo() ) );

    connect( _ui->deleteButton, SIGNAL( clicked() ),
             this,              SLOT  ( deleteRepo() ) );

    // _ui->closeButton() is already connected to QDialog::accept()
    // in the .ui file
}


void RepoConfigDialog::currentChanged()
{
    updateCurrentData();
}


void RepoConfigDialog::updateCurrentData()
{
    QSignalBlocker blockPrio       ( _ui->currentRepoPriority    );
    QSignalBlocker blockEnabled    ( _ui->currentRepoEnabled     );
    QSignalBlocker blockAutoRefresh( _ui->currentRepoAutoRefresh );

    RepoTableItem * current = _ui->repoTable->currentRepoItem();

    if ( current )
    {
        const ZyppRepoInfo & repoInfo = current->repoInfo();

#if 0
        logDebug() << "Current: "      << repoInfo.name()
                   << " Prio: "        << repoInfo.priority()
                   << " Enabled: "     << repoInfo.enabled()
                   << " AutoRefresh: " << repoInfo.autorefresh()
                   << endl;
#endif

        std::string url    = repoInfo.url().asString();
        std::string rawUrl = repoInfo.rawUrl().asString();

        if ( rawUrl == url )
            rawUrl = "";

        _ui->currentRepoRawUrl->setEnabled( ! rawUrl.empty() );
        _ui->rawUrlCaption->setEnabled    ( ! rawUrl.empty() );

        if ( MyrlynApp::runningAsRoot() )
        {
            _ui->currentRepoPriority->setEnabled( true );
            _ui->currentRepoEnabled->setEnabled( true );
            _ui->currentRepoAutoRefresh->setEnabled( true );
        }

        _ui->currentRepoName->setText( QString( "<b>%1</b>" ).arg( fromUTF8( repoInfo.name() ) ) );
        _ui->currentRepoUrl->setText( fromUTF8( url ) );
        _ui->currentRepoRawUrl->setText( fromUTF8( rawUrl ) );
        _ui->currentRepoPriority->setValue( repoInfo.priority() );
        _ui->currentRepoEnabled->setChecked( repoInfo.enabled() );
        _ui->currentRepoAutoRefresh->setChecked( repoInfo.autorefresh());
    }
    else // Clear all
    {
        _ui->currentRepoName->clear();
        _ui->currentRepoUrl->clear();
        _ui->currentRepoPriority->setValue( 99 );
        _ui->currentRepoEnabled->setChecked( false );
        _ui->currentRepoAutoRefresh->setChecked( false );

        _ui->currentRepoPriority->setEnabled( false );
        _ui->currentRepoEnabled->setEnabled( false );
        _ui->currentRepoAutoRefresh->setEnabled( false );
    }
}


void RepoConfigDialog::currentEdited()
{
    RepoTableItem * currentItem = _ui->repoTable->currentRepoItem();

    if ( currentItem )
    {
        ZyppRepoInfo repoInfo = currentItem->repoInfo();

        repoInfo.setPriority( _ui->currentRepoPriority->value() );
        repoInfo.setEnabled( _ui->currentRepoEnabled->isChecked() );
        repoInfo.setAutorefresh( _ui->currentRepoAutoRefresh->isChecked() );

#if 0
        logDebug() << "Current: "      << repoInfo.name()
                   << " Prio: "        << repoInfo.priority()
                   << " Enabled: "     << repoInfo.enabled()
                   << " AutoRefresh: " << repoInfo.autorefresh()
                   << endl;
#endif

        restartNeeded();
        currentItem->setRepoInfo( repoInfo );
    }
}


void RepoConfigDialog::addRepo()
{
    logDebug() << "Adding repo" << endl;

    RepoEditDialog dialog( RepoEditDialog::AddRepo, this );
    int result = dialog.addRepo();

    if ( result == QDialog::Accepted )
    {
        logDebug() << "User closed the repo edit dialog with 'OK'" << endl;

        ZyppRepoInfo repoInfo = dialog.repoInfo();
#if 1
        logDebug() << "Result: "       << repoInfo.name()
                   << " URL: "         << repoInfo.url().asString()
                   << " Prio: "        << repoInfo.priority()
                   << " Enabled: "     << repoInfo.enabled()
                   << " AutoRefresh: " << repoInfo.autorefresh()
                   << endl;
#endif
        if ( MyrlynApp::runningAsRealRoot() )
            _repoManager->addRepository( repoInfo );
        else
        {
            logWarning() << "Faking adding a new repo "
                         << repoInfo.name() << endl;
        }

        RepoTableItem * item = new RepoTableItem( _ui->repoTable, repoInfo );
        CHECK_NEW( item );
        restartNeeded();
    }
    else
    {
        logDebug() << "User cancelled the dialog" << endl;
    }
}


void RepoConfigDialog::editRepo()
{
    logDebug() << "Editing repo" << endl;

    RepoTableItem * currentItem = _ui->repoTable->currentRepoItem();

    if ( currentItem )
    {
        ZyppRepoInfo repoInfo = currentItem->repoInfo();

        RepoEditDialog dialog( RepoEditDialog::EditRepo, this );
        int result = dialog.editRepo( repoInfo );

        if ( result == QDialog::Accepted )
        {
            logDebug() << "User closed the repo edit dialog with 'OK'" << endl;

            repoInfo = dialog.repoInfo();
#if 1
            logDebug() << "Result: "       << repoInfo.name()
                       << " URL: "         << repoInfo.url().asString()
                       << " Prio: "        << repoInfo.priority()
                       << " Enabled: "     << repoInfo.enabled()
                       << " AutoRefresh: " << repoInfo.autorefresh()
                       << endl;
#endif
            currentItem->setRepoInfo( repoInfo );
            restartNeeded();
        }
        else
        {
            logDebug() << "User cancelled the dialog" << endl;
        }
    }
}


void RepoConfigDialog::restartNeeded( bool needed )
{
    _restartNeeded = needed;
    _ui->restartNeeded->setVisible( needed );
}


void RepoConfigDialog::deleteRepo()
{
    logWarning() << "Not implemented yet" << endl;

    int result = QDialog::Rejected;

    if ( result == QDialog::Accepted )
    {
        // TO DO: actually delete the repo
        // TO DO: delete the item in the repo table
        restartNeeded();
    }
}


void RepoConfigDialog::accept()
{
    if ( _restartNeeded )
    {
        showRestartNeededPopup();
        MyrlynApp::instance()->quit();
    }
    else
    {
        QDialog::accept();
    }
}


void RepoConfigDialog::showRestartNeededPopup()
{
    QMessageBox msgBox( this );

    msgBox.setText( _( "The application needs to be restarted\n"
                       "for the changes to take effect.\n"
                       "\n"
                       "Quitting now. Please restart." ) );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.addButton( QMessageBox::Ok );

    msgBox.exec();
}
