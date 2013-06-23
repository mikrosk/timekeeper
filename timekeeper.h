#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <QDialog>
#include "ui_timekeeper.h"

#include <QDateTime>
#include <QVector>
#include <QTimer>

class QModelIndex;
class QStandardItem;

struct WorkTime
{
	WorkTime() : clockInDelay( 2 )
			   , clockOutDelay( 1 )
	{
	}
	QDateTime	clockInTime;
	quint8		clockInDelay;
	QDateTime	clockOutTime;
	quint8		clockOutDelay;
};

class TimeKeeper : public QDialog
{
	Q_OBJECT

public:
	TimeKeeper(QWidget *parent = 0);
	~TimeKeeper();

public slots:
	void storeTimes();
	void updateTimeLeft();
    void updateEverything( QStandardItem* item );

private:
    QString appDataPath() const;
	void setBlueAndBoldText( QLabel* label, const QString& text );
	void setBlackAndBoldText( QLabel* label, const QString& text );
	void setGreenAndBoldText( QLabel* label, const QString& text );
	void setRedAndBoldText( QLabel* label, const QString& text );
	void setBigBlackAndBoldText( QLabel* label, const QString& text );
	void setBigRedAndBoldText( QLabel* label, const QString& text );

	QDateTime leaveTimeWithDelays() const;
	QDateTime leaveTimeWithOvertime() const;
	qint64 weekOvertime() const;
	void updateWeekOvertime();

	Ui::TimeKeeperClass ui;

	QVector<WorkTime>	m_times;
	QDateTime			m_leaveTime;
	QTimer				m_timer;
};

#endif // TIMEKEEPER_H
