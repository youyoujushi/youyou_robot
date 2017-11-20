#!/usr/bin/env python

""" do_task_node.py - Version 1.1 2013-12-20

    recv voice command,then do specified task

    Created for the Pi Robot Project: http://www.pirobot.org
    Copyright (c) 2012 Patrick Goebel.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.5
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details at:
    
    http://www.gnu.org/licenses/gpl.html
      
"""

import os
import rospy
from parse_cmd import *
from std_msgs.msg import *

        
if __name__ == '__main__':
    try:
        pcmd = parse_cmd(os.path.split(os.path.realpath(__file__))[0] + '/../cfg/commands.json')
        rospy.spin()
    except:
        pass
        

        
