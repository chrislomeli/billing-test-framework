#ifndef _COCIDate
#include "ocidate.h"
#endif

#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef _COCITimeSpan
#include "ocitime.h"
#endif

const int DATSTRSIZ = 75;

COCIDate::COCIDate(const COCISession& Sess)
: m_Session(Sess)
, m_Fmt((char*)"DD-MON-YYYY HH24:MI:SS")
{
  m_Indicator = OCI_IND_NULL;

  m_pDate = (OCIDate*)malloc(sizeof(OCIDate));
  set_instance((void*&)m_pDate);

  // Initialise date to null
  initialise();

  // Keep a null date for comparison
  memset(&m_Null_Date, '\0', sizeof(OCIDate));
}

COCIDate::COCIDate(const COCISession& Sess, dvoid* instance)
: m_Session(Sess)
, m_Fmt((char*)"DD-MON-YYYY HH24:MI:SS")
{
  m_Indicator = OCI_IND_NOTNULL;

  m_pDate = (OCIDate*)malloc(sizeof(OCIDate));
  set_instance((void*&)m_pDate);
  memcpy(m_pDate, instance, sizeof(OCIDate));
}

COCIDate::COCIDate(const COCISession& Sess, const char* str, const char* fmt)
: m_Session(Sess)
, m_Fmt((char*)"DD-MON-YYYY HH24:MI:SS")
{
  m_Indicator = OCI_IND_NOTNULL;
  m_pDate = (OCIDate*)malloc(sizeof(OCIDate));
  set_instance((void*&)m_pDate);
  to_date(str,fmt);
}

COCIDate::COCIDate(const COCIDate& Date)
: m_Session(Date.m_Session)
{
  m_Indicator = Date.m_Indicator;

  *this = Date;
}


COCIDate::~COCIDate()
{
  free(m_pDate);
}

COCIDate& COCIDate::to_date(const char* str, const char* fmt)
{
  m_Indicator = OCI_IND_NOTNULL;
  ub4 strl = (ub4)strlen(str);
  ub1 fmtlen = 0;

  if(fmt == 0)
  {
    fmt = m_Fmt;
  }
  fmtlen = (ub1)strlen((const char*)fmt);

  CHECK(m_Session.get_error(),
        OCIDateFromText(m_Session.get_error(),
                        (text*)str, strl,
                        (text*)fmt, fmtlen,
                        (text*)0, 0,
                        (OCIDate *)m_pDate));

  return *this;
}

std::string COCIDate::to_text(const char* fmt) const
{
  char buf[DATSTRSIZ];
  memset(buf, '\0', DATSTRSIZ);
  ub4 i = DATSTRSIZ;

  ub1 fmtlen = 0;
  std::string fmtstr;

  if(fmt == 0)
  {
    fmt = m_Fmt;
    fmtlen = (ub1)strlen(fmt);
  }
  else
  {
    fmtlen = (ub1)strlen((const char*)fmt);
  }

  if(!((COCIDate&)*this).is_null())
  {
    CHECK(m_Session.get_error(),
          OCIDateToText(m_Session.get_error(),
                        (OCIDate *)m_pDate,
                        (text *)fmt, fmtlen,
                        (text *)0, 0,
                        &i, (text *)buf));
  }
  std::string str = buf;
  return str;
}

COCIDate & COCIDate::operator = (const COCIDate & date)
{
  m_Indicator = OCI_IND_NOTNULL;
  m_pDate = (OCIDate*)malloc(sizeof(OCIDate));
  set_instance((void*&)m_pDate);

  CHECK ( m_Session.get_error(),
          OCIDateAssign(m_Session.get_error(),
                        (OCIDate *)date.m_pDate,
                        (OCIDate *)m_pDate) )
  return *this;
}

bool COCIDate::operator == (const COCIDate & date) const
{
  sword result = compare(*this, date);
  return result ? false : true;
}

bool COCIDate::operator != (const COCIDate & date) const
{
  return !operator==(date);
}

bool operator>  (const COCIDate & lhs, const COCIDate & rhs)
{
  sword result = compare(lhs, rhs);
  return (result > 0);

}

bool operator>= (const COCIDate & lhs, const COCIDate & rhs) 
{
  sword result = compare(lhs, rhs);
  return (result >= 0);
}

bool operator<  (const COCIDate & lhs, const COCIDate & rhs)
{
  sword result = compare(lhs, rhs);
  return (result < 0);
}

bool operator<= (const COCIDate & lhs, const COCIDate & rhs)
{
  sword result = compare(lhs, rhs);
  return (result <= 0);
}

