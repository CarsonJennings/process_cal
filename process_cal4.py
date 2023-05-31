#!/usr/bin/env python3

import datetime
import re

class process_cal:
    '''
    The process_cal class is a class that expects an ics file. When a process_cal object is created
    it will be transformed into a list of events from the corresponding ics file.
    '''

    def __init__(self, events):
        event_list = readfile(events)
        self.events = event_list

    def get_events_for_day(self, target_dt):
        '''
        returns a formatted string with all of the events and their information
        on the given datetime
        '''
        events_on_day = ''
        valid_events = 0

        for cur_event in self.events:
            if cur_event.start.date() == target_dt.date() and valid_events == 0:
                fstart = cur_event.start.strftime("%B %d, %Y (%a)")
                events_on_day += "{dt}\n".format(dt=fstart)
                fstart_tm = get_time(cur_event.start)
                fend_tm = get_time(cur_event.end)

                for i in range(len(fstart)):
                    events_on_day += "-"

                events_on_day += "\n"
                events_on_day += "{fst_tm} to {fed_tm}: {sum} {{{{{loc}}}}}".format(fst_tm=fstart_tm,
                    fed_tm=fend_tm, sum=cur_event.summary, loc=cur_event.location)
                valid_events += 1

            elif cur_event.start.date() == target_dt.date():
                fstart_tm = get_time(cur_event.start)
                fend_tm = get_time(cur_event.end)

                events_on_day += "\n"
                events_on_day += "{fst_tm} to {fed_tm}: {sum} {{{{{loc}}}}}".format(fst_tm=fstart_tm,
                    fed_tm=fend_tm, sum=cur_event.summary, loc=cur_event.location)
                valid_events += 1

            elif cur_event.start.date() > target_dt.date():
                break
        return events_on_day


class Event:
    '''
    The Event class creates an Event object which has multiple fields that store
    information about the Event. It holds datetimes for the start time, end time
    as well as the time an event will repeat to. Additionally it has fields for
    the Event summary and location
    '''

    def __init__(self, start=None, end=None, location='', summary='', rrule=None):
        self.start = start
        self.end = end
        self.location = location
        self.summary = summary
        self.rrule = rrule


def readfile(filename):
    '''
    The readfile function takes an ics file and reads through it line by line
    adding important information into temporary variable which is then used to
    create Event object that are then added to the event list. 
    '''
    fo = open(filename)
    lines = fo.readlines()

    event_arr = []
    cur_event = None

    for line in lines:
        start_time = re.search("DTSTART:(.+)", line)
        end_time = re.search("DTEND:(.+)", line)
        location = re.search("LOCATION:(.+)", line)
        summary = re.search("SUMMARY:(.+)", line)
        rrule = re.search("RRULE:(.+)UNTIL=(.+);(.+)", line)

        if line[:12] == 'BEGIN:VEVENT':
            cur_event = Event()
        elif start_time:
            cur_event.start = datetime.datetime(int(start_time.group(1)[:4]),
                int(start_time.group(1)[4:6]), int(start_time.group(1)[6:8]),
                int(start_time.group(1)[9:11]), int(start_time.group(1)[11:13]))

        elif end_time:
            cur_event.end = datetime.datetime(int(end_time.group(1)[:4]),
                int(end_time.group(1)[4:6]), int(end_time.group(1)[6:8]),
                int(end_time.group(1)[9:11]), int(end_time.group(1)[11:13]))

        elif location:
            cur_event.location = location.group(1)

        elif summary:
            cur_event.summary = summary.group(1)

        elif rrule:
            cur_event.rrule = datetime.datetime(int(rrule.group(2)[:4]),
                int(rrule.group(2)[4:6]), int(rrule.group(2)[6:8]),
                int(rrule.group(2)[9:11]), int(rrule.group(2)[11:13]))

        elif line[:10] == 'END:VEVENT':
            if (cur_event.rrule == None):
                event_arr = add_inorder(event_arr, cur_event)
            else:
                event_arr = add_repeating(event_arr, cur_event)


    fo.close()
    return event_arr

def add_repeating(event_arr, first_event):
    '''
    The add_repeating function is used when an event is being added that is supposed
    to repeat for a set amount of time. This function will correctly add all the
    repeated events to the event list
    '''
    repeat_to = first_event.rrule
    start_tm = first_event.start
    end_tm = first_event.end
    increment_wk = datetime.timedelta(weeks=1)

    while start_tm < repeat_to:
        new_event = Event(start_tm, end_tm, first_event.location, first_event.summary)
        event_arr = add_inorder(event_arr, new_event)
        start_tm += increment_wk
        end_tm += increment_wk

    return event_arr


def add_inorder(event_arr, cur_event):
    '''
    The add_inorder function takes the event_list and an event and adds the event
    into the event list in order based on the start times of the events.
    '''
    arr_len = len(event_arr)
    if arr_len == 0:
        event_arr.append(cur_event)
        return event_arr
    else:
        i=0
        while i < arr_len and cur_event.start > event_arr[i].start:
            i += 1

        event_arr.insert(i, cur_event)
        return event_arr

def get_time(cur_time):
    '''
    The get_time function takes a datetime and formats it into a 
    more readable 12 hour time which is then returned as a string
    '''
    format_time = cur_time.strftime("%I:%M %p")

    if format_time[0] == '0':
        format_time = ' ' + format_time[1:]
    return format_time