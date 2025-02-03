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

#include <zypp/repo/RepoVariables.h>    // RepoVariablesStringReplacer
#include <zypp/RepoManager.h>           // makeStupidAlias()

#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "WindowSettings.h"
#include "utf8.h"
#include "YQi18n.h"
#include "RepoEditDialog.h"


RepoEditDialog::RepoEditDialog( Mode      mode,
                                QWidget * parent )
    : QDialog( parent ? parent : MainWindow::instance() )
    , _mode( mode )
    , _ui( new Ui::RepoEdit )  // Use the Qt designer .ui form (XML)
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

    showExpandedVariables();
    updateWindowTitle();
    _ui->repoTypeContainer->setVisible( _mode == AddRepo );
    resize( sizeHint() ); // Fallback initial size if there is none in the settings
    connectWidgets();

    WindowSettings::read( this, _mode == AddRepo ? "RepoAddDialog" : "RepoEditDialog" );
}


RepoEditDialog::~RepoEditDialog()
{
    WindowSettings::write( this, _mode == AddRepo ? "RepoAddDialog" : "RepoEditDialog" );

    delete _ui;
}


void RepoEditDialog::showExpandedVariables()
{
    // Expand some variables that can be used in a repo URL

    zypp::repo::RepoVariablesStringReplacer replacer;
    std::string releasever = replacer( "$releasever" );
    std::string basearch   = replacer( "$basearch"   );
    std::string arch       = replacer( "$arch"       );

    QString text( "$releasever: " );  // No translation!
    text += fromUTF8( releasever );
    _ui->releasever->setText( text ); // Only one variable directly in the dialog

    // ...some more as tooltip on that variable to avoid overcrowding the dialog.
    // Could this be more discoverable? Maybe, but this is for total nerds anyway.
    // They can be expected to have some more explorer spirit.

    QString tooltip = QString( "$basearch:  %1\n"
                               "$arch:  %2\n"
                               "\n"
                               "%3" )
        .arg( fromUTF8( basearch ) )
        .arg( fromUTF8( arch     ) )
        .arg( _( "See also  'man zypper'" ) );
    _ui->releasever->setToolTip( tooltip );
}


void RepoEditDialog::updateWindowTitle()
{
    QString title = ( _mode == AddRepo ) ?
        _( "Add Repository" ) : _( "Edit Repository" );

    setWindowTitle( title );
    _ui->heading->setTextFormat( Qt::RichText );
    _ui->heading->setText( QString( "<b>%1</b>" ).arg( title ) );
}


void RepoEditDialog::connectWidgets()
{
    connect( _ui->customRepoRadioButton,    SIGNAL( toggled( bool )   ),
             this,                          SLOT  ( repoTypeChanged() ) );

    connect( _ui->communityRepoRadioButton, SIGNAL( toggled( bool )   ),
             this,                          SLOT  ( repoTypeChanged() ) );

    connect( _ui->repoRawUrl, SIGNAL( textChanged      ( QString ) ),
             this,            SLOT  ( updateExpandedUrl()          ) );
}


int RepoEditDialog::addRepo()
{
    _repoInfo = ZyppRepoInfo();
    _repoInfo.setEnabled( true );
    _repoInfo.setAutorefresh( true );

    _ui->repoTypeContainer->show();
    _ui->repoName->clear();
    _ui->repoRawUrl->clear();
    _ui->expandedUrl->clear();

    int result = exec();

    return result;
}


int RepoEditDialog::editRepo( const ZyppRepoInfo & repoInfo )
{
    _repoInfo = repoInfo;

    _ui->repoTypeContainer->hide();
    _ui->repoName->setText( fromUTF8( _repoInfo.name() ) );
    _ui->repoRawUrl->setText( fromUTF8( _repoInfo.rawUrl().asString() ) );

    int result = exec();

    return result;
}


void RepoEditDialog::repoTypeChanged()
{
    _ui->communityReposList->setEnabled( _ui->communityRepoRadioButton->isChecked() );
}


void RepoEditDialog::updateExpandedUrl()
{
    std::string rawUrl = toUTF8( _ui->repoRawUrl->text() );
    zypp::repo::RepoVariablesStringReplacer replacer;
    std::string expandedUrl = replacer( rawUrl );

    _ui->expandedUrl->setText( fromUTF8( expandedUrl ) );
}


void RepoEditDialog::accept()
{
    try
    {
        saveRepoInfo();
        QDialog::accept();
    }
    catch ( const zypp::url::UrlException & exception )
    {
        logError() << "Caught zypp UrlException: " << exception.asString() << endl;
        showWarningPopup( _( "Invalid URL: " ), exception );
    }
    catch ( const zypp::Exception & exception )
    {
        logError() << "Caught zypp Exception: " << exception.asString() << endl;
        showWarningPopup( _( "Invalid input: " ), exception );
    }
}


void RepoEditDialog::saveRepoInfo()
{
    std::string oldRepoName = _repoInfo.name();
    _repoInfo.setName( toUTF8( _ui->repoName->text() ) );

    // This may throw an exception. Let it escalate to the caller.

    QString urlText = _ui->repoRawUrl->text();
    zypp::Url url( toUTF8( urlText ) );
    _repoInfo.setBaseUrl( url );


    // Make sure we have a unique alias

    if ( _repoInfo.alias().empty() || _repoInfo.name() != oldRepoName  )
    {
        std::string alias = zypp::RepoManager::makeStupidAlias( url );
        _repoInfo.setAlias( alias );

        logDebug() << "New alias for "
                   << _repoInfo.name() << ": "
                   << alias
                   << endl;
    }
    else
    {
        logDebug() << "Leaving old alias for "
                   << _repoInfo.name() << ": "
                   << _repoInfo.alias()
                   << endl;
    }
}


void RepoEditDialog::showWarningPopup( const QString &         message,
                                       const zypp::Exception & exception )
{
    QMessageBox msgBox( this );

    msgBox.setText( message + "\n" + fromUTF8( exception.asString() ) );
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.addButton( QMessageBox::Ok );

    msgBox.exec();
}

