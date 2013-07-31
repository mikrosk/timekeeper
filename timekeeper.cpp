#include "timekeeper.h"

#include <QDebug>
#include <QStandardItemModel>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#include <QFile>
#include <QMessageBox>
#include <QDataStream>
#include <QDir>
#include <QList>

static QDataStream& operator<<( QDataStream& stream, const WorkTime& work )
{
	stream << work.clockInTime;
	stream << work.clockInDelay;
	stream << work.clockOutTime;
	stream << work.clockOutDelay;

	return stream;
}

static QDataStream& operator>>( QDataStream& stream, WorkTime& work )
{
	stream >> work.clockInTime;
	stream >> work.clockInDelay;
	stream >> work.clockOutTime;
	stream >> work.clockOutDelay;

	return stream;
}

TimeKeeper::TimeKeeper(QWidget *parent)
    : QDialog(parent, Qt::Window )
{
	ui.setupUi(this);

    setWindowFlags( ( windowFlags() | Qt::CustomizeWindowHint ) & ~Qt::WindowMaximizeButtonHint );

	// we expect there's always something
    QString dataPath = appDataPath();
	QDir dataPathDir( dataPath );
	dataPathDir.mkpath( dataPath );

    QFile file( dataPathDir.filePath( "timekeeper.db" ) );
	QDataStream stream( &file );
    if( file.open( QFile::ReadOnly ) )
	{
		stream >> m_times;
		file.close();
	}

	QDateTime now = QDateTime::currentDateTime();

	if( m_times.count() > 0 && m_times.first().clockInTime.date().weekNumber() < now.date().weekNumber() )
	{
		// new week, clear stats
		m_times.clear();
	}

#if 0
	// Mon - Wed
	WorkTime time;
	time.clockInDelay = 0;
	time.clockInTime = QDateTime( QDate( 2013, 6, 17 ), QTime( 8, 0 ) );
	time.clockOutDelay = 0;
	time.clockOutTime = QDateTime( QDate( 2013, 6, 17 ), QTime( 16, 30 ) );
	m_times.append( time );
	time.clockInDelay = 0;
	time.clockInTime = QDateTime( QDate( 2013, 6, 18 ), QTime( 8, 0 ) );
	time.clockOutDelay = 0;
	time.clockOutTime = QDateTime( QDate( 2013, 6, 18 ), QTime( 16, 30 ) );
	m_times.append( time );
	time.clockInDelay = 0;
	time.clockInTime = QDateTime( QDate( 2013, 6, 19 ), QTime( 8, 0 ) );
	time.clockOutDelay = 0;
	time.clockOutTime = QDateTime( QDate( 2013, 6, 19 ), QTime( 16, 30 ) );
	m_times.append( time );

	// Thu
	time.clockInDelay = 0;
	time.clockInTime = QDateTime( QDate( 2013, 6, 20 ), QTime( 7, 42 ) );
	time.clockOutDelay = 0;
	time.clockOutTime = QDateTime( QDate( 2013, 6, 20 ), QTime( 16, 20 ) );
	m_times.append( time );

	// Fri
	time.clockInDelay = 2;
	time.clockInTime = QDateTime( QDate( 2013, 6, 21 ), QTime( 8, 1 ) );
	time.clockOutDelay = 1;
	time.clockOutTime = QDateTime();
	m_times.append( time );
#endif

	if( m_times.count() == 0 || m_times.last().clockInTime.date() < now.date() )
	{
		//QMessageBox::information( 0, QApplication::applicationName(), "Your clock in time is " + now.toLocalTime().toString( Qt::SystemLocaleShortDate ) );
		WorkTime time;
		time.clockInTime = now;
		m_times.append( time );
	}
	else if( m_times.count() > 0 )
	{
		// reboot -> don't display clock out time (make it invalid)
		m_times.last().clockOutTime = QDateTime();
	}

	m_leaveTime = m_times.last().clockInTime.addSecs( 8 * 60 * 60 + 30 * 60 );
	connect( &m_timer, SIGNAL( timeout() ), this, SLOT( updateTimeLeft() ) );
	m_timer.setInterval( 500 );
	m_timer.start();

	// always take the last time as reference
	setBlueAndBoldText( ui.labelLeaveTime, leaveTimeWithDelays().time().toString( "hh:mm AP" ) );
	setBlackAndBoldText( ui.labelLeaveTimeWithOvertime, leaveTimeWithOvertime().time().toString( "(hh:mm AP)" ) );

	updateWeekOvertime();

	QStandardItemModel* model = new QStandardItemModel( 0, 5, this );
	model->setHorizontalHeaderItem( 0, new QStandardItem( QString( "Work Day" ) ) );
	model->setHorizontalHeaderItem( 1, new QStandardItem( QString( "Clock In Time" ) ) );
	model->setHorizontalHeaderItem( 2, new QStandardItem( QString( "Clock In Delay" ) ) );
	model->setHorizontalHeaderItem( 3, new QStandardItem( QString( "Clock Out Time" ) ) );
	model->setHorizontalHeaderItem( 4, new QStandardItem( QString( "Clock Out Delay" ) ) );

	foreach( WorkTime time, m_times )
	{
		QList<QStandardItem*> items;
		items.append( new QStandardItem( time.clockInTime.toString( "dddd" ) ) );
        items.last()->setEditable( false );
		items.append( new QStandardItem( time.clockInTime.toLocalTime().toString( "dd/MM/yy hh:mm AP" ) ) );
        items.last()->setEditable( false );
		items.append( new QStandardItem( QString( "%1" ).arg( time.clockInDelay ) ) );
		if( time.clockOutTime.isValid() )
		{
			items.append( new QStandardItem( time.clockOutTime.toLocalTime().toString( "dd/MM/yy hh:mm AP" ) ) );
            items.last()->setEditable( false );
		}
		else
		{
			items.append( new QStandardItem() );
		}
		items.append( new QStandardItem( QString( "%1" ).arg( time.clockOutDelay ) ) );
		model->appendRow( items );
	}

    connect( model, SIGNAL( itemChanged( QStandardItem* ) ), this, SLOT( updateEverything( QStandardItem* ) ) );

	ui.tableView->setModel( model );
#if QT_VERSION >= 0x050000
    ui.tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#else
    ui.tableView->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#endif
}