COCIDate& COCIDate::operator+=(const COCITimeSpan& time)
{
  if(!time.is_zero())
  {
    ub1 hour = 0;    // variables for this date/time
    ub1 min = 0;
    ub1 sec = 0;
    sb4 day = 0;
    get_time(hour, min, sec);

    // add the seconds
    sec += time.get_seconds();
    if(sec >= 60) {
      sec -= 60;
      min++;
    }
    else if(sec < 0) {
      sec += 60;
      min--;
    }

    // add the minutes
    min += time.get_minutes();
    if(min >= 60) {
      min -= 60;
      hour++;
    }
    else if(min < 0) {
      min += 60;
      hour--;
    }

    // add the hours
    hour += time.get_hours();
    if(hour >= 24) {
      hour -= 24;
      day++;
    }
    else if(hour < 0) {
      hour += 24;
      day--;
    }

    // days to add
    day += time.get_days();

    add_days(day);

    // we've made sure that these variables are within the bounds
    // of a ub1 so this is safe
    set_time(hour, min, sec);
  }

  return *this;
}

COCIDate& COCIDate::operator-=(const COCITimeSpan& time)
{
  if(!time.is_zero())
  {
    ub1 hour = 0;    // variables for this date/time
    ub1 min = 0;
    ub1 sec = 0;
    sb4 day = 0;
    get_time(hour, min, sec);

    sec -= time.get_seconds();
    if(sec < 0) {
      sec += 60;
      min--;
    }
    else if(sec >= 60) {
      sec -= 60;
      min++;
    }

    min -= time.get_minutes();
    if(min < 0) {
      min += 60;
      hour--;
    }
    else if(min >= 60) {
      min -= 60;
      hour++;
    }

    hour -= time.get_hours();
    if(hour < 0) {
      hour += 24;
      day--;
    }
    else if(hour >= 24) {
      hour -= 24;
      day++;
    }

    day -= time.get_days();

    add_days(day);
    set_time(hour, min, sec);
  }

  return *this;
}

COCIDate operator+(const COCIDate& lhs, const COCITimeSpan& rhs)
{
  COCIDate result(lhs);
  result.operator+=(rhs);
  return result;
}

COCIDate operator-(const COCIDate& lhs, const COCITimeSpan& rhs)
{
  COCIDate result(lhs);
  result.operator-=(rhs);
  return result;
}

COCITimeSpan operator-(const COCIDate& lhs, const COCIDate& rhs)
{
  COCITimeSpan result;                  // result to return

  // Decide which way round to perform operation
  COCIDate* plhs = &(COCIDate&)lhs;
  COCIDate* prhs = &(COCIDate&)rhs;
  if(lhs < rhs) {
    plhs = &(COCIDate&)rhs;
    prhs = &(COCIDate&)lhs;
  }
  // Short circuit if dates are equal
  else if(lhs == rhs) {
    return result;
  }

  sb4 carry = 0;
  sb4 part = 0;
  ub1 lhs_hour = 0;
  ub1 lhs_min = 0;
  ub1 lhs_sec = 0;
  ub1 rhs_hour = 0;
  ub1 rhs_min = 0;
  ub1 rhs_sec = 0;

  plhs->get_time(lhs_hour, lhs_min, lhs_sec);
  prhs->get_time(rhs_hour, rhs_min, rhs_sec);

  // Seconds
  part = lhs_sec - rhs_sec;
  if(part < 0) {
    part += 60;
    carry = 1;
  }
  result.set_seconds((ub1)part);

  // Minutes
  part = lhs_min - rhs_min - carry;
  if(part < 0) {
    part += 60;
    carry = 1;
  }
  else {
    carry = 0;
  }
  result.set_minutes((ub1)part);

  // Hours
  part = lhs_hour - rhs_hour - carry;
  if(part < 0) {
    part += 24;
    carry = 1;
  }
  else {
    carry = 0;
  }
  result.set_hours((ub1)part);

  part = lhs.days_between(rhs);
  part -= carry;
  result.set_days(part);

  return result;
}

COCIDate& COCIDate::operator = (char * str)
{
  to_date(str);
  return *this;
}



bool COCIDate::is_between(const COCIDate& date1, const COCIDate& date2) const
{
  bool retval = false;

  if(((date1 <= *this) && (*this <= date2)) ||
     ((date2 <= *this) && (*this <= date1)))
  {
    retval = true;
  }

  return retval;
}

void COCIDate::add_days(sb4 day)
{
  CHECK(m_Session.get_error(),
        OCIDateAddDays(m_Session.get_error(),
                       (OCIDate* )m_pDate,
                       day,
                       (OCIDate* )m_pDate));
}

