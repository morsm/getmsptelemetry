#! /bin/bash
#
# Loop script for MorseFlyer mk2

TELEM_EXEC=/usr/local/bin/getmsptelemetry
MOTOR_EXEC=/usr/local/bin/motorsrunning

WWW=/var/www/html
TELEMETRY_DIR=/var/telemetry
WEB_AJAX_TRIGGER=$WWW/status_telemetry.txt
MOTORS_FILE=$TELEMETRY_DIR/run/motors
TELEMETRY_CURRENT=$TELEMETRY_DIR/run/current_telemetry

TELEMETRY_WEB_RECORD=$WWW/telemetry/telemetry.xml
RASPIMJPEG_FIFO=$WWW/FIFO

START_VIDEO_COMMAND="ca 1"
STOP_VIDEO_COMMAND="ca 0"

MOTORS_WERE_RUNNING=1  # 1 means false
MOTORS_ARE_RUNNING=1

function start_telemetry_and_movie
{
	# Create new telemetry record
	TIMESTAMP=`date +%Y%m%d_%H%M%S`
	HOST=`hostname`
	TEL_FILE=$TELEMETRY_DIR/morseflyer_telemetry_$HOST_$TIMESTAMP.xml
	
	echo -e "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Telemetry xmlns=\"http://www.termors.net/xml/morseflyer/telemetry/v1\">\n" >$TEL_FILE
	echo $TEL_FILE >$TELEMETRY_CURRENT

    echo "Starting telemetry file $TEL_FILE"
	
	# Start movie
	echo $START_VIDEO_COMMAND >$RASPIMJPEG_FIFO
}

function stop_telemetry_and_movie
{
	#Close telemetry record
	if [ -f $TELEMETRY_CURRENT ]
	then
		TELEM_FILE=`cat $TELEMETRY_CURRENT`
		
		if [ -f $TELEM_FILE ]
		then
			echo "</Telemetry>" >>$TELEM_FILE
		fi

        echo "Closing telemetry file $TELEM_FILE"
		
		rm $TELEMETRY_CURRENT # Remove the file that points to the current record
	fi
	
	# Start movie
	echo $STOP_VIDEO_COMMAND >$RASPIMJPEG_FIFO
}

# Start execution. State machine: check if motors were running before and if they are running now
echo "MorseFlyer loop script started"

if [ -f $MOTORS_FILE ]
then
	MOTORS_WERE_RUNNING=`cat $MOTORS_FILE`
    if [ $MOTORS_WERE_RUNNING -eq 0 ]
    then
        echo "Motors were running upon prior execution"
    else
        echo "Motors were not running before"
    fi
fi

$MOTOR_EXEC
RETVAL=$?
if [ $RETVAL -eq 0 ]
then
	# Motors are running
	MOTORS_ARE_RUNNING=0

    if [ $MOTORS_WERE_RUNNING -eq 0 ]
    then
        echo "Motors were running upon prior execution"
    else
        echo "Motors were not running before"
    fi

fi

# Write current motor status to file
echo $MOTORS_ARE_RUNNING >$MOTORS_FILE

# Possible state change if motors are running now but weren't before
if [ $MOTORS_ARE_RUNNING -eq 0 ] && [ $MOTORS_WERE_RUNNING -ne 0 ]
then
    echo "Starting telemetry"
	start_telemetry_and_movie
fi

# Possible state change if motors were running before but aren't anymore
if [ $MOTORS_WERE_RUNNING -eq 0 ] && [ $MOTORS_ARE_RUNNING -ne 0 ]
then
    echo "Stopping telemetry"
	stop_telemetry_and_movie
fi

# Get current telemetry record
TELEMETRY=`$TELEM_EXEC`
if [ $? -eq 0 ]
then
	# Telemetry received successfully
	
	# Write to web file
	echo $TELEMETRY >$TELEMETRY_WEB_RECORD
	# Update AJAX
	echo -n "telemetry" >$WEB_AJAX_TRIGGER
	
	# If motors are running and there is a current telemetry record, append to that
	if [ $MOTORS_ARE_RUNNING -eq 0 ] && [ -f $TELEMETRY_CURRENT ]
	then
		TELEM_FILE=`cat $TELEMETRY_CURRENT`
		
		if [ -f $TELEM_FILE ]
		then
			echo $TELEMETRY >>$TELEM_FILE
		fi
	fi
fi

echo "MorseFlyer loop script ended."

