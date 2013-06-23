TimeKeeper 1.0
==============

A simple tool to tell you when to leave the office ;-)

Purpose
-------
when I moved to Australia, I (naturally) had to change my job as well. I
used to work as a contractor for a long time but unfortunately, my current
employer doesn't allow this anymore so I'm a regular employee again, after
all those years.

One of the biggest problems I've got here is to track my time, i.e. I must
stay in the office for 8.5 hours per day but we have no database or whatever
for tracking this, at least as far as employees are concerned.

So I've developed this little tool. I use it like this:

1. When computer starts, I start this app as well (I've got it in my startup
scripts called).

2. Clock In Delay is the difference between Clock In Time (the time the app
has started) and the time when you have entered the building.  So, if I
showed up at work at 8:00 AM and the app says the Clock In Time is 8:02 AM,
I want to set "2" as Clock In Delay, which is default.

3. The same goes for Clock Out Time/Delay, just remember to have the app
running when leaving the office / shutting down your computer for the last
time. Except this, you can reboot your computer and/or restart the app as
many times as you wish.

4. If there's an error, you can edit Clock In/Out Delay times, everything
will be recalculated.

5. Every week the table is cleared, to keep things simple.

Description
-----------
Work Day:	Derived from Clock In Time.
Clock In Time:	Time when the app has started.
Clock In Delay:	Number of minutes which have passed from entering the
		building to Clock In Time. Default = 2.
Clock Out Time:	Time when the app has finished. Only for completed work
		days.
Clock Out Delay:Number of minutes which (will) have passed from shutting
		down the computer (Clock Out Time) to exiting the building.
		Default = 1.
Recommended clock out time for today:
		The first value means the time when you can shut down your
		computer (i.e.  it takes the delays into account), the
		second value means the same minus this week's overtime.
This week's overtime:
		Difference between the expected every day's performance (8.5
		hours) and your actual performance.  Can be negative (you
		miss some time) or positive (you've got overtime).
The realtime counter:
		It shows you how much time is still left ;-) Essentially the
		same as the second recommended clock out time value minus
		Clock In Time.

Technical Details
-----------------
GPL v3, compatible with Qt4/Qt5, built on Windows and Linux.

Contact
-------
If you'd like to see a new feature or better documentation, drop me a line.
Right now I'm the only user so I don't care that much.

Miro Kropacek
miro.kropacek@gmail.com
