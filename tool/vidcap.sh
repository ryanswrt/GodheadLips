#!/bin/sh
#this linux shell script will record a selected window and output both an OGV and a FLV with a frame size of selected window.
#required tools are recordmydesktop, mencoder and possibly ffmpeg and ogv tools
#http://recordmydesktop.sourceforge.net/about.php
#http://www.mplayerhq.hu
#http://xiph.org/
#http://www.ffmpeg.org/

#no sound is recorded
 
bitrate="500"

capwindow=`xwininfo |grep "Window id:"|sed -e "s/xwininfo\:\ Window id:\ // ;s/\ .*//"`
capwindoww="`xwininfo -stats |grep "Width:" | awk  '{print $2}'`"
capwindowh="`xwininfo -stats |grep "Height:" | awk  '{print $2}'`"
capwindowt="`xwininfo | grep '\"' | awk -F '\"' '{print $2}'`"

echo "$capwindow"
capname="$capwindow_$(date +%F_%0k.%M.%S)_$capwindoww"x"$capwindowh"
echo "$capname will be recorded.  ctrl+alt+s will stop recording"

recordmydesktop --windowid=$capwindow -o="$capname.ogv" --no-sound
mencoder -forceidx -of lavf -nosound -ovc lavc -lavcopts vcodec=flv:vbitrate=$bitrate:mbd=2:mv0:trell:v4mv:cbp:last_pred=3  -o "$capname.flv" "$capname.ogv"

echo "$capname.flv recorded of $capwindowt"

#this script is licenced GPLv3 2011 Francisco Athens http://www.gnu.org/licenses/gpl.html