void COCIDate::add_months(sb4 month)
{
  CHECK(m_Session.get_error(),
        OCIDateAddMonths(m_Session.get_error(),
                        (OCIDate* )m_pDate,
                        month,
                        (OCIDate* )m_pDate));
}

uword COCIDate::check() const
{
  uword valid;
  CHECK(m_Session.get_error(),
        OCIDateCheck(m_Session.get_error(),
                     (OCIDate *)m_pDate,
                     &valid));
  return valid;
}

sb4 COCIDate::days_between(const COCIDate& other_date) const
{
  sb4 days;
  CHECK(m_Session.get_error(),
        OCIDateDaysBetween(m_Session.get_error(), 
                          (OCIDate *)m_pDate,
                          (OCIDate *)&other_date.m_pDate,
                          &days));

  return days;
}

void COCIDate::get_date(sb2& year, ub1& month, ub1& day) const
{
  OCIDateGetDate((OCIDate *)m_pDate,
                 &year,
                 &month,
                 &day);
}

void COCIDate::get_time(ub1& hour, ub1& min, ub1& sec) const
{
  OCIDateGetTime((OCIDate *)m_pDate,
                 &hour,
                 &min,
                 &sec);
}

COCIDate COCIDate::get_last_day(void) const
{
  OCIDate last_day_date;
  CHECK(m_Session.get_error(),
        OCIDateLastDay(m_Session.get_error(),
                       (OCIDate *)m_pDate,
                       &last_day_date));

  return COCIDate(m_Session, (void *)&last_day_date);
}

COCIDate COCIDate::get_next_day(text* day) const
{
  OCIDate next_day_date;
  CHECK(m_Session.get_error(),
        OCIDateNextDay(m_Session.get_error(),
                       (OCIDate *)m_pDate,
                       day,
                       (ub4)strlen((char *)day),
                       &next_day_date));
  return COCIDate(m_Session, (void*)&next_day_date);
}


void COCIDate::set_datetime(time_t ltime)
{
//	this->to_date("12-AUG-2006 00:00:00", "DD-MON-YYYY HH24:MI:SS");

	struct tm * ptm = localtime(&ltime);

	sb2 year	= (ptm->tm_year + 1900); 
	ub1 month	= (ptm->tm_mon + 1); 
	ub1 day		= ptm->tm_mday;
	ub1 hour	= ptm->tm_hour;
	ub1 min		= ptm->tm_min;
	ub1 sec		= ptm->tm_sec;


	OCIDateSetTime((OCIDate *)m_pDate,
					hour,
					min,
					sec);

	OCIDateSetDate((OCIDate *)m_pDate,
					year,
					month,
					day);
}

void COCIDate::set_date(sb2 year, ub1 month, ub1 day)
{
  OCIDateSetDate((OCIDate *)m_pDate,
                 year,
                 month,
                 day);
}

void COCIDate::set_time(ub1 hour, ub1 min, ub1 sec)
{
  OCIDateSetTime((OCIDate *)m_pDate,
                 hour,
                 min,
                 sec);
}

COCIDate COCIDate::sys_date(const COCISession& Sess)
{
  OCIDate sysdate;
  CHECK(Sess.get_error(),
        OCIDateSysDate(Sess.get_error(),
                       &sysdate));

  return COCIDate(Sess, (void*)&sysdate);
}

///////////////////////
//
//  Compares two COCIDate's and returns:
//    0 - if they are equal
//   <0 - if LHS < RHS
//   >0 - if LHS > RHS
sword compare(const COCIDate& lhs, const COCIDate& rhs)
{
  sword result = 0;
  bool l = ((COCIDate&)lhs).is_null();
  bool r = ((COCIDate&)rhs).is_null();

  // If any of the numbers are null => result is not equal
  if(l || r)
  {
    result = !result;
  }
  else
  {
    CHECK( ((COCIDate&)lhs).get_session().get_error(),
             OCIDateCompare(((COCIDate&)lhs).get_session().get_error(),
                            (OCIDate *)(((COCIDate&)lhs).get_oci_date()),
                            (OCIDate *)(((COCIDate&)rhs).get_oci_date()),
                            &result) )
  }
  return result;
}


bool COCIDate::is_null()
{
  //return memcmp(&m_Null_Date, m_pDate, sizeof(OCIDate)) == 0;
  return m_Indicator = (memcmp(&m_Null_Date, m_pDate, sizeof(OCIDate)) == 0);
}

void COCIDate::initialise()
{
  memset(m_pDate, '\0', sizeof(OCIDate));
}