TimeKeeper::~TimeKeeper()
{

}

QString TimeKeeper::appDataPath() const
{
 #if QT_VERSION >= 0x050000
    return QStandardPaths::standardLocations( QStandardPaths::DataLocation ).first();
#else
    return QDesktopServices::storageLocation( QDesktopServices::DataLocation );
#endif
}

QDateTime TimeKeeper::leaveTimeWithDelays() const
{
	uint secs = m_leaveTime.toTime_t();
	secs -= m_times.last().clockInDelay * 60;
	secs -= m_times.last().clockOutDelay * 60;
	
	return QDateTime::fromTime_t( secs );
}

QDateTime TimeKeeper::leaveTimeWithOvertime() const
{
	qint64 secs = m_leaveTime.toTime_t();
	secs -= weekOvertime();
	// these are not considered in week overtime!
	secs -= m_times.last().clockInDelay * 60;
	secs -= m_times.last().clockOutDelay * 60;
	
	return QDateTime::fromTime_t( secs );
}

qint64 TimeKeeper::weekOvertime() const
{
	qint64 secs = 0;
	foreach( WorkTime time, m_times )
	{
		if( time.clockOutTime.isValid() )
		{
			secs += time.clockInTime.secsTo( time.clockOutTime );
			secs += time.clockInDelay * 60;
			secs += time.clockOutDelay * 60;
		}
	}

	// substract the number of days which have passed so far times 8.5 hours
	secs -= ( m_times.count() - 1 ) * ( 8 * 3600 + 3600 / 2 );

	return secs;
}

