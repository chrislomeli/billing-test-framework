#ifndef _COCITimeSpan
#include "ocitime.h"
#endif

COCITimeSpan::COCITimeSpan(sb4 days, sb1 hours, sb1 minutes, sb1 seconds)
{
   m_Days = days;
  if((hours > 24) || (hours < -24)) 
  {
    // exception
  }
  m_Hours = hours;
  if((minutes > 60) || (minutes < -60)) 
  {
    // exception
  }
  m_Minutes = minutes;
  if((seconds > 60) || (seconds < -60)) 
  {
    // exception
  }
  m_Seconds = seconds;
}

COCITimeSpan::~COCITimeSpan()
{
}

COCITimeSpan::COCITimeSpan(const COCITimeSpan& src)
{
  *this = src;
}


void COCITimeSpan::set_hours(sb1 hours)
{
  if((hours > 24) || (hours < -24))
  {
    // exception
  }
  m_Hours = hours;
}

void COCITimeSpan::set_minutes(sb1 minutes)
{
  if((minutes > 60) || (minutes < -60))
  {
    // exception
  }
  m_Minutes = minutes;
}

void COCITimeSpan::set_seconds(sb1 seconds)
{
  if((seconds > 60) || (seconds < -60))
  {
    // exception
  }
  m_Seconds = seconds;
}


sb4 COCITimeSpan::get_total_seconds() const
{
  // If the number of days is larger than this, we possibly won't be
  // able to fit the interval into a sb4 so throw an exception
  if((get_days() > 24853) || (get_days() < -24853))
  {
    // exception
  }

  sb4 result = 0;

  result += get_seconds();
  result += (60 * get_minutes());
  result += (60 * 60 * get_hours());
  result += (24 * 60 * 60 * get_days());

  return result;
}

COCITimeSpan& COCITimeSpan::operator=(const COCITimeSpan& rhs)
{
  m_Days = rhs.m_Days;
  m_Hours = rhs.m_Hours;
  m_Minutes = rhs.m_Minutes;
  m_Seconds = rhs.m_Seconds;

  return *this;
}

COCITimeSpan& COCITimeSpan::operator+=(const COCITimeSpan& rhs)
{
  m_Seconds += rhs.m_Seconds;
  if(m_Seconds >= 60) {
    m_Seconds -= 60;
    m_Minutes++;
  }
  else if(m_Seconds < 0) {
    m_Seconds += 60;
    m_Minutes--;
  }

  m_Minutes += rhs.m_Minutes;
  if(m_Minutes >= 60) {
    m_Minutes -= 60;
    m_Hours++;
  }
  else if(m_Minutes < 0) {
    m_Minutes += 60;
    m_Hours--;
  }

  m_Hours += rhs.m_Hours;
  if(m_Hours >= 24) {
    m_Hours -= 24;
    m_Days++;
  }
  else if(m_Hours < 0) {
    m_Hours += 24;
    m_Days--;
  }

  m_Days += rhs.m_Days;

  return *this;
}


bool COCITimeSpan::is_zero() const
{
  return ((m_Days ==0) && (m_Hours == 0) && (m_Minutes == 0) && (m_Seconds == 0));
}