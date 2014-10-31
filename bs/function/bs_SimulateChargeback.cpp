#include <bs/function/bs_SimulateChargeback.h>
#include <fsys/f_Dir.h>

bool
bs_SimulateChargeback::simulateChargeback(
  string trans_id,
  double amount,
  string reason_code,
  string category,	 // default - RECD
  time_t *transaction_date,
  string cb_pending_dir, // default - /var/juno/queue/fusa/chargebacks/pending
  string pending_root,   // default - rfr
  string pending_ext,	 // default - data
  string usage_code,	 // default - 1
  bool cb_reversal )	 // default - false
{
  ostringstream os;
  ostringstream msg;

  time_t ltime;
  pin_virtual_time(&ltime);

  if ( category.empty() )
    category.assign("RECD");

  if ( pending_root.empty() )
    pending_root.assign("chargebacks");

  if ( pending_ext.empty() )
    pending_ext.assign("data");

  //--set date string-------------------
  c_DateTime dater;
  dater.set(ltime);
  string str_date;
  dater.formatDateString(str_date, "MM/DD/YYYY");

  //--set transaction date string-------------------
  c_DateTime dater1;
  dater1.set(*transaction_date);
  string trans_date;
  dater1.formatDateString(trans_date, "MM/DD/YYYY");

  //--get amount just like -------------------
  double abs_amount = fabs( amount );
  char sign = ( cb_reversal ) ? '-' : ' ';  // negative for chargeback reversal, space for chargeback
  os << sign << abs_amount;
  string str_amount( os.str() );


  /*
  - cb pending_dir       /var/juno/queue/fusa/chargebacks/pending
  - cb pending_root      chargebacks
  - cb pending_ext       data
  */
  string str_fullpath;
  int fd;
  FILE * fp;


  //---------run root should exist-------------------------
  if ( ! util::Access(cb_pending_dir,F_EXISTS) ) {
                cout << ". Creating " <<  cb_pending_dir << " ..." << endl;
                if ( ! f_Dir::makePath(cb_pending_dir) ) {
                        cout << "Failed to create " <<  cb_pending_dir << " ..." << endl;
                        return false;
                }
  }


  // create file
  if ( util::createfile(
      str_fullpath, fd,
      cb_pending_dir.c_str(),
      (const char *)"chargeback",
      (const char *)"data",
      (const char *)"MMDDYYYY" ) < 1 )
  {
    msg << "Can not create file chargebacks.<date>.data in " << cb_pending_dir;
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
  fprintf(fp, "HPDE0017|99999|%s|%s|%s|00:00:01\n",
        str_date.c_str(),
        str_date.c_str(),
        str_date.c_str());
  fprintf(fp, "RPDE0017D|TD|75839|||USD|%s||85390147|\"%s\"|4444333322221111|%s|%s|%s|%s|%s||%s\n",
	category.c_str(),
        trans_id.c_str(),
        reason_code.c_str(),
        trans_date.c_str(),
        str_date.c_str(),
        str_date.c_str(),
        str_amount.c_str(),
	    usage_code.c_str());

  fprintf(fp, "*DFREND|PID=972331|FREQ=DAILY|CO=970699\n" );


  //--- cleanup ------------
  fclose(fp);
  return true;
}


