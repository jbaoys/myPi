#!/usr/bin/python

import time
import sys
import curses

def keyloop(stdscr):
    #sleepTime = int(stdscr.getstr())
    while (1):
        time.sleep(0.1)
        curses.flushinp()
        stdscr.clear()
        stdscr.addstr("Key in:")
        doit = stdscr.getch()
        stdscr.addstr("\n")
        stdscr.addstr("Input ")
        stdscr.addch(doit)
        stdscr.refresh()
        print ("key = %u" % doit)
        if doit == ord('N') or doit == ord('n'):
            stdscr.addstr("Exiting.....\n")
            break

curses.wrapper(keyloop)
