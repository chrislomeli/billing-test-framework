#ifndef _COCIDate
#define _COCIDate

#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef _COCIBase
#include "ocibase.h"
#endif

#ifndef _STRING_
#include <string>
#endif

class COCISession;
class COCITimeSpan;

class COCIDate : public COCIBase
{
public:
  COCIDate(const COCISession& Sess);
  COCIDate(const COCISession& Sess, dvoid* instance);

  COCIDate(const COCISession& Sess, const char* str, const char* fmt = NULL);
  COCIDate(const COCIDate& date);
  ~COCIDate();

  COCIDate& to_date(const char* str, const char* fmt = NULL);
  std::string to_text(const char* fmt = NULL) const;

  // Assignment
  COCIDate& operator = (const COCIDate& date);

  // Comparison
  bool operator == (const COCIDate& date) const;
  bool operator != (const COCIDate& date) const;

  friend bool operator>  (const COCIDate & lhs, const COCIDate & rhs);
  friend bool operator>= (const COCIDate & lhs, const COCIDate & rhs);
  friend bool operator<  (const COCIDate & lhs, const COCIDate & rhs);
  friend bool operator<= (const COCIDate & lhs, const COCIDate & rhs);

  COCIDate& operator+=(const COCITimeSpan& time);
  COCIDate& operator-=(const COCITimeSpan& time);

  friend COCIDate operator+(const COCIDate& lhs, const COCITimeSpan& rhs);
  friend COCIDate operator-(const COCIDate& lhs, const COCITimeSpan& rhs);


  friend COCITimeSpan operator-(const COCIDate& lhs, const COCIDate& rhs);

  COCIDate& operator = (char * str);

  bool is_between(const COCIDate& date1, const COCIDate& date2) const;

  // Adds or subtracts days from this date (day can be positive or negative)
  void add_days(sb4 day);

  // Adds or subtracts months from this date (month can be positive or negative)
  void add_months(sb4 month);

  // Checks to see if this date is valid
  // Returns the following
  // OCI_DATE_INVALID_DAY              0x1        Bad day
  // OCI_DATE_DAY_BELOW_VALID          0x2        Bad day low/high bit (1=low)
  // OCI_DATE_INVALID_MONTH            0x4        Bad month
  // OCI_DATE_MONTH_BELOW_VALID        0x8        Bad month low/high bit (1=low)
  // OCI_DATE_INVALID_YEAR             0x10       Bad year
  // OCI_DATE_YEAR_BELOW_VALID         0x20       Bad year low/high bit (1=low)
  // OCI_DATE_INVALID_HOUR             0x40       Bad hour
  // OCI_DATE_HOUR_BELOW_VALID         0x80       Bad hour low/high bit (1=low)
  // OCI_DATE_INVALID_MINUTE           0x100      Bad minute
  // OCI_DATE_MINUTE_BELOW_VALID       0x200      Bad minute Low/high bit (1=low)
  // OCI_DATE_INVALID_SECOND           0x400      Bad second
  // OCI_DATE_SECOND_BELOW_VALID       0x800      Bad second Low/high bit (1=low)
  // OCI_DATE_DAY_MISSING_FROM_1582    0x1000     Day is one of those "missing" from 1582
  // OCI_DATE_YEAR_ZERO                0x2000     Year may not equal zero
  // OCI_DATE_INVALID_FORMAT           0x8000     Bad date format input
  uword check() const;

  // Gets the number of days between this date and another date
  sb4  days_between(const COCIDate& other_date) const;

  // Gets the year, month, and day of this date
  void get_date(sb2& year, ub1& month, ub1& day) const;

  // Gets the hour, minute, second of this date
  void get_time(ub1& hour, ub1& min, ub1& sec) const;

  // Gets the date of the last day of this month
  COCIDate get_last_day(void) const;

  // Gets the date of the next day of the week, for this date
  // IN : day - First day of week named by this is returned
  COCIDate get_next_day(text* day) const;

  // Sets this date to year, month, and day
  void set_date(sb2 year, ub1 month, ub1 day);

  // Sets the time of this date to hour, minute, and second 
  void set_time(ub1 hour, ub1 min, ub1 sec);

  // Sets the time of this date from utc date 
  void set_datetime(time_t ltime);

  // Gets the system time and date
  static COCIDate sys_date(const COCISession& Sess);


  friend sword compare(const COCIDate& lhs, const COCIDate& rhs);



  OCIDate* get_oci_date(){return m_pDate;}
  const COCISession& get_session(){return m_Session;}

  bool is_null();
  void initialise();

private:
  const COCISession& m_Session;
  OCIDate* m_pDate;

  OCIDate m_Null_Date;
  char* m_Fmt;

};

#endif // _COCIDate
