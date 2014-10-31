#include <bs/function/bs_SimulateACHReturn.h>
#include <fsys/f_Dir.h>

bool
bs_SimulateACHReturn::simulateACHReturn(
  string trans_id,
  double amount,
  string reason_code,
  string ach_pending_dir, //  /var/juno/queue/fusa/rfr/pending
  string pending_root,  //  rfr
  string pending_ext,   //  data
  bool refund )
{
  ostringstream os;
  ostringstream msg;

  time_t ltime;
  pin_virtual_time(&ltime);

  if ( pending_root.empty() )
    pending_root.assign("rfr");

  if ( pending_ext.empty() )
    pending_ext.assign("data");

  //--set date string-------------------
  c_DateTime dater;
  dater.set(ltime);
  string str_date;
  dater.formatDateString(str_date, "MM/DD/YYYY");

  //--get amount just like -------------------
  double abs_amount = fabs( amount );
  char sign = ( refund ) ? ' ' : '-';  // negative for payment, space for refund
  os << sign << abs_amount;
  string str_amount( os.str() );


  /*
  - ach pending_dir       /var/juno/queue/fusa/rfr/pending
  - ach pending_root      rfr
  - ach pending_ext       data
  */
  string str_fullpath;
  int fd;
  FILE * fp;


  //---------run root should exist-------------------------
  if ( ! util::Access(ach_pending_dir,F_EXISTS) ) {
                cout << ". Creating " <<  ach_pending_dir << " ..." << endl;
                if ( ! f_Dir::makePath(ach_pending_dir) ) {
                        cout << "Failed to create " <<  ach_pending_dir << " ..." << endl;
                        return false;
                }
  }


  // create file
  if ( util::createfile(
      str_fullpath, fd,
      ach_pending_dir.c_str(),
      (const char *)"rfr",
      (const char *)"data",
      (const char *)"MMDDYYYY" ) < 1 )
  {
    msg << "Can not create file rfr.<date>.data in " << ach_pending_dir;
    err_msg.assign(msg.str());
    return false;
  }
  // open as stream
  if ((fp = fdopen(fd,"w")) == NULL )
  {
    msg << "Can not re-open file " << str_fullpath << " as a stream ...";
    err_msg.assign(msg.str());
    return false;
    close(fd);
    return false;
  }

  /*
  *DFRBEG|PID=972331|FREQ=DAILY|CO=970699
  HPDE0018|970699|07/26/2004|07/26/2004|07/27/2004|02:05:38
  RPDE0018D|TD|75839|||USD|RTM||85390147|"T1,b36046,154"|0000002808|R67|07/20/2004|07/22/2004|07/26/2004| 38.35|1
  *DFREND|PID=972331|FREQ=DAILY|CO=970699
  */

  fprintf(fp, "*DFRBEG|PID=972331|FREQ=DAILY|CO=970699\n");
  fprintf(fp, "HPDE0018|99999|01/01/1970|01/01/1970|01/01/1970|00:00:01\n");
  fprintf(fp, "RPDE0018D|DD|75839|||USD|RTM||85390147|\"%s\"|0000002808|%s|%s|%s|%s|%s|1\n",
        trans_id.c_str(),
        reason_code.c_str(),
        str_date.c_str(),
        str_date.c_str(),
        str_date.c_str(),
        str_amount.c_str() );

  fprintf(fp, "*DFREND|PID=972331|FREQ=DAILY|CO=970699\n" );


  //--- cleanup ------------
  fclose(fp);
  return true;
}


