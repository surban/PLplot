#----------------------------------------------------------------------------
# PLplot TK demos
# 
# Maurice LeBrun
# IFS, University of Texas at Austin
# 26-Jan-1995
#
# To plot these, start up plserver.  Type "source tkdemos.tcl", at which
# point a plframe extended widget is created.  Then type "1" for the first
# demo, "2" for the second, and so on.
#
# Note: each demo proc is supplied a widget argument if run from a widget,
# or "loopback" if being run from pltcl.  In the latter case, the
# "loopback cmd" does nothing, but is required two make the two styles of
# Tcl scripts compatible.
#
# $Id$
# $Log$
# Revision 1.3  2000/12/17 23:45:20  airwin
# correct the path for the tcl scripts
#
# Revision 1.2  1995/06/30 13:48:29  furnish
# Update loop limit to reflect the two new demo files.
#
# Revision 1.1  1995/01/27  02:52:54  mjl
# New front-end demo file for use from plserver.  It uses the same example
# program files as pltcl but passes the name of the widget.
#
#----------------------------------------------------------------------------

plstdwin .
plxframe .plw
pack append . .plw {left expand fill}

for {set i 1} {$i <= 18} {incr i} {
    set demo x[format "%02d" $i]
    source ../tcl/$demo.tcl
    proc $i {} "$demo .plw.plwin"
}
