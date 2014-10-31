#ifndef _COCITimeSpan
#define _COCITimeSpan

#ifndef _COCISession
#include "ocisess.h"
#endif

#ifndef OCI_ORACLE
#include "oci.h"
#endif

class COCITimeSpan
{
public:
  COCITimeSpan(sb4 days = 0, sb1 hours = 0, sb1 minutes = 0, 
               sb1 seconds = 0);

  ~COCITimeSpan();
  COCITimeSpan::COCITimeSpan(const COCITimeSpan& src);

  sb4 get_days() const              { return m_Days; }
  sb2 get_hours() const             { return m_Hours; }
  sb2 get_minutes() const           { return m_Minutes; }
  sb2 get_seconds() const           { return m_Seconds; }

  void set_days(sb4 days)           { m_Days = days; }
  void set_hours(sb1 hours);
  void set_minutes(sb1 minutes);
  void set_seconds(sb1 seconds);
  
  sb4 get_total_seconds() const;

  COCITimeSpan& operator=(const COCITimeSpan& rhs);
  COCITimeSpan& operator+=(const COCITimeSpan& rhs);


  bool is_zero() const;




private: 
  sb4   m_Days;
  sb1   m_Hours;
  sb1   m_Minutes;
  sb1   m_Seconds;
};

#endif