void TimeKeeper::updateWeekOvertime()
{
	qint64 secs = weekOvertime();

	bool negative = false;
	if( secs < 0 )
	{
		secs = -secs;
		negative = true;
	}

	qint32 hours = secs / 3600;
	qint32 mins = ( secs % 3600 ) / 60;

	if( negative )
	{
		setRedAndBoldText( ui.labelWeekOvertime, QString( "-%1 hour(s) %2 minute(s)" ).arg( hours ).arg( mins ) );
	}
	else
	{
		setGreenAndBoldText( ui.labelWeekOvertime, QString( "+%1 hour(s) %2 minute(s)" ).arg( hours ).arg( mins ) );
	}
}

void TimeKeeper::storeTimes()
{
    QString dataPath = appDataPath();
    QFile file( QDir( dataPath ).filePath( "timekeeper.db" ) );
	QDataStream stream( &file );
	
	m_times.last().clockOutTime = QDateTime::currentDateTime();
    if( file.open( QFile::WriteOnly ) )
	{
		stream << m_times;
		file.close();
	}
}

void TimeKeeper::updateEverything( QStandardItem* item )
{
    switch( item->column() )
    {
    case 2:
        m_times[item->row()].clockInDelay = item->text().toInt();
        break;
    case 4:
        m_times[item->row()].clockOutDelay = item->text().toInt();
        break;
    default:
        QMessageBox::warning( this, QApplication::applicationName(), "Illegal column");
    }

    setBlueAndBoldText( ui.labelLeaveTime, leaveTimeWithDelays().time().toString( "hh:mm AP" ) );
    setBlackAndBoldText( ui.labelLeaveTimeWithOvertime, leaveTimeWithOvertime().time().toString( "(hh:mm AP)" ) );

    updateWeekOvertime();
}

void TimeKeeper::setBlueAndBoldText( QLabel* label, const QString& text )
{
	label->setText( "<html><head/><body><p><span style=\"font-weight:600; color:#0000ff;\">" + text + "</span></p></body></html>" );
}

void TimeKeeper::setGreenAndBoldText( QLabel* label, const QString& text )
{
	label->setText( "<html><head/><body><p><span style=\"font-weight:600; color:#00aa00;\">" + text + "</span></p></body></html>" );
}

void TimeKeeper::setRedAndBoldText( QLabel* label, const QString& text )
{
	label->setText( "<html><head/><body><p><span style=\"font-weight:600; color:#ff0000;\">" + text + "</span></p></body></html>" );
}

void TimeKeeper::setBlackAndBoldText( QLabel* label, const QString& text )
{
	label->setText( "<html><head/><body><p><span style=\"font-weight:600; color:#000000;\">" + text + "</span></p></body></html>" );
}

void TimeKeeper::setBigBlackAndBoldText( QLabel* label, const QString& text )
{
	label->setText( "<html><head/><body><p><span style=\"font-size:18pt; font-weight:600;\">" + text + "</span></p></body></html>" );
}

void TimeKeeper::setBigRedAndBoldText( QLabel* label, const QString& text )
{
	label->setText( "<html><head/><body><p><span style=\"font-size:18pt; font-weight:600; color:#ff0000;\">" + text + "</span></p></body></html>" );
}

void TimeKeeper::updateTimeLeft()
{
	QDateTime now = QDateTime::currentDateTime();
	qint64 secsTotal = now.secsTo( leaveTimeWithOvertime() );
	bool negative = false;

	if( secsTotal < 0 )
	{
		secsTotal = -secsTotal;
		negative = true;
	}

	qint32 hours = secsTotal / 3600;
	qint32 mins = ( secsTotal % 3600 ) / 60;
	qint32 secs = ( secsTotal % 3600 ) % 60;

	if( negative )
	{
		setBigRedAndBoldText( ui.labelTimeLeft, QString( "-%1:%2:%3" ).arg( hours ).arg( mins, 2, 10, QLatin1Char( '0' ) ).arg( secs, 2, 10, QLatin1Char( '0' ) ) );
	}
	else
	{
		setBigBlackAndBoldText( ui.labelTimeLeft, QString( "%1:%2:%3" ).arg( hours ).arg( mins, 2, 10, QLatin1Char( '0' ) ).arg( secs, 2, 10, QLatin1Char( '0' ) ) );
	}
}
